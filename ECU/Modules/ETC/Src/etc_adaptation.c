/*
 * etc_adaptation.c
 */

#include "etc_internal.h"
#include "config_ckp.h"
#include "compiler.h"
#include <float.h>
#include <math.h>
#include <string.h>

#define ETC_ADAPTATION_TIMEOUT_US          (5000 * TIME_US_IN_MS)
#define ETC_ADAPTATION_PARK_US             (300 * TIME_US_IN_MS)
#define ETC_ADAPTATION_SETTLE_US           (80 * TIME_US_IN_MS)
#define ETC_ADAPTATION_FAST_MIN_SPEED      (30.0f)
#define ETC_ADAPTATION_FAST_SPEED_FACTOR   (4.0f)
#define ETC_ADAPTATION_FAST_WINDOW_US      (800 * TIME_US_IN_MS)
#define ETC_ADAPTATION_FAST_TRAVEL_US      (TIME_US_IN_S / 30u + 1u)
#define ETC_ADAPTATION_FAST_SETTLE_ERROR   (0.60f)
#define ETC_ADAPTATION_FAST_NEAR_ERROR     (1.5f)
#define ETC_ADAPTATION_FAST_DWELL_US       (200 * TIME_US_IN_MS)
#define ETC_ADAPTATION_FAST_CONFIRM_US     (200 * TIME_US_IN_MS)
#define ETC_ADAPTATION_PROBE_LIMIT_US      (80 * TIME_US_IN_MS)
#define ETC_ADAPTATION_PROBE_RATE_DIV      (3u)
#define ETC_ADAPTATION_PROBE_RATE          (1.0f / ETC_ADAPTATION_PROBE_RATE_DIV)
#define ETC_ADAPTATION_PROBE_TIMEOUT_US    (ETC_ADAPTATION_TIMEOUT_US * ETC_ADAPTATION_PROBE_RATE_DIV)
#define ETC_ADAPTATION_PROBE_TRAVEL        (0.25f)
#define ETC_ADAPTATION_REVERSAL_APPROACH   (1.0f)
#define ETC_ADAPTATION_SETTLE_ERROR        (0.40f)
#define ETC_ADAPTATION_SETTLE_SPEED        (2.0f)
#define ETC_ADAPTATION_SEEK_CAPTURE_ERROR  (0.30f)
#define ETC_ADAPTATION_SEEK_HOLD_TRAVEL    (0.20f)
#define ETC_ADAPTATION_SEEK_POSITION_KP    (8.0f)
#define ETC_ADAPTATION_SEEK_SPEED_MAX      (32.0f)
#define ETC_ADAPTATION_MECH_SEEK_SPEED_MAX (16.0f)
#define ETC_ADAPTATION_MECH_SEEK_ACCEL     (120.0f)
#define ETC_ADAPTATION_SEEK_SPEED_KP       (0.018f)
#define ETC_ADAPTATION_SEEK_SPEED_KI       (0.20f)
#define ETC_ADAPTATION_SEEK_EFFORT_RATE    (18.0f)
#define ETC_ADAPTATION_MECH_EFFORT_RATE    (6.0f)
#define ETC_ADAPTATION_FAST_LOW            (6.0f)
#define ETC_ADAPTATION_FAST_MIDDLE         (20.0f)
#define ETC_ADAPTATION_FAST_HIGH           (80.0f)
#define ETC_ADAPTATION_FAST_MIN_SPACING    (2.0f)
#define ETC_ADAPTATION_FAST_STEPS          (6u)
#define ETC_ADAPTATION_OSCILLATION_BAND    (0.30f)
#define ETC_ADAPTATION_OSCILLATION_DECAY   (0.90f)
#define ETC_ADAPTATION_OSCILLATION_PEAKS   (2u)
#define ETC_ADAPTATION_MARGIN              (1.5f)

typedef struct {
    float position_p;
    float position_i;
    float position_d;
    float speed_p;
}etc_adaptation_preset_t;

static const etc_adaptation_preset_t etc_adaptation_presets[ETC_ADAPTATION_PID_FINALISTS] = {
    { 1.00f, 1.00f, 1.00f, 1.00f },
    { 0.80f, 0.65f, 1.20f, 0.90f },
    { 1.15f, 0.80f, 1.10f, 1.10f },
    { 1.00f, 0.80f, 1.10f, 1.00f },
};

static void etc_adaptation_set_trial(etc_ctx_t *ctx)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  const etc_adaptation_preset_t *preset = &etc_adaptation_presets[adaptation->trial_index];
  etc_adaptation_config_t *trial = &adaptation->trial;

  *trial = adaptation->model;
  trial->pid_position = ctx->config.pid_position;
  trial->pid_position.Kp *= preset->position_p;
  trial->pid_position.Ki *= preset->position_i;
  trial->pid_position.Kd *= preset->position_d;
  trial->pid_speed = ctx->config.pid_speed;
  trial->pid_speed.Kp *= preset->speed_p;
}

