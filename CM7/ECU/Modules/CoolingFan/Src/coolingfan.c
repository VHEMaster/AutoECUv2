/*
 * coolingfan.c
 *
 *  Created on: Jun 21, 2024
 *      Author: VHEMaster
 */

#include "coolingfan.h"
#include "compiler.h"
#include <string.h>

error_t coolingfan_init(coolingfan_ctx_t *ctx, const coolingfan_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(coolingfan_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(coolingfan_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t coolingfan_configure(coolingfan_ctx_t *ctx, const coolingfan_config_t *config)
{
  error_t err = E_OK;
  bool valid, support;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = coolingfan_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(coolingfan_config_t));
    }

    if(ctx->config.enabled == true) {
      err = ecu_config_gpio_output_valid(ctx->config.output_drive_pin, &valid);
      BREAK_IF(err != E_OK);

      if(valid) {
        err = ecu_config_gpio_output_lock(ctx->config.output_drive_pin);
        BREAK_IF(err != E_OK);
        ctx->output_pin_locked = true;

        err = ecu_config_gpio_output_set_mode(ctx->config.output_drive_pin, ECU_GPIO_TYPE_DIRECT);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_output_get_id(ctx->config.output_drive_pin, &ctx->output_drive_pin);
        BREAK_IF(err != E_OK);
      }

      err = ecu_config_gpio_input_valid(ctx->config.input_control_pin, &valid);
      BREAK_IF(err != E_OK);

      if(valid) {
        err = ecu_config_gpio_input_lock(ctx->config.input_control_pin);
        BREAK_IF(err != E_OK);
        ctx->control_pin_locked = true;

        err = ecu_config_gpio_input_has_mode_support(ctx->config.input_control_pin, ECU_GPIO_INPUT_TYPE_ANALOG, &support);
        if(support) {
          ctx->control_pin_mode = ECU_GPIO_INPUT_TYPE_ANALOG;
        } else {
          ctx->control_pin_mode = ECU_GPIO_INPUT_TYPE_DIGITAL;
        }
        err = ecu_config_gpio_input_set_mode(ctx->config.input_control_pin, ctx->control_pin_mode);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_input_get_id(ctx->config.input_control_pin, &ctx->input_control_pin);
        BREAK_IF(err != E_OK);
      }

      err = ecu_config_gpio_input_valid(ctx->config.input_force_pin, &valid);
      BREAK_IF(err != E_OK);

      if(valid) {
        err = ecu_config_gpio_input_lock(ctx->config.input_force_pin);
        BREAK_IF(err != E_OK);
        ctx->force_pin_locked = true;

        err = ecu_config_gpio_input_set_mode(ctx->config.input_force_pin, ECU_GPIO_INPUT_TYPE_DIGITAL);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_input_get_id(ctx->config.input_force_pin, &ctx->input_force_pin);
        BREAK_IF(err != E_OK);
      }

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)coolingfan_reset(ctx);
  }

  return err;
}

error_t coolingfan_reset(coolingfan_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->output_pin_locked != false) {
      (void)ecu_config_gpio_output_unlock(ctx->config.output_drive_pin);
      ctx->output_pin_locked = false;
    }

    if(ctx->control_pin_locked != false) {
      (void)ecu_config_gpio_input_unlock(ctx->config.input_control_pin);
      ctx->control_pin_locked = false;
    }

    if(ctx->force_pin_locked != false) {
      (void)ecu_config_gpio_input_unlock(ctx->config.input_force_pin);
      ctx->force_pin_locked = false;
    }
    ctx->configured = false;

  } while(0);

  return err;
}

