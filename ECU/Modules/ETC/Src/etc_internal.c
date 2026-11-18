/*
 * etc_internal.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#include "etc_internal.h"
#include "compiler.h"
#include "math_fast.h"
#include <float.h>
#include <math.h>
#include <string.h>

#define ETC_CONTROL_APPROACH_GAIN         (35.0f)
#define ETC_CONTROL_POSITION_MAX_SPEED    (180.0f)
#define ETC_CONTROL_BRAKING_DECEL          (8000.0f)
#define ETC_CONTROL_MOTION_SPEED_GAIN     (0.040f)
#define ETC_CONTROL_MOTION_NEAR            (2.0f)
#define ETC_CONTROL_MOTION_BLEND           (4.0f)
#define ETC_CONTROL_SPEED_FILTER_US       (4000.0f)
#define ETC_CONTROL_BRAKE_BLEND_SPEED      (40.0f)
#define ETC_CONTROL_MOTION_REARM            (3.0f)
#define ETC_CONTROL_MOTION_START_DISTANCE (6.0f)
#define ETC_CONTROL_MODEL_HANDOVER_TIME   (30 * TIME_US_IN_MS)
#define ETC_CONTROL_MODEL_REST_BAND       (0.30f)
#define ETC_CONTROL_MODEL_TARGET_BAND     (0.15f)
#define ETC_CONTROL_MODEL_FRICTION_RANGE  (0.50f)
#define ETC_CONTROL_MODEL_BREAKAWAY_SPEED (25.0f)
#define ETC_CONTROL_MODEL_REVERSE_SPEED   (5.0f)
#define ETC_CONTROL_REVERSAL_MIN_ERROR    (0.35f)
#define ETC_CONTROL_REVERSAL_MAX_ERROR    (5.0f)
#define ETC_CONTROL_REVERSAL_TRAVEL       (0.25f)
#define ETC_CONTROL_REVERSAL_RATE         (12.0f)
#define ETC_CONTROL_REVERSAL_TIME         (100 * TIME_US_IN_MS)
#define ETC_CONTROL_REVERSAL_RELEASE_TIME (15 * TIME_US_IN_MS)
#define ETC_CONTROL_SETTLE_WINDOW         (24 * TIME_US_IN_MS)
#define ETC_CONTROL_SETTLE_TRAVEL         (0.04f)
#define ETC_CONTROL_SETTLE_DEADBAND       (0.10f)
#define ETC_CONTROL_SETTLE_MAX_ERROR      (1.0f)
#define ETC_CONTROL_SETTLE_TARGET_CHANGE  (0.05f)
#define ETC_CONTROL_SETTLE_GAIN           (2.5f)
#define ETC_CONTROL_SETTLE_EFFORT_LIMIT   (0.45f)
#define ETC_CONTROL_REACH_PROGRESS        (0.20f)
#define ETC_CONTROL_REACH_MIN_PROGRESS    (0.50f)

bool etc_internal_number_valid(float value)
{
  uint32_t bits;

  /* The firmware is also built with fast-math. */
  memcpy(&bits, &value, sizeof(bits));
  return (bits & 0x7f800000u) != 0x7f800000u;
}

static bool etc_internal_pid_valid(const math_pid_koffs_t *pid)
{
  return etc_internal_number_valid(pid->Kp) && pid->Kp >= 0.0f &&
      etc_internal_number_valid(pid->Ki) && pid->Ki >= 0.0f &&
      etc_internal_number_valid(pid->Kd) && pid->Kd >= 0.0f;
}

static bool etc_internal_scale_valid(float scale, float min, float max)
{
  return etc_internal_number_valid(scale) && scale >= min && scale <= max;
}

static bool etc_internal_has_reversal(const etc_adaptation_config_t *config)
{
  const bool valid = true;

  return memcmp(&config->reversal_valid, &valid, sizeof(valid)) == 0;
}