error_t etc_adaptation_start(etc_ctx_t *ctx)
{
  error_t err = E_AGAIN;
  uint32_t prim;
  etc_adaptation_ctx_t *adaptation;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    adaptation = &ctx->adaptation;
    prim = EnterCritical();
    if(!ctx->ready || !ctx->configured || !ctx->config.enabled || ctx->reset_request) {
      err = E_NOTRDY;
    } else if(adaptation->status == E_AGAIN) {
      err = E_BUSY;
    } else {
      adaptation->started = true;
      adaptation->status = E_AGAIN;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t etc_adaptation_process(etc_ctx_t *ctx)
{
  error_t err = E_NOTRDY;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    prim = EnterCritical();
    if(ctx->adaptation.started || ctx->adaptation.status == E_AGAIN) {
      err = ctx->adaptation.status;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t etc_adaptation_result(etc_ctx_t *ctx, etc_adaptation_config_t *result)
{
  error_t err = E_NOTRDY;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL || result == NULL || result == &ctx->config.adaptation, err = E_PARAM);

    prim = EnterCritical();
    if(ctx->adaptation.started || ctx->adaptation.status == E_AGAIN) {
      err = ctx->adaptation.status;
    }
    if(err == E_OK) {
      memcpy(result, &ctx->adaptation.result, sizeof(*result));
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t etc_adaptation_cancel(etc_ctx_t *ctx)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    prim = EnterCritical();
    if(ctx->adaptation.status == E_AGAIN) {
      ctx->adaptation.cancel = true;
      err = E_AGAIN;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

static void etc_adaptation_set_state(etc_ctx_t *ctx, etc_adaptation_fsm_t state, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;

  adaptation->fsm = state;
  adaptation->state_time = now;
  adaptation->stable_time = now;
  if(state == ETC_ADAPTATION_FSM_SEEK || state == ETC_ADAPTATION_FSM_FAST_SEEK) {
    float dutycycle = ctx->data.dutycycle;

    adaptation->seek_integral = 0.0f;
    adaptation->seek_effort = dutycycle * fabsf(dutycycle) * ctx->power_voltage;
    adaptation->seek_speed = 0.0f;
    adaptation->seek_holding = false;
  }
  if(state == ETC_ADAPTATION_FSM_FAST_STEP) {
    etc_internal_reset_control(ctx, now);
  }
}

static error_t etc_adaptation_init_run(etc_ctx_t *ctx, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  const etc_adaptation_setup_t *setup = &ctx->config.adaptation_setup;
  error_t err = E_OK;
  uint32_t prim;

  do {
    prim = EnterCritical();
    *adaptation = (etc_adaptation_ctx_t){
        .status = adaptation->status,
        .apply_status = adaptation->apply_status == E_AGAIN ? E_AGAIN : E_OK,
        .started = adaptation->started,
        .cancel = adaptation->cancel,
        .initialized = true,
    };
    ExitCritical(prim);

    ctx->process_time = 0;
    adaptation->sample_time = now;
    etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_PARK, now);

    BREAK_IF_ACTION(!etc_internal_number_valid(setup->position_min) ||
        !etc_internal_number_valid(setup->position_max) || !etc_internal_number_valid(setup->dutycycle_max) ||
        setup->position_min < 0.0f || setup->position_max > 100.0f ||
        setup->position_max - setup->position_min < 10.0f ||
        setup->dutycycle_max <= 0.0f || setup->dutycycle_max > 0.60f ||
        setup->tps_average_samples > ETC_ADAPTATION_TPS_AVERAGE_MAX,
        adaptation->failure.reason = ETC_ADAPTATION_FAILURE_SETUP; err = E_PARAM);

    adaptation->setup = *setup;
    if(adaptation->setup.tps_average_samples == 0) {
      adaptation->setup.tps_average_samples = ETC_ADAPTATION_TPS_AVERAGE_DEFAULT;
    }
    adaptation->model.tps_average_samples = adaptation->setup.tps_average_samples;
    err = etc_internal_drive(ctx, false, 0.0f);

  } while(0);

  return err;
}

static float etc_adaptation_get_position(const etc_adaptation_ctx_t *adaptation, uint8_t point)
{
  return adaptation->model.points[point].position;
}

static float etc_adaptation_get_fast_position(const etc_adaptation_ctx_t *adaptation, uint8_t index)
{
  const etc_adaptation_point_t *points = adaptation->model.points;
  const float min = points[ETC_ADAPTATION_POINT_LOW_1].position;
  const float max = points[ETC_ADAPTATION_POINT_HIGH_2].position;
  float low = CLAMP(ETC_ADAPTATION_FAST_LOW, min, max);
  float middle = CLAMP(ETC_ADAPTATION_FAST_MIDDLE, min, max);
  float high = CLAMP(ETC_ADAPTATION_FAST_HIGH, min, max);
  /* 6 -> 80 -> 20 -> 80 -> 6 -> 20 -> 6 covers each pair in both directions. */
  static const uint8_t route[ETC_ADAPTATION_FAST_STEPS] = {0u, 2u, 1u, 2u, 0u, 1u};
  float positions[3];

  if(middle - low < ETC_ADAPTATION_FAST_MIN_SPACING || high - middle < ETC_ADAPTATION_FAST_MIN_SPACING) {
    low = min;
    high = max;
    middle = (min + max) * 0.5f;
  }
  positions[0] = low;
  positions[1] = middle;
  positions[2] = high;

  return positions[route[index % ITEMSOF(route)]];
}

static void etc_adaptation_start_fast_trial(etc_ctx_t *ctx, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  float nearest = FLT_MAX;
  float distance;

  adaptation->trial_index = adaptation->finalist_index;
  etc_adaptation_set_trial(ctx);
  adaptation->trial_score = 0.0f;
  adaptation->worst_step_score = 0.0f;
  adaptation->step = 0;
  for(uint8_t i = 0; i < 3u; i++) {
    distance = fabsf(ctx->position_current - etc_adaptation_get_fast_position(adaptation, i));
    if(distance < nearest) {
      nearest = distance;
      adaptation->fast_route_offset = i;
    }
  }
  etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_FAST_SEEK, now);
}

static error_t etc_adaptation_park(etc_ctx_t *ctx, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  etc_adaptation_point_t *points = adaptation->model.points;
  float min = adaptation->setup.position_min;
  float max = adaptation->setup.position_max;
  float rest = ctx->position_current;
  float lower, upper, default_lower, default_upper;
  error_t err = E_AGAIN;

  do {
    err = etc_internal_drive(ctx, false, 0.0f);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(ctx->tps_average.count < adaptation->setup.tps_average_samples, err = E_AGAIN);

    if(adaptation->park_sample_valid) {
      adaptation->park_position_min = MIN(adaptation->park_position_min, rest);
      adaptation->park_position_max = MAX(adaptation->park_position_max, rest);
    }
    if(!adaptation->park_sample_valid ||
        adaptation->park_position_max - adaptation->park_position_min > ETC_ADAPTATION_SETTLE_ERROR) {
      adaptation->park_sample_valid = true;
      adaptation->park_position_min = rest;
      adaptation->park_position_max = rest;
      adaptation->stable_time = now;
    }
    BREAK_IF_ACTION(time_diff(now, adaptation->state_time) < ETC_ADAPTATION_PARK_US ||
        time_diff(now, adaptation->stable_time) < ETC_ADAPTATION_SETTLE_US, err = E_AGAIN);

    rest = (adaptation->park_position_min + adaptation->park_position_max) * 0.5f;
    BREAK_IF_ACTION(rest - min < 4.0f || max - rest < 6.0f,
        adaptation->failure.reason = ETC_ADAPTATION_FAILURE_REST_RANGE; err = E_BADVALUE);

    lower = min + ETC_ADAPTATION_MARGIN;
    upper = max - ETC_ADAPTATION_MARGIN;
    default_lower = rest - ETC_ADAPTATION_MARGIN;
    default_upper = rest + ETC_ADAPTATION_MARGIN;

    points[ETC_ADAPTATION_POINT_LOW_1].position = lower;
    points[ETC_ADAPTATION_POINT_LOW_2].position = BLEND(lower, default_lower, 0.5f);
    points[ETC_ADAPTATION_POINT_DEFAULT_LOWER].position = default_lower;
    points[ETC_ADAPTATION_POINT_DEFAULT_UPPER].position = default_upper;
    points[ETC_ADAPTATION_POINT_MIDDLE_1].position = BLEND(default_upper, upper, 0.33f);
    points[ETC_ADAPTATION_POINT_MIDDLE_2].position = BLEND(default_upper, upper, 0.53f);
    points[ETC_ADAPTATION_POINT_HIGH_1].position = BLEND(default_upper, upper, 0.80f);
    points[ETC_ADAPTATION_POINT_HIGH_2].position = upper;
    adaptation->direction = 1;
    adaptation->best_score = FLT_MAX;
    etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_SEEK, now);

    err = E_AGAIN;

  } while(0);

  return err;
}

static error_t etc_adaptation_seek(etc_ctx_t *ctx, float limit, time_delta_us_t delta, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  float target, error, dt, speed, speed_error, feedback, integral, effort, effort_delta;
  bool mechanical = !adaptation->model.valid;
  error_t err = E_AGAIN;

  do {
    if(adaptation->model.valid) {
      target = etc_adaptation_get_fast_position(adaptation, adaptation->fast_route_offset + adaptation->step);
    } else {
      target = etc_adaptation_get_position(adaptation, adaptation->point);
      if(!adaptation->seek_reversal_ready) {
        /* Return to the point on the old flank before reversing motor effort. */
        target += adaptation->direction * ETC_ADAPTATION_REVERSAL_APPROACH;
      }
    }
    error = target - ctx->position_current;

    /* Keep the effort constant while checking that the plate has settled. */
    if(adaptation->seek_holding) {
      if(fabsf(error) > ETC_ADAPTATION_SETTLE_ERROR ||
          fabsf(ctx->position_current - adaptation->seek_hold_position) > ETC_ADAPTATION_SEEK_HOLD_TRAVEL) {
        adaptation->seek_holding = false;
      }
    } else if(fabsf(error) <= ETC_ADAPTATION_SEEK_CAPTURE_ERROR &&
        (!mechanical || fabsf(ctx->current_speed) <= ETC_ADAPTATION_SETTLE_SPEED)) {
      adaptation->seek_holding = true;
      adaptation->seek_hold_position = ctx->position_current;
      adaptation->seek_speed = 0.0f;
      adaptation->stable_time = now;
    }

    if(!adaptation->seek_holding) {
      dt = (float)delta * TIME_S_IN_US;
      speed = mechanical ? ETC_ADAPTATION_MECH_SEEK_SPEED_MAX : ETC_ADAPTATION_SEEK_SPEED_MAX;
      speed = CLAMP(ETC_ADAPTATION_SEEK_POSITION_KP * error, -speed, speed);
      if(mechanical) {
        float speed_delta = ETC_ADAPTATION_MECH_SEEK_ACCEL * dt;
        adaptation->seek_speed += CLAMP(speed - adaptation->seek_speed, -speed_delta, speed_delta);
        speed = adaptation->seek_speed;
      }
      speed_error = speed - ctx->current_speed;
      feedback = ETC_ADAPTATION_SEEK_SPEED_KP * speed_error;
      integral = adaptation->seek_integral + ETC_ADAPTATION_SEEK_SPEED_KI * dt * speed_error;
      effort = feedback + integral;
      if((effort >= -limit && effort <= limit) || effort * speed_error < 0.0f) {
        adaptation->seek_integral = CLAMP(integral, -limit, limit);
      }

      effort = CLAMP(feedback + adaptation->seek_integral, -limit, limit);
      effort_delta = (mechanical ? ETC_ADAPTATION_MECH_EFFORT_RATE : ETC_ADAPTATION_SEEK_EFFORT_RATE) * dt;
      adaptation->seek_effort += CLAMP(effort - adaptation->seek_effort, -effort_delta, effort_delta);
    }

    effort = CLAMP(adaptation->seek_effort, -limit, limit);
    err = etc_internal_drive(ctx, true, effort);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(!adaptation->seek_holding ||
        time_diff(now, adaptation->stable_time) < ETC_ADAPTATION_SETTLE_US, err = E_AGAIN);

    if(mechanical && !adaptation->seek_reversal_ready) {
      adaptation->seek_reversal_ready = true;
      adaptation->seek_reversal_position = ctx->position_current;
      etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_SEEK, now);
      err = E_AGAIN;
      break;
    }

    adaptation->start_position = ctx->position_current;
    if(adaptation->model.valid) {
      adaptation->error_integral = 0.0f;
      adaptation->residual_raw_prev = 0.0f;
      adaptation->overshoot = 0.0f;
      adaptation->fast_step = (etc_adaptation_fast_step_t){
          .band = MAX(ETC_ADAPTATION_OSCILLATION_BAND, 2.0f * (adaptation->park_position_max - adaptation->park_position_min)),
          .position_extremum = ctx->position_current,
          .previous_extremum = ctx->position_current,
      };
      etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_FAST_STEP, now);
    } else {
      BREAK_IF_ACTION((adaptation->seek_reversal_position - ctx->position_current) * adaptation->direction <
          ETC_ADAPTATION_PROBE_TRAVEL, adaptation->failure.reason = ETC_ADAPTATION_FAILURE_PROBE_DIRECTION; err = E_BADVALUE);
      adaptation->start_effort = effort;
      adaptation->effort = effort;
      adaptation->motion_detected = false;
      adaptation->probe_limit_reached = false;
      etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_PROBE, now);
    }

    err = E_AGAIN;

  } while(0);

  return err;
}

