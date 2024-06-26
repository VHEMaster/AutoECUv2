/*
 * fuelpump.c
 *
 *  Created on: Jun 21, 2024
 *      Author: VHEMaster
 */

#include "fuelpump.h"
#include "compiler.h"
#include <string.h>

error_t fuelpump_init(fuelpump_ctx_t *ctx, const fuelpump_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(fuelpump_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(fuelpump_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

error_t fuelpump_configure(fuelpump_ctx_t *ctx, const fuelpump_config_t *config)
{
  error_t err = E_OK;
  bool valid, support;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = fuelpump_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(fuelpump_config_t));
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

      err = ecu_config_gpio_input_valid(ctx->config.input_trigger_pin, &valid);
      BREAK_IF(err != E_OK);

      if(valid) {
        err = ecu_config_gpio_input_lock(ctx->config.input_trigger_pin);
        BREAK_IF(err != E_OK);
        ctx->trigger_pin_locked = true;

        err = ecu_config_gpio_input_set_mode(ctx->config.input_trigger_pin, ECU_GPIO_INPUT_TYPE_DIGITAL);
        BREAK_IF(err != E_OK);

        err = ecu_config_gpio_input_get_id(ctx->config.input_trigger_pin, &ctx->input_trigger_pin);
        BREAK_IF(err != E_OK);
      }

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)fuelpump_reset(ctx);
  }

  return err;
}

error_t fuelpump_reset(fuelpump_ctx_t *ctx)
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

    if(ctx->trigger_pin_locked != false) {
      (void)ecu_config_gpio_input_unlock(ctx->config.input_trigger_pin);
      ctx->trigger_pin_locked = false;
    }
    ctx->configured = false;

  } while(0);

  return err;
}

void fuelpump_loop_slow(fuelpump_ctx_t *ctx)
{
  error_t err = E_OK;
  input_value_t input_value;
  bool input_bool;
  time_us_t now;
  bool ignpower_trig;
  bool init_trig;
  time_us_t ckp_last_time;
  time_delta_us_t control_timeout;

  if(ctx->ready) {
    if(ctx->configured) {
      ckp_last_time = ctx->ckp_last_time;
      now = time_now_us();

      ignpower_trig = ctx->data.ignpower_on;
      ignpower_trig |= ctx->config.trigger_source == FUELPUMP_CONFIG_TRIGGER_ALWAYS_ON;

      init_trig = ignpower_trig;
      init_trig |= ctx->config.trigger_source != FUELPUMP_CONFIG_TRIGGER_CRANKSHAFT;

      while(true) {
        switch(ctx->fsm_state) {
          case FUELPUMP_FSM_INIT:
            if(init_trig) {
              ctx->fsm_state_time = now;
              ctx->fsm_state = FUELPUMP_FSM_STARTUP;
              continue;
            } else {
              ctx->data.working = false;
            }
            break;
          case FUELPUMP_FSM_STARTUP:
            if(init_trig) {
              ctx->data.working = true;
              if(time_diff(now, ctx->fsm_state_time) >= ctx->config.prepumping_time) {
                ctx->fsm_state = FUELPUMP_FSM_PROCESS;
                continue;
              }
            } else {
              ctx->data.working = false;
              ctx->fsm_state = FUELPUMP_FSM_INIT;
            }
            break;
          case FUELPUMP_FSM_PROCESS:
            if(ctx->config.trigger_source == FUELPUMP_CONFIG_TRIGGER_ALWAYS_ON ||
                ctx->config.trigger_source == FUELPUMP_CONFIG_TRIGGER_IGNPOWER) {
              ctx->data.working = ignpower_trig;
            } else if(ctx->config.trigger_source == FUELPUMP_CONFIG_TRIGGER_CRANKSHAFT) {
              if(ctx->data.ignpower_on == false) {
                ctx->data.working = false;
                ctx->fsm_state = FUELPUMP_FSM_INIT;
              } else if(ctx->data.ckp_triggered) {
                ctx->data.working = true;
              } else if(ctx->data.working) {
                if(time_diff(now, ckp_last_time) >= ctx->config.crankshaft_timeout) {
                  ctx->data.working = false;
                } else {
                  ctx->data.working = true;
                }
              }
            } else if(ctx->config.trigger_source == FUELPUMP_CONFIG_TRIGGER_GPIO_INPUT) {
              if(ctx->trigger_pin_locked) {
                err = input_get_value(ctx->input_trigger_pin, &input_value, NULL);
                if(err == E_OK) {
                  input_bool = input_value ? true : false;
                  input_bool ^= ctx->config.trigger_gpio_inverted;
                  ctx->data.working = input_bool;
                } else if(err != E_AGAIN) {
                  ctx->diag.bits.input_trigger_error = true;
                }
              } else {
                ctx->data.working = false;
              }
            } else if(ctx->config.trigger_source == FUELPUMP_CONFIG_TRIGGER_MANUAL) {
              ctx->data.working = ctx->data.manual_enabled;
            } else {
              ctx->data.working = false;
            }
            break;
          default:
            break;
        }
        break;
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

ITCM_FUNC void fuelpump_ckp_signal_update(fuelpump_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag)
{
  time_us_t now;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    now = time_now_us();

    if(data->validity < CKP_DATA_DETECTED) {
      ctx->data.ckp_triggered = false;
    } else {
      if(ctx->config.ckp_trigger == FUELPUMP_CONFIG_CKP_TRIGGER_DETECTED && data->validity >= CKP_DATA_DETECTED) {
        ctx->data.ckp_triggered = true;
      } else if(ctx->config.ckp_trigger == FUELPUMP_CONFIG_CKP_TRIGGER_SYNCHRONIZED && data->validity >= CKP_DATA_SYNCHRONIZED) {
        ctx->data.ckp_triggered = true;
      }

      if(ctx->data.ckp_triggered) {
        ctx->ckp_last_time = now;
      }
    }

  } while(0);
}

void fuelpump_ignpower_update(fuelpump_ctx_t *ctx, bool ignpower_on)
{

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    ctx->data.ignpower_on = ignpower_on;

  } while(0);
}

error_t fuelpump_get_data(fuelpump_ctx_t *ctx, fuelpump_data_t *data)
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

error_t fuelpump_get_diag(fuelpump_ctx_t *ctx, fuelpump_diag_t *diag)
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

error_t fuelpump_manual_set(fuelpump_ctx_t *ctx, bool manual_enabled)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    ctx->data.manual_enabled = manual_enabled;

  } while(0);

  return err;
}

error_t fuelpump_force_reset(fuelpump_ctx_t *ctx)
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

error_t fuelpump_force_set(fuelpump_ctx_t *ctx, bool force_enabled)
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