bool etc_internal_adaptation_config_valid(const etc_adaptation_config_t *config)
{
  const bool valid = true;

  /* Read the persisted representation before using it as a bool. */
  if(memcmp(&config->valid, &valid, sizeof(valid)) != 0 || config->tps_average_samples > ETC_ADAPTATION_TPS_AVERAGE_MAX ||
      !etc_internal_pid_valid(&config->pid_position) || !etc_internal_pid_valid(&config->pid_speed) ||
      !etc_internal_scale_valid(config->spring_scale_lower, 0.95f, 1.05f) ||
      !etc_internal_scale_valid(config->spring_scale_upper, 0.95f, 1.05f) ||
      !etc_internal_scale_valid(config->friction_scale_lower, 0.75f, 1.25f) ||
      !etc_internal_scale_valid(config->friction_scale_upper, 0.75f, 1.25f)) {
    return false;
  }

  for(uint8_t i = 0; i < ETC_ADAPTATION_POINTS; i++) {
    const etc_adaptation_point_t *point = &config->points[i];
    const etc_adaptation_reversal_t *reversal = &config->reversal[i];
    if(!etc_internal_number_valid(point->position) || point->position < 0.0f || point->position > 100.0f ||
        !etc_internal_number_valid(point->spring) || fabsf(point->spring) > 16.0f ||
        !etc_internal_number_valid(point->friction) || point->friction < 0.0f || point->friction > 16.0f ||
        (i > 0 && point->position - config->points[i - 1].position < 0.5f)) {
      return false;
    }
    if(etc_internal_has_reversal(config) &&
        (!etc_internal_number_valid(reversal->opening) || fabsf(reversal->opening) > 16.0f ||
            !etc_internal_number_valid(reversal->closing) || fabsf(reversal->closing) > 16.0f ||
            reversal->opening < reversal->closing)) {
      return false;
    }
  }

  return true;
}

static uint8_t etc_internal_model_interval(const etc_adaptation_config_t *model, float position, float *fraction)
{
  const etc_adaptation_point_t *points = model->points;
  uint8_t i = 1;

  while(i < ETC_ADAPTATION_POINTS - 1 && position > points[i].position) {
    i++;
  }
  *fraction = CLAMP((position - points[i - 1].position) / (points[i].position - points[i - 1].position), 0.0f, 1.0f);
  return i;
}

static float etc_internal_feedforward(etc_ctx_t *ctx, const etc_adaptation_config_t *model, float target, float speed)
{
  const etc_adaptation_point_t *points = model->points;
  const etc_adaptation_point_t *rest_lower = &points[ETC_ADAPTATION_POINT_DEFAULT_LOWER];
  const etc_adaptation_point_t *rest_upper = &points[ETC_ADAPTATION_POINT_DEFAULT_UPPER];
  float position = ctx->position_current;
  float rest_half_width = (rest_upper->position - rest_lower->position) * 0.5f;
  float rest = rest_lower->position + rest_half_width;
  float rest_band = MIN(ETC_CONTROL_MODEL_REST_BAND, rest_half_width * 0.5f);
  float error = target - position;
  float direction = copysignf(1.0f, error);
  float fraction, spring, friction, directional_effort, rest_weight, target_weight, speed_weight, trim_fraction;
  uint8_t i = etc_internal_model_interval(model, position, &fraction);
  spring = points[i - 1].spring + (points[i].spring - points[i - 1].spring) * fraction;
  friction = points[i - 1].friction + (points[i].friction - points[i - 1].friction) * fraction;

  /* The two points around the parked plate are placed symmetrically by the FSM. */
  rest_weight = CLAMP((fabsf(position - rest) - rest_band) / (rest_half_width - rest_band), 0.0f, 1.0f);
  if(position > rest_lower->position && position < rest_upper->position) {
    spring = (position < rest ? rest_lower->spring : rest_upper->spring) * rest_weight;
  }

  trim_fraction = (position - rest_lower->position) / (rest_upper->position - rest_lower->position);
  trim_fraction = CLAMP(trim_fraction, 0.0f, 1.0f);
  spring *= BLEND(model->spring_scale_lower, model->spring_scale_upper, trim_fraction);
  friction *= BLEND(model->friction_scale_lower, model->friction_scale_upper, trim_fraction);

  /* The midpoint and half-width encode separate measured opening and closing thresholds.
   * Blend toward the requested threshold at breakaway and leave passive friction when braking. */
  directional_effort = spring + direction * friction;
  target_weight = CLAMP((fabsf(error) - ETC_CONTROL_MODEL_TARGET_BAND) / ETC_CONTROL_MODEL_FRICTION_RANGE, 0.0f, 1.0f);
  speed_weight = 1.0f - CLAMP(MAX(fabsf(speed) / ETC_CONTROL_MODEL_BREAKAWAY_SPEED,
      -speed * direction / ETC_CONTROL_MODEL_REVERSE_SPEED), 0.0f, 1.0f);
  return BLEND(spring, directional_effort, rest_weight * target_weight * speed_weight);
}