static void etc_adaptation_fit_spring(etc_adaptation_point_t *points, uint8_t count)
{
  float means[ETC_ADAPTATION_POINTS];
  uint8_t lengths[ETC_ADAPTATION_POINTS];
  uint8_t blocks = 0, point = 0;

  /* Merge neighboring blocks whose measured spring effort decreases toward opening. */
  for(uint8_t i = 0; i < count; i++) {
    means[blocks] = points[i].spring;
    lengths[blocks++] = 1;
    while(blocks > 1 && means[blocks - 2] > means[blocks - 1]) {
      blocks--;
      uint8_t length = lengths[blocks - 1] + lengths[blocks];
      means[blocks - 1] = BLEND(means[blocks - 1], means[blocks], (float)lengths[blocks] / length);
      lengths[blocks - 1] = length;
    }
  }

  for(uint8_t i = 0; i < blocks; i++) {
    for(uint8_t j = 0; j < lengths[i]; j++) {
      points[point++].spring = means[i];
    }
  }
}

static error_t etc_adaptation_probe(etc_ctx_t *ctx, float limit, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  etc_adaptation_point_t *point = &adaptation->model.points[adaptation->point];
  float displacement = (ctx->position_current - adaptation->start_position) * adaptation->direction;
  float effort, opening, closing;
  error_t err = E_AGAIN;

  do {
    BREAK_IF_ACTION(displacement < -2.0f * ETC_ADAPTATION_PROBE_TRAVEL,
        adaptation->failure.reason = ETC_ADAPTATION_FAILURE_PROBE_DIRECTION; err = E_BADVALUE);

    if(displacement >= ETC_ADAPTATION_PROBE_TRAVEL) {
      if(!adaptation->motion_detected) {
        adaptation->motion_detected = true;
        adaptation->motion_effort = adaptation->effort;
        adaptation->stable_time = now;
      }
      if(time_diff(now, adaptation->stable_time) >= 6 * TIME_US_IN_MS) {
        if(adaptation->direction > 0) {
          adaptation->opening_effort = adaptation->motion_effort;
          adaptation->direction = -1;
        } else {
          opening = adaptation->opening_effort;
          closing = adaptation->motion_effort;
          BREAK_IF_ACTION(opening < closing || fabsf(opening) > 16.0f || fabsf(closing) > 16.0f,
              adaptation->failure.reason = ETC_ADAPTATION_FAILURE_PROBE_THRESHOLDS; err = E_BADVALUE);
          /* Midpoint and half-width of the measured breakaway band. */
          point->spring = (opening + closing) * 0.5f;
          point->friction = (opening - closing) * 0.5f;
          adaptation->model.reversal[adaptation->point] = (etc_adaptation_reversal_t){opening, closing};
          adaptation->direction = 1;
          if(++adaptation->point >= ETC_ADAPTATION_POINTS) {
            etc_adaptation_fit_spring(adaptation->model.points, ETC_ADAPTATION_POINT_DEFAULT_UPPER);
            etc_adaptation_fit_spring(&adaptation->model.points[ETC_ADAPTATION_POINT_DEFAULT_UPPER],
                ETC_ADAPTATION_POINTS - ETC_ADAPTATION_POINT_DEFAULT_UPPER);
            adaptation->model.spring_scale_lower = 1.0f;
            adaptation->model.spring_scale_upper = 1.0f;
            adaptation->model.friction_scale_lower = 1.0f;
            adaptation->model.friction_scale_upper = 1.0f;
            adaptation->model.pid_position = ctx->config.pid_position;
            adaptation->model.pid_speed = ctx->config.pid_speed;
            adaptation->model.valid = true;
            adaptation->model.reversal_valid = true;
            BREAK_IF_ACTION(!etc_internal_adaptation_config_valid(&adaptation->model),
                adaptation->failure.reason = ETC_ADAPTATION_FAILURE_MODEL; err = E_BADVALUE);
            if(adaptation->setup.disable_pid_tuning) {
              adaptation->result = adaptation->model;
              err = E_OK;
              break;
            }

            adaptation->finalist_count = ITEMSOF(etc_adaptation_presets);
            for(uint8_t i = 0; i < adaptation->finalist_count; i++) {
              adaptation->finalists[i].trial = i;
            }
            adaptation->fast_pid = true;
            etc_adaptation_start_fast_trial(ctx, now);
            break;
          }
        }
        adaptation->seek_reversal_ready = false;
        etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_SEEK, now);
        break;
      }
    } else {
      adaptation->motion_detected = false;
    }

    effort = adaptation->start_effort +
        (float)time_diff(now, adaptation->state_time) * TIME_S_IN_US * ETC_ADAPTATION_PROBE_RATE * adaptation->direction;
    if(!adaptation->motion_detected) {
      if(effort * adaptation->direction >= limit) {
        if(!adaptation->probe_limit_reached) {
          adaptation->probe_limit_reached = true;
          adaptation->probe_limit_time = now;
        }
        BREAK_IF_ACTION(time_diff(now, adaptation->probe_limit_time) >= ETC_ADAPTATION_PROBE_LIMIT_US,
            adaptation->failure.reason = ETC_ADAPTATION_FAILURE_PROBE_LIMIT; err = E_BADVALUE);
      } else {
        adaptation->probe_limit_reached = false;
      }
      adaptation->effort = effort;
    }
    adaptation->effort = CLAMP(adaptation->effort, -limit, limit);
    err = etc_internal_drive(ctx, true, adaptation->effort);

    if(err == E_OK) {
      err = E_AGAIN;
    }

  } while(0);

  return err;
}