void coolingfan_loop_slow(coolingfan_ctx_t *ctx)
{
  error_t err = E_OK;
  input_value_t input_value;
  bool input_bool;
  time_us_t now;
  time_us_t ckp_last_time;
  time_delta_us_t control_timeout;

  if(ctx->ready) {
    if(ctx->configured) {
      ckp_last_time = ctx->ckp_last_time;
      now = time_get_current_us();

      if(ctx->data.emergency_trigger && ctx->data.ignition_on) {
        ctx->data.working = true;
      }

      if(ctx->data.activate_trigger) {
        if(ctx->data.ckp_triggered && ctx->data.ignition_on) {
          ctx->data.working = true;
          ctx->ckp_work_accept = true;
        } else if(ctx->ckp_work_accept) {
          if(time_diff(now, ckp_last_time) >= ctx->config.crankshaft_timeout) {
            ctx->data.working = false;
            ctx->ckp_work_accept = false;
          } else {
            ctx->data.working = true;
          }
        }
      }

      if(ctx->force_pin_locked) {
        err = input_get_value(ctx->input_force_pin, &input_value, NULL);
        if(err == E_OK) {
          input_bool = input_value ? true : false;
          input_bool ^= ctx->config.force_gpio_inverted;
          if(input_bool && !ctx->data.working) {
            if(ctx->config.allow_force_when_stopped) {
              ctx->data.working = true;
            }
          }
        } else if(err != E_AGAIN) {
          ctx->diag.bits.input_trigger_error = true;
        }
      }

      if(ctx->data.ignition_on && ctx->data.ckp_detected &&
          ctx->ckp_rpm > 0.0f && ctx->ckp_rpm < ctx->config.crankshaft_rpm_min) {
        ctx->data.working = false;
      }

      if(ctx->data.force_engaged) {
        ctx->data.working = ctx->data.force_enabled;
      }

      if(ctx->working_prev != ctx->data.working) {
        if(ctx->output_pin_locked) {
          err = output_set_value(ctx->output_drive_pin, ctx->data.working);
          if(err == E_OK) {
            ctx->working_prev = ctx->data.working;
          } else {
            ctx->diag.bits.output_drive_error = true;
          }
        } else {
          ctx->working_prev = ctx->data.working;
        }
        ctx->control_time = now;
        ctx->control_working_valid = false;
      }

      if(ctx->data.working) {
        control_timeout = ctx->config.control_time_delay_on;
      } else {
        control_timeout = ctx->config.control_time_delay_off;
      }

      if(ctx->control_pin_locked) {
        if(ctx->control_working_valid == false) {
          if(time_diff(now, ctx->control_time) >= control_timeout) {
            ctx->control_working_status = ctx->data.working;
            ctx->control_working_valid = true;
          } else {
            ctx->control_working_valid = false;
          }
        } else {
          err = input_get_value(ctx->input_control_pin, &input_value, NULL);
          if(err == E_OK) {
            ctx->control_voltage = input_value * INPUTS_ANALOG_MULTIPLIER_R;
          } else if(err != E_AGAIN) {
            ctx->diag.bits.input_control_error = true;
          }

          if(ctx->control_pin_mode == ECU_GPIO_INPUT_TYPE_ANALOG) {
            if(ctx->data.working && ctx->control_voltage < ctx->config.control_voltage_threshold_on) {
              ctx->diag.bits.control_on_error = true;
            } else if(!ctx->data.working && ctx->control_voltage > ctx->config.control_voltage_threshold_off) {
              ctx->diag.bits.control_off_error = true;
            }
          } else if(ctx->control_pin_mode == ECU_GPIO_INPUT_TYPE_DIGITAL) {
            input_bool = input_value ? true : false;
            input_bool ^= ctx->config.control_gpio_inverted;

            if(ctx->data.working && !input_bool) {
              ctx->diag.bits.control_on_error = true;
            } else if(!ctx->data.working && input_bool) {
              ctx->diag.bits.control_off_error = true;
            }
          }
        }
      }
    }
  }
}

ITCM_FUNC void coolingfan_ckp_signal_update(coolingfan_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag)
{
  time_us_t now;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    now = time_get_current_us();

    if(data->validity < CKP_DATA_DETECTED) {
      ctx->data.ckp_triggered = false;
      ctx->data.ckp_detected = false;
    } else {
      ctx->data.ckp_detected = true;

      if(ctx->config.ckp_trigger == COOLINGFAN_CONFIG_CKP_TRIGGER_DETECTED && data->validity >= CKP_DATA_DETECTED) {
        ctx->data.ckp_triggered = true;
      } else if(ctx->config.ckp_trigger == COOLINGFAN_CONFIG_CKP_TRIGGER_SYNCHRONIZED && data->validity >= CKP_DATA_SYNCHRONIZED) {
        ctx->data.ckp_triggered = true;
      }

      if(ctx->data.ckp_triggered) {
        ctx->ckp_last_time = now;
      }
    }

    if(data->validity >= CKP_DATA_DETECTED) {
      ctx->ckp_rpm = data->rpm;
    } else {
      ctx->ckp_rpm = 0;
    }

  } while(0);
}

void coolingfan_ignition_update(coolingfan_ctx_t *ctx, bool ignition_on)
{

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    ctx->data.ignition_on = ignition_on;

  } while(0);
}

error_t coolingfan_get_data(coolingfan_ctx_t *ctx, coolingfan_data_t *data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *data = ctx->data;

  } while(0);

  return err;
}

error_t coolingfan_get_diag(coolingfan_ctx_t *ctx, coolingfan_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *diag = ctx->diag;

  } while(0);

  return err;
}

error_t coolingfan_emergency_trigger(coolingfan_ctx_t *ctx, bool emergency_trigger)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.emergency_trigger = emergency_trigger;

  } while(0);

  return err;
}

error_t coolingfan_activate_trigger(coolingfan_ctx_t *ctx, bool activate_trigger)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.activate_trigger = activate_trigger;

  } while(0);

  return err;
}

error_t coolingfan_force_reset(coolingfan_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.force_engaged = false;
    ctx->data.force_enabled = false;

  } while(0);

  return err;
}

error_t coolingfan_force_set(coolingfan_ctx_t *ctx, bool force_enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.force_engaged = true;
    ctx->data.force_enabled = force_enabled;

  } while(0);

  return err;
}