static bool etc_internal_reversal(etc_ctx_t *ctx, const etc_adaptation_config_t *model, float error, float speed,
    float limit, time_delta_us_t delta, time_us_t now)
{
  etc_reversal_ctx_t *reversal = &ctx->reversal;
  float position = ctx->position_current;
  float displacement = position - reversal->position;
  int8_t direction = error > 0.0f ? 1 : -1;

  if(fabsf(displacement) >= ETC_CONTROL_REVERSAL_TRAVEL) {
    reversal->last_direction = displacement > 0.0f ? 1 : -1;
    reversal->position = position;
  }

  if(reversal->direction != 0) {
    if(error * reversal->direction <= ETC_CONTROL_MODEL_TARGET_BAND || fabsf(error) > ETC_CONTROL_REVERSAL_MAX_ERROR) {
      reversal->direction = 0;
      return false;
    }
    if((position - reversal->start_position) * reversal->direction >= ETC_CONTROL_REVERSAL_TRAVEL ||
        speed * reversal->direction >= ETC_CONTROL_MODEL_REVERSE_SPEED ||
        time_diff(now, reversal->time) >= ETC_CONTROL_REVERSAL_TIME) {
      reversal->release_direction = reversal->direction;
      reversal->direction = 0;
      reversal->time = now;
      return false;
    }
  } else {
    if(direction == reversal->last_direction || reversal->last_direction == 0 ||
        fabsf(error) < ETC_CONTROL_REVERSAL_MIN_ERROR || fabsf(error) > ETC_CONTROL_REVERSAL_MAX_ERROR ||
        fabsf(speed) >= ETC_CONTROL_MODEL_REVERSE_SPEED) {
      return false;
    }

    float fraction;
    uint8_t i = etc_internal_model_interval(model, position, &fraction);
    const etc_adaptation_reversal_t *lower = &model->reversal[i - 1];
    const etc_adaptation_reversal_t *upper = &model->reversal[i];
    float threshold = direction > 0 ? BLEND(lower->opening, upper->opening, fraction) :
        BLEND(lower->closing, upper->closing, fraction);
    float duty = ctx->data.dutycycle;

    /* Consume the reversal once, even if the drive stops before take-up completes. */
    reversal->last_direction = 0;
    reversal->position = position;
    reversal->effort = duty * fabsf(duty) * ctx->power_voltage;
    reversal->threshold = CLAMP(threshold, -limit, limit);
    if((reversal->threshold - reversal->effort) * direction <= 0.0f) {
      return false;
    }
    reversal->direction = direction;
    reversal->release_direction = 0;
    reversal->start_position = position;
    reversal->time = now;
    ctx->motion_state = ETC_MOTION_IDLE;
  }

  float effort_delta = ETC_CONTROL_REVERSAL_RATE * (float)delta * TIME_S_IN_US;
  reversal->effort += CLAMP(reversal->threshold - reversal->effort, -effort_delta, effort_delta);
  return true;
}