static error_t etc_adaptation_next_trial(etc_ctx_t *ctx, bool valid, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  error_t err = E_AGAIN;
  float score = adaptation->trial_score + adaptation->worst_step_score;

  do {
    if(valid && etc_internal_adaptation_config_valid(&adaptation->trial)) {
      adaptation->finalists[adaptation->finalist_index].fast_score = score;
      if(score < adaptation->best_score) {
        adaptation->result = adaptation->trial;
        adaptation->best_score = score;
        adaptation->selected_trial = adaptation->trial_index;
      }
    }

    if(++adaptation->finalist_index >= adaptation->finalist_count) {
      BREAK_IF_ACTION(!adaptation->result.valid,
          adaptation->failure.reason = ETC_ADAPTATION_FAILURE_PID_FAST_NO_CANDIDATE; err = E_BADVALUE);
      err = E_OK;
      break;
    }

    etc_adaptation_start_fast_trial(ctx, now);

  } while(0);

  return err;
}

static bool etc_adaptation_fast_dwell(etc_adaptation_fast_step_t *step, float residual, time_delta_us_t elapsed)
{
  int8_t side;

  if(residual <= ETC_ADAPTATION_FAST_NEAR_ERROR) {
    step->near_target_seen = true;
  }
  if(!step->near_target_seen || fabsf(residual) <= ETC_ADAPTATION_FAST_SETTLE_ERROR) {
    step->near_error_side = 0;
  } else {
    side = residual > 0.0f ? 1 : -1;
    if(side != step->near_error_side) {
      step->near_error_since = elapsed;
      step->near_error_side = side;
    }
    step->max_near_error_time = MAX(step->max_near_error_time, elapsed - step->near_error_since);
  }

  return step->max_near_error_time < ETC_ADAPTATION_FAST_DWELL_US;
}

static bool etc_adaptation_fast_oscillation(etc_adaptation_fast_step_t *step, float position)
{
  float delta = position - step->position_extremum;
  float excursion;
  int8_t direction = delta > 0.0f ? 1 : -1;

  if(step->motion_direction == 0) {
    if(fabsf(delta) >= step->band) {
      step->motion_direction = direction;
      step->position_extremum = position;
    }
  } else if(delta * step->motion_direction > 0.0f) {
    step->position_extremum = position;
  } else if(fabsf(delta) >= step->band) {
    /* Compare successive excursions even if neither crosses the target. */
    excursion = fabsf(step->position_extremum - step->previous_extremum);
    if(step->previous_excursion > 0.0f && excursion >= step->previous_excursion * ETC_ADAPTATION_OSCILLATION_DECAY) {
      step->nondecaying_peaks++;
    } else {
      step->nondecaying_peaks = 0;
    }
    step->previous_excursion = excursion;
    step->previous_extremum = step->position_extremum;
    step->position_extremum = position;
    step->motion_direction = direction;
  }

  return step->nondecaying_peaks >= ETC_ADAPTATION_OSCILLATION_PEAKS;
}

static void etc_adaptation_fast_sample(etc_ctx_t *ctx, float residual, float residual_raw, float direction,
    time_delta_us_t elapsed)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  etc_adaptation_fast_step_t *step = &adaptation->fast_step;
  float speed = ctx->current_speed * direction;
  float positive_error = MIN(residual_raw, adaptation->residual_raw_prev);

  if(!step->reached_target) {
    step->peak_speed = MAX(step->peak_speed, MIN(speed, step->previous_speed));
    step->previous_speed = speed;
    step->reached_target = residual <= 0.0f;
  }

  if(fabsf(residual) > ETC_ADAPTATION_FAST_SETTLE_ERROR || fabsf(ctx->current_speed) > 2.0f * ETC_ADAPTATION_SETTLE_SPEED) {
    step->settle_since = 0;
    step->settled_undershoot = 0.0f;
  } else if(step->settle_since == 0) {
    step->settle_since = elapsed;
  } else {
    step->settled_undershoot = MAX(step->settled_undershoot, positive_error);
  }

  if(adaptation->overshoot > step->band) {
    step->rebound = MAX(step->rebound, positive_error);
  }
}