static float etc_internal_settle_effort(etc_ctx_t *ctx, const etc_adaptation_config_t *model, float target, time_us_t now)
{
  etc_settle_ctx_t *settle = &ctx->settle;
  const etc_adaptation_point_t *points = model->points;
  float rest = (points[ETC_ADAPTATION_POINT_DEFAULT_LOWER].position +
      points[ETC_ADAPTATION_POINT_DEFAULT_UPPER].position) * 0.5f;
  time_delta_us_t elapsed = time_diff(now, settle->window_time);

  if(fabsf(target - settle->target) > ETC_CONTROL_SETTLE_TARGET_CHANGE ||
      target < points[0].position || target > points[ETC_ADAPTATION_POINTS - 1].position ||
      fabsf(target - rest) <= ETC_CONTROL_MODEL_REST_BAND) {
    *settle = (etc_settle_ctx_t){.target = target, .window_time = now};
    return 0.0f;
  }

  if(ctx->reversal.direction != 0 || ctx->reversal.release_direction != 0) {
    settle->previous_valid = false;
    settle->position_sum = 0.0f;
    settle->samples = 0;
    settle->window_time = now;
    return settle->effort;
  }

  settle->position_sum += ctx->position_current;
  settle->samples++;
  if(elapsed >= ETC_CONTROL_SETTLE_WINDOW) {
    float position = settle->position_sum / settle->samples;
    float error = target - position;
    float distance = fabsf(error);

    /* Consecutive TPS averages separate a static error from creep and single-sample noise. */
    if(settle->previous_valid && fabsf(position - settle->previous_position) <= ETC_CONTROL_SETTLE_TRAVEL &&
        distance > ETC_CONTROL_SETTLE_DEADBAND && distance <= ETC_CONTROL_SETTLE_MAX_ERROR) {
      float correction = copysignf(distance - ETC_CONTROL_SETTLE_DEADBAND, error);
      correction *= ETC_CONTROL_SETTLE_GAIN * ETC_CONTROL_SETTLE_WINDOW * TIME_S_IN_US;
      settle->effort = CLAMP(settle->effort + correction, -ETC_CONTROL_SETTLE_EFFORT_LIMIT, ETC_CONTROL_SETTLE_EFFORT_LIMIT);
    }
    settle->previous_position = position;
    settle->previous_valid = true;
    settle->window_time = now;
    settle->position_sum = 0.0f;
    settle->samples = 0;
  }

  return settle->effort;
}

void etc_internal_reset_control(etc_ctx_t *ctx, time_us_t now)
{
  math_pid_reset(&ctx->pid_position, now);
  math_pid_reset(&ctx->pid_speed, now);
  ctx->pid_position.Output = 0.0f;
  ctx->pid_speed.Output = 0.0f;
  ctx->pid_position.Current = ctx->position_current;
  ctx->pid_speed.Current = ctx->current_speed;
  ctx->target_speed = 0.0f;
  ctx->motion_state = ETC_MOTION_IDLE;
  ctx->reversal = (etc_reversal_ctx_t){.position = ctx->position_current};
  ctx->settle = (etc_settle_ctx_t){.window_time = now};
  ctx->pos_reach_position = ctx->position_current;
  ctx->pos_reach_time = now;
}

static float etc_internal_pid_update(math_pid_ctx_t *pid, float input, time_us_t now)
{
  float previous_integral = CLAMP(pid->I, pid->ClampFrom, pid->ClampTo);
  float output = math_pid_update(pid, input, now);

  if((output >= pid->ClampTo && pid->I > previous_integral) ||
      (output <= pid->ClampFrom && pid->I < previous_integral)) {
    pid->I = previous_integral;
    pid->Output = CLAMP(pid->P + pid->I + pid->D, pid->ClampFrom, pid->ClampTo);
  }

  return pid->Output;
}