static error_t etc_adaptation_step(etc_ctx_t *ctx, float limit, time_delta_us_t delta, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  etc_adaptation_fast_step_t *step = &adaptation->fast_step;
  etc_adaptation_pid_finalist_t *finalist = &adaptation->finalists[adaptation->finalist_index];
  float target = etc_adaptation_get_fast_position(adaptation, adaptation->fast_route_offset + adaptation->step + 1u);
  float span = fabsf(target - adaptation->start_position);
  float direction = target > adaptation->start_position ? 1.0f : -1.0f;
  float residual = (target - ctx->position_current) * direction;
  float residual_raw = (target - ctx->tps_data.position_unfiltered) * direction;
  time_delta_us_t elapsed = time_diff(now, adaptation->state_time);
  time_delta_us_t duration = ETC_ADAPTATION_FAST_WINDOW_US + (time_delta_us_t)(span * ETC_ADAPTATION_FAST_TRAVEL_US);
  float dt = (float)delta * TIME_S_IN_US;
  float effort, score, required_speed;
  bool fast_settled, dwell_ok;
  error_t err = E_AGAIN;

  do {
    adaptation->error_integral += fabsf(residual) * dt;
    /* Use raw TPS for peaks, with two consecutive samples to reject isolated spikes. */
    adaptation->overshoot = MAX(adaptation->overshoot, -MAX(residual_raw, adaptation->residual_raw_prev));
    etc_adaptation_fast_sample(ctx, residual, residual_raw, direction, elapsed);
    adaptation->residual_raw_prev = residual_raw;

    if(etc_adaptation_fast_oscillation(step, ctx->position_current)) {
      finalist->fast_rejection = ETC_ADAPTATION_FAILURE_PID_FAST_OSCILLATION;
      adaptation->oscillation_rejections++;
      err = etc_internal_drive(ctx, false, 0.0f);
      if(err == E_OK) {
        err = etc_adaptation_next_trial(ctx, false, now);
      } else if(err == E_AGAIN) {
        err = E_NOTRDY;
      }
      break;
    }

    dwell_ok = etc_adaptation_fast_dwell(step, residual, elapsed);
    finalist->max_near_error_time = MAX(finalist->max_near_error_time, step->max_near_error_time);
    BREAK_IF_ACTION(!dwell_ok,
        finalist->fast_rejection = ETC_ADAPTATION_FAILURE_PID_FAST_DWELL; err = etc_adaptation_next_trial(ctx, false, now));

    fast_settled = step->settle_since != 0 && elapsed - step->settle_since >= ETC_ADAPTATION_FAST_CONFIRM_US;
    if(fast_settled || elapsed >= duration) {
      if(adaptation->step == 0 || step->peak_speed < finalist->min_peak_speed) {
        finalist->min_peak_speed = step->peak_speed;
      }

      required_speed = MIN(ETC_ADAPTATION_FAST_MIN_SPEED, ETC_ADAPTATION_FAST_SPEED_FACTOR * span);
      BREAK_IF_ACTION(step->peak_speed < required_speed,
          finalist->fast_rejection = ETC_ADAPTATION_FAILURE_PID_FAST_SPEED; err = etc_adaptation_next_trial(ctx, false, now));
      BREAK_IF_ACTION(!fast_settled,
          finalist->fast_rejection = ETC_ADAPTATION_FAILURE_PID_FAST_SETTLE; err = etc_adaptation_next_trial(ctx, false, now));
      finalist->fast_steps_passed++;

      score = adaptation->error_integral * ((float)TIME_US_IN_S / duration) +
          2.0f * adaptation->overshoot + step->settled_undershoot + step->rebound +
          span * (float)step->settle_since / duration +
          span * (float)step->max_near_error_time / ETC_ADAPTATION_FAST_DWELL_US;
      score /= MAX(span, 1.0f);
      adaptation->trial_score += score;
      adaptation->worst_step_score = MAX(adaptation->worst_step_score, score);
      BREAK_IF_ACTION(++adaptation->step >= ETC_ADAPTATION_FAST_STEPS, err = etc_adaptation_next_trial(ctx, true, now));
      etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_FAST_SEEK, now);
      break;
    }

    effort = etc_internal_control(ctx, &adaptation->trial, target, limit, now);
    BREAK_IF_ACTION(!etc_internal_number_valid(effort),
        adaptation->failure.reason = ETC_ADAPTATION_FAILURE_PID_EFFORT; err = E_BADVALUE);
    err = etc_internal_drive(ctx, true, effort);
    BREAK_IF(err != E_OK);

    finalist->peak_dutycycle = MAX(finalist->peak_dutycycle, fabsf(ctx->data.dutycycle));
    if(!step->command_sent) {
      step->command_sent = true;
      finalist->fast_steps_started++;
    }
    err = E_AGAIN;

  } while(0);

  return err;
}

static float etc_adaptation_get_effort_limit(const etc_ctx_t *ctx)
{
  const etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  float duty_limit = adaptation->setup.dutycycle_max;

  if(adaptation->fsm == ETC_ADAPTATION_FSM_FAST_STEP) {
    duty_limit = 1.0f;
  }

  return duty_limit * duty_limit * ctx->power_voltage;
}

static void etc_adaptation_capture_failure(etc_ctx_t *ctx, error_t err, time_us_t now)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  etc_adaptation_failure_data_t *failure = &adaptation->failure;

  failure->fsm = adaptation->fsm;
  failure->error = err;
  failure->elapsed = time_diff(now, adaptation->state_time);
  failure->position = ctx->position_current;
  failure->position_raw = ctx->tps_data.position_unfiltered;
  failure->speed = ctx->current_speed;
  failure->power_voltage = ctx->power_voltage;
  failure->dutycycle = ctx->data.dutycycle;
  failure->effort_limit = etc_adaptation_get_effort_limit(ctx);
  failure->point = adaptation->point;
  failure->trial = MIN(adaptation->trial_index, ITEMSOF(etc_adaptation_presets) - 1u);
  failure->step = adaptation->step;
  failure->direction = adaptation->direction;
  failure->fast_pid = adaptation->fast_pid;
  failure->oscillation_rejections = adaptation->oscillation_rejections;
}