static float etc_internal_motion_effort(etc_ctx_t *ctx, float target, float error, float effort, float feedforward,
    float limit, time_delta_us_t delta)
{
  float distance = fabsf(error);
  float speed, remaining, target_speed, motion_effort, speed_weight, distance_weight, weight;
  float average_delay, delay;

  if(ctx->motion_state == ETC_MOTION_IDLE || fabsf(target - ctx->motion_target) >= ETC_CONTROL_MOTION_REARM) {
    ctx->motion_target = target;
    ctx->motion_direction = copysignf(1.0f, error);
    ctx->motion_state = distance >= ETC_CONTROL_MOTION_START_DISTANCE ? ETC_MOTION_ACCELERATE : ETC_MOTION_SETTLE;
  }
  if(ctx->motion_state == ETC_MOTION_SETTLE) {
    return effort;
  }

  speed = ctx->current_speed * ctx->motion_direction;
  average_delay = 0.5f * (float)(MAX(ctx->tps_average.window, 1u) - 1u);
  delay = (average_delay * (float)delta + ETC_CONTROL_SPEED_FILTER_US) * TIME_S_IN_US;
  remaining = MAX(0.0f, error * ctx->motion_direction - ETC_CONTROL_MOTION_NEAR - MAX(speed, 0.0f) * delay);
  target_speed = fast_sqrt(2.0f * ETC_CONTROL_BRAKING_DECEL * remaining);
  if(ctx->motion_state == ETC_MOTION_ACCELERATE && (speed >= target_speed || remaining == 0.0f)) {
    ctx->motion_state = ETC_MOTION_BRAKE;
  }

  if(ctx->motion_state == ETC_MOTION_BRAKE) {
    if(speed <= ETC_CONTROL_BRAKE_BLEND_SPEED) {
      ctx->motion_state = ETC_MOTION_SETTLE;
      return effort;
    }
    /* A fixed target gets one launch. Braking cannot reapply the launch torque. */
    target_speed = MIN(target_speed, speed);
  }

  motion_effort = feedforward + ctx->motion_direction * ETC_CONTROL_MOTION_SPEED_GAIN * (target_speed - speed);
  speed_weight = (fabsf(speed) - ETC_CONTROL_BRAKE_BLEND_SPEED) / ETC_CONTROL_BRAKE_BLEND_SPEED;
  distance_weight = (distance - ETC_CONTROL_MOTION_NEAR) / ETC_CONTROL_MOTION_BLEND;
  weight = CLAMP(MAX(speed_weight, distance_weight), 0.0f, 1.0f);

  ctx->target_speed = BLEND(ctx->target_speed, ctx->motion_direction * target_speed, weight);

  return BLEND(effort, CLAMP(motion_effort, -limit, limit), weight);
}

static const etc_adaptation_config_t *etc_internal_get_active_model(const etc_ctx_t *ctx)
{
  if(ctx->config.adaptation.valid && !ctx->config.adaptation_setup.disable_model) {
    return &ctx->config.adaptation;
  }

  return NULL;
}

float etc_internal_control(etc_ctx_t *ctx, const etc_adaptation_config_t *model, float target, float limit, time_us_t now)
{
  float effort, speed_limit, feedforward = 0.0f;
  float error = target - ctx->position_current;
  time_delta_us_t delta = time_diff(now, ctx->pid_position.LastTime);
  bool fast = model != NULL && (ctx->adaptation.status == E_AGAIN || !ctx->config.adaptation_setup.disable_pid_tuning);
  float speed = fast ? ctx->current_speed : ctx->model_speed;

  math_pid_set_koffs(&ctx->pid_position, fast ? &model->pid_position : &ctx->config.pid_position);
  math_pid_set_koffs(&ctx->pid_speed, fast ? &model->pid_speed : &ctx->config.pid_speed);

  if(model != NULL) {
    feedforward = etc_internal_feedforward(ctx, model, target, speed);
    feedforward = CLAMP(feedforward, -limit, limit);
  } else if(ctx->position_current < ctx->data.default_position - 0.3f) {
    feedforward = -0.3f;
  } else if(ctx->position_current > ctx->data.default_position + 0.3f) {
    feedforward = 0.3f;
  }

  if(fast) {
    speed_limit = MIN(ETC_CONTROL_POSITION_MAX_SPEED, 3.0f + ETC_CONTROL_APPROACH_GAIN * fabsf(error));
    math_pid_set_clamp(&ctx->pid_position, -speed_limit, speed_limit);
    math_pid_set_clamp(&ctx->pid_speed, -limit - feedforward, limit - feedforward);
  } else {
    math_pid_set_clamp(&ctx->pid_position, -FLT_MAX, FLT_MAX);
    math_pid_set_clamp(&ctx->pid_speed, -FLT_MAX, FLT_MAX);
  }

  bool use_reversal = model != NULL && etc_internal_has_reversal(model);
  if(delta == 0) {
    if(use_reversal && ctx->reversal.direction != 0) {
      return CLAMP(ctx->reversal.effort, -limit, limit);
    }
    return CLAMP(ctx->pid_speed.Output + feedforward, -limit, limit);
  }

  bool reversing = use_reversal && etc_internal_reversal(ctx, model, error, speed, limit, delta, now);
  float position_ki = ctx->pid_position.K.Ki;
  float speed_ki = ctx->pid_speed.K.Ki;
  if(reversing) {
    ctx->pid_position.K.Ki = 0.0f;
    ctx->pid_speed.K.Ki = 0.0f;
  }

  math_pid_set_target(&ctx->pid_position, target);
  ctx->target_speed = fast ? etc_internal_pid_update(&ctx->pid_position, ctx->position_current, now) :
      math_pid_update(&ctx->pid_position, ctx->position_current, now);

  math_pid_set_target(&ctx->pid_speed, ctx->target_speed);
  effort = fast ? etc_internal_pid_update(&ctx->pid_speed, ctx->current_speed, now) :
      math_pid_update(&ctx->pid_speed, ctx->current_speed, now);
  if(reversing) {
    ctx->pid_position.K.Ki = position_ki;
    ctx->pid_speed.K.Ki = speed_ki;
    return CLAMP(ctx->reversal.effort, -limit, limit);
  }
  effort += feedforward;
  if(fast) {
    effort = etc_internal_motion_effort(ctx, target, error, effort, feedforward, limit, delta);
  }

  if(use_reversal && ctx->reversal.release_direction != 0) {
    etc_reversal_ctx_t *reversal = &ctx->reversal;
    time_delta_us_t elapsed = time_diff(now, reversal->time);
    if(elapsed >= ETC_CONTROL_REVERSAL_RELEASE_TIME || error * reversal->release_direction <= ETC_CONTROL_MODEL_TARGET_BAND ||
        fabsf(error) > ETC_CONTROL_REVERSAL_MAX_ERROR) {
      reversal->release_direction = 0;
    } else if((effort - reversal->effort) * reversal->release_direction > 0.0f) {
      /* Blend back to PID without delaying a braking command. */
      effort = BLEND(reversal->effort, effort, (float)elapsed / ETC_CONTROL_REVERSAL_RELEASE_TIME);
    }
  }

  return CLAMP(effort, -limit, limit);
}

static float etc_internal_tps_average(etc_ctx_t *ctx, float position)
{
  etc_tps_average_ctx_t *average = &ctx->tps_average;
  uint8_t samples = 1;

  if(ctx->adaptation.status == E_AGAIN) {
    samples = ctx->adaptation.setup.tps_average_samples;
  } else if(ctx->runtime_model_active && !ctx->runtime_base_pid) {
    samples = ctx->config.adaptation.tps_average_samples;
  }
  if(samples == 0) {
    samples = ETC_ADAPTATION_TPS_AVERAGE_DEFAULT;
  }

  if(average->window != samples || ctx->process_time == 0) {
    average->window = samples;
    average->count = 0;
    average->index = 0;
    average->sum = 0.0f;
  }

  if(average->count < samples) {
    average->count++;
  } else {
    average->sum -= average->samples[average->index];
  }
  average->sum += position;
  average->samples[average->index] = position;
  if(++average->index >= samples) {
    average->index = 0;
  }

  return average->sum / average->count;
}