void etc_adaptation_loop(etc_ctx_t *ctx)
{
  etc_adaptation_ctx_t *adaptation = &ctx->adaptation;
  time_us_t now = time_now_us();
  time_delta_us_t delta;
  float limit = 0.0f;
  error_t err = E_AGAIN;
  error_t stop_err;
  time_delta_us_t timeout;
  ckp_data_t ckp;
  uint32_t prim;
  bool cancel;

  do {
    if(!adaptation->initialized) {
      err = etc_adaptation_init_run(ctx, now);
      if(err == E_OK) {
        err = E_AGAIN;
      }
    }

    prim = EnterCritical();
    cancel = adaptation->cancel;
    ExitCritical(prim);

    if(adaptation->fsm != ETC_ADAPTATION_FSM_STOP) {
      timeout = adaptation->fsm == ETC_ADAPTATION_FSM_PROBE ? ETC_ADAPTATION_PROBE_TIMEOUT_US : ETC_ADAPTATION_TIMEOUT_US;
      if(cancel) {
        adaptation->failure.reason = ETC_ADAPTATION_FAILURE_CANCELLED;
        err = E_ABORTED;
      } else if(time_diff(now, adaptation->state_time) >= timeout) {
        adaptation->failure.reason = ETC_ADAPTATION_FAILURE_STATE_TIMEOUT;
        err = E_TIMEOUT;
      }

      delta = time_diff(now, adaptation->sample_time);
      BREAK_IF(err == E_AGAIN && delta == 0);
      adaptation->sample_time = now;

      if(err == E_AGAIN) {
        err = ecu_sensors_ckp_get_value(ECU_SENSOR_CKP_1, &ckp);
        if(err == E_OK && ckp.validity != CKP_DATA_NONE) {
          adaptation->failure.reason = ETC_ADAPTATION_FAILURE_ENGINE_MOVING;
          err = E_INVALACT;
        } else if(err != E_OK) {
          adaptation->failure.reason = ETC_ADAPTATION_FAILURE_CKP;
          if(err == E_AGAIN) {
            err = E_NOTRDY;
          }
        }
      }
      if(err == E_OK) {
        err = etc_internal_sample(ctx, now);
        if(err == E_AGAIN) {
          if(adaptation->fsm == ETC_ADAPTATION_FSM_PARK) {
            adaptation->park_sample_valid = false;
          } else {
            err = E_NOTRDY;
          }
        }
        if(err != E_OK && err != E_AGAIN) {
          adaptation->failure.reason = ETC_ADAPTATION_FAILURE_SAMPLE;
        }
      }

      if(err == E_OK) {
        if((adaptation->fsm != ETC_ADAPTATION_FSM_PARK &&
            (ctx->tps_data.position_unfiltered < adaptation->setup.position_min ||
                ctx->tps_data.position_unfiltered > adaptation->setup.position_max))) {
          adaptation->failure.reason = ETC_ADAPTATION_FAILURE_POSITION_RANGE;
          err = E_BADVALUE;
        } else if(ctx->power_voltage < 8.0f || ctx->power_voltage > 16.0f) {
          adaptation->failure.reason = ETC_ADAPTATION_FAILURE_SUPPLY_RANGE;
          err = E_BADVALUE;
        } else {
          limit = etc_adaptation_get_effort_limit(ctx);
          switch(adaptation->fsm) {
            case ETC_ADAPTATION_FSM_PARK:
              err = etc_adaptation_park(ctx, now);
              break;
            case ETC_ADAPTATION_FSM_SEEK:
            case ETC_ADAPTATION_FSM_FAST_SEEK:
              err = etc_adaptation_seek(ctx, limit, delta, now);
              break;
            case ETC_ADAPTATION_FSM_PROBE:
              err = etc_adaptation_probe(ctx, limit, now);
              break;
            case ETC_ADAPTATION_FSM_FAST_STEP:
              err = etc_adaptation_step(ctx, limit, delta, now);
              break;
            default:
              adaptation->failure.reason = ETC_ADAPTATION_FAILURE_STATE;
              err = E_INVALACT;
              break;
          }
        }
      }

      BREAK_IF(err == E_AGAIN);

      if(err != E_OK) {
        if(adaptation->failure.reason == ETC_ADAPTATION_FAILURE_NONE) {
          adaptation->failure.reason = ETC_ADAPTATION_FAILURE_MOTOR_COMMAND;
        }
        etc_adaptation_capture_failure(ctx, err, now);
      }

      adaptation->stop_status = err;
      etc_adaptation_set_state(ctx, ETC_ADAPTATION_FSM_STOP, now);
    }

    stop_err = etc_internal_drive(ctx, false, 0.0f);
    BREAK_IF(stop_err != E_OK && time_diff(now, adaptation->state_time) < ETC_ADAPTATION_TIMEOUT_US);

    etc_internal_reset_control(ctx, now);
    prim = EnterCritical();
    adaptation->initialized = false;
    if(stop_err != E_OK) {
      ctx->diag.bits.position_reach_failure = true;
      adaptation->status = stop_err == E_AGAIN ? E_TIMEOUT : stop_err;
      adaptation->failure.stop_error = stop_err;
      if(adaptation->failure.reason == ETC_ADAPTATION_FAILURE_NONE) {
        adaptation->failure.reason = ETC_ADAPTATION_FAILURE_STOP_OUTPUT;
        etc_adaptation_capture_failure(ctx, adaptation->status, now);
      }
    } else {
      adaptation->status = adaptation->cancel ? E_ABORTED : adaptation->stop_status;
      if(adaptation->cancel && adaptation->failure.reason == ETC_ADAPTATION_FAILURE_NONE) {
        adaptation->failure.reason = ETC_ADAPTATION_FAILURE_CANCELLED;
        etc_adaptation_capture_failure(ctx, E_ABORTED, now);
      }
    }
    adaptation->cancel = false;
    ExitCritical(prim);

  } while(0);
}