error_t etc_internal_sample(etc_ctx_t *ctx, time_us_t now)
{
  error_t err = E_OK;
  time_delta_us_t time_delta = time_diff(now, ctx->process_time);
  input_value_t input_analog_value;
  float speed;
  l9960_status_openload_t motor_openload;

  do {
    err = ecu_sensors_tps_get_value(ctx->config.sensor_tps, &ctx->tps_data);
    ctx->diag.bits.tps_handle_error = err != E_OK && err != E_AGAIN;
    BREAK_IF(err != E_OK);

    ctx->diag.bits.tps_data_validity_error = !ctx->tps_data.data_valid;
    ctx->diag.bits.tps_pos_validity_error = !ctx->tps_data.position_valid ||
        !etc_internal_number_valid(ctx->tps_data.position) || !etc_internal_number_valid(ctx->tps_data.position_unfiltered);
    BREAK_IF_ACTION(ctx->diag.bits.tps_data_validity_error || ctx->diag.bits.tps_pos_validity_error, err = E_FAULT);

    err = ecu_sensors_tps_get_diag(ctx->config.sensor_tps, &ctx->tps_diag);
    ctx->diag.bits.tps_handle_error = err != E_OK && err != E_AGAIN;
    BREAK_IF(err != E_OK);
    ctx->diag.bits.tps_diag_error = ctx->tps_diag.data != 0;
    BREAK_IF_ACTION(ctx->diag.bits.tps_diag_error, err = E_FAULT);

    ctx->data.current_position = ctx->tps_data.position;
    if(time_delta != 0) {
      ctx->position_prev = ctx->position_current;
      ctx->position_current = etc_internal_tps_average(ctx, ctx->tps_data.position_unfiltered);
      speed = (ctx->position_current - ctx->position_prev) * (1000000.0f / (float)time_delta);
      if(ctx->process_time == 0) {
        ctx->current_speed = 0.0f;
        ctx->model_speed = 0.0f;
      } else if(ctx->adaptation.status == E_AGAIN || (ctx->runtime_model_active && !ctx->runtime_base_pid)) {
        ctx->current_speed += (speed - ctx->current_speed) * ((float)time_delta / ((float)time_delta + ETC_CONTROL_SPEED_FILTER_US));
      } else {
        ctx->current_speed = speed;
        if(ctx->runtime_base_pid) {
          ctx->model_speed += (speed - ctx->model_speed) * ((float)time_delta / ((float)time_delta + ETC_CONTROL_SPEED_FILTER_US));
        }
      }
      ctx->process_time = now;
    }

    err = input_get_value(ctx->power_voltage_pin, &input_analog_value, NULL);
    BREAK_IF(err != E_OK);
    ctx->power_voltage = (float)input_analog_value * INPUTS_ANALOG_MULTIPLIER_R;
    BREAK_IF_ACTION(ctx->power_voltage <= 0.0f, err = E_BADVALUE);

    err = ecu_devices_motor_get_diag(ctx->config.device_motor, &ctx->motor_diag);
    ctx->diag.bits.motor_handle_error = err != E_OK && err != E_AGAIN;
    BREAK_IF(err != E_OK);

    motor_openload = ctx->motor_diag.bits.ol;
    ctx->motor_diag.bits.ol = 0;
    ctx->diag.bits.motor_error = ctx->motor_diag.data != 0 || motor_openload == L9960_STATUS_OL_OPENLOAD;
    BREAK_IF_ACTION(ctx->diag.bits.motor_error, err = E_FAULT);

  } while(0);

  return err;
}

error_t etc_internal_drive(etc_ctx_t *ctx, bool enabled, float effort)
{
  error_t err = E_OK;
  error_t disable_err;
  float dutycycle = 0.0f;

  if(enabled) {
    if(!etc_internal_number_valid(effort)) {
      err = E_BADVALUE;
    } else {
      dutycycle = CLAMP(effort / ctx->power_voltage, -1.0f, 1.0f);
      dutycycle = copysignf(fast_sqrt(fabsf(dutycycle)), dutycycle);

      err = ecu_devices_motor_set_frequency(ctx->config.device_motor, ctx->config.pwm_freq);
      if(err == E_OK) {
        err = ecu_devices_motor_set_dutycycle(ctx->config.device_motor, dutycycle);
        ctx->diag.bits.motor_pwm_error = err != E_OK && err != E_AGAIN;
        if(err == E_OK) {
          err = ecu_devices_motor_set_enabled(ctx->config.device_motor, true);
        }
      }
    }
  }

  if(!enabled || err != E_OK) {
    dutycycle = 0.0f;
    disable_err = ecu_devices_motor_set_dutycycle(ctx->config.device_motor, dutycycle);
    if(err == E_OK) {
      err = disable_err;
    }
    disable_err = ecu_devices_motor_set_enabled(ctx->config.device_motor, false);
    if(err == E_OK) {
      err = disable_err;
    }
  }

  ctx->diag.bits.motor_handle_error |= err != E_OK && err != E_AGAIN;
  ctx->data.active = enabled && err == E_OK;
  ctx->data.dutycycle = dutycycle;
  ctx->data.output_voltage = dutycycle * ctx->power_voltage;

  return err;
}

error_t etc_internal_process(etc_ctx_t *ctx)
{
  error_t err;
  const etc_adaptation_config_t *model = etc_internal_get_active_model(ctx);
  bool use_model = model != NULL;
  bool base_pid = use_model && ctx->config.adaptation_setup.disable_pid_tuning;
  bool mode_changed = ctx->runtime_model_active != use_model || ctx->runtime_base_pid != base_pid;
  bool model_changed = mode_changed || ctx->process_time == 0;
  time_us_t now = time_now_us();
  float effort, reach_error, progress, required_progress, target = ctx->data.target_position;

  if(mode_changed) {
    ctx->runtime_model_active = use_model;
    ctx->runtime_base_pid = base_pid;
    ctx->process_time = 0;
  }

  if(base_pid && model_changed) {
    /* Match the motor command already in effect, in the effort domain. */
    ctx->model_handover_effort = ctx->data.dutycycle * fabsf(ctx->data.dutycycle) * ctx->power_voltage;
    ctx->model_handover_time = now;
    ctx->model_handover_active = true;
  } else if(!base_pid) {
    ctx->model_handover_active = false;
  }

  err = etc_internal_sample(ctx, now);
  if(err != E_OK || !ctx->data.enabled || ctx->diag.bits.position_reach_failure || !etc_internal_number_valid(target)) {
    ctx->model_handover_active = false;
    etc_internal_reset_control(ctx, now);
    return etc_internal_drive(ctx, false, 0.0f);
  }

  if(!ctx->data.active || model_changed) {
    etc_internal_reset_control(ctx, now);
  }

  target = CLAMP(target, 0.0f, 100.0f);
  effort = etc_internal_control(ctx, model, target, ctx->power_voltage, now);
  if(model != NULL) {
    float settle_effort = etc_internal_settle_effort(ctx, model, target, now);
    if(ctx->reversal.direction == 0 && ctx->reversal.release_direction == 0) {
      effort = CLAMP(effort + settle_effort, -ctx->power_voltage, ctx->power_voltage);
    }
  }
  if(ctx->model_handover_active) {
    time_delta_us_t elapsed = time_diff(now, ctx->model_handover_time);
    ctx->model_handover_active = elapsed < ETC_CONTROL_MODEL_HANDOVER_TIME;
    if(ctx->model_handover_active) {
      effort = BLEND(ctx->model_handover_effort, effort, (float)elapsed / ETC_CONTROL_MODEL_HANDOVER_TIME);
    }
  }

  reach_error = target - ctx->position_current;
  if(fabsf(reach_error) < ctx->config.motor_reach_threshold) {
    ctx->pos_reach_position = ctx->position_current;
    ctx->pos_reach_time = now;
    ctx->diag.bits.position_reach_failure = false;
  } else if(time_diff(now, ctx->pos_reach_time) > ctx->config.motor_reach_timeout) {
    reach_error = target - ctx->pos_reach_position;
    progress = (ctx->position_current - ctx->pos_reach_position) * copysignf(1.0f, reach_error);
    required_progress = MAX(ETC_CONTROL_REACH_MIN_PROGRESS, ETC_CONTROL_REACH_PROGRESS * fabsf(reach_error));
    /* A long move gets another window only after measurable progress toward the target. */
    if(model != NULL && progress >= required_progress) {
      ctx->pos_reach_position = ctx->position_current;
      ctx->pos_reach_time = now;
    } else {
      ctx->diag.bits.position_reach_failure = true;
    }
  }

  return etc_internal_drive(ctx, !ctx->diag.bits.position_reach_failure, effort);
}
