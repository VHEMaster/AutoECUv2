/*
 * timing_knockwindow.c
 *
 *  Created on: 5 окт. 2026 г.
 *      Author: VHEMaster
 */



#include "config_global.h"
#include "timing_common.h"
#include "config_hw.h"
#include "common.h"
#include "interpolation.h"

static void knockwindow_pulsedadc_sampling_cplt_cb_t(void *usrdata, const pulsedadc_sampling_cplt_ctx_t *cplt_ctx);
static void knockwindow_pulsedadc_sampling_error_cb_t(void *usrdata);

error_t knockwindow_init(knockwindow_ctx_t *ctx, const knockwindow_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx->calibration_config == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(knockwindow_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(knockwindow_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t knockwindow_configure(knockwindow_ctx_t *ctx, const knockwindow_config_t *config)
{
  error_t err = E_OK;
  const knockwindow_config_setup_ctx_t *config_setup;
  knockwindow_setup_ctx_t *setup;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(knockwindow_config_t));
    }

    if(ctx->config.enabled) {
      for(uint8_t i = 0; i < config->setups_count; i++) {
        config_setup = &ctx->config.setups[i];
        setup = &ctx->setups[i];

        err = ecu_devices_get_pulsedadc_ctx(config_setup->pulsedadc_instance, &setup->pulsedadc_ctx);
        BREAK_IF(err != E_OK);

        err = ecu_devices_pulsedadc_register_cb(config_setup->pulsedadc_instance,
            knockwindow_pulsedadc_sampling_cplt_cb_t,
            knockwindow_pulsedadc_sampling_error_cb_t,
            setup);
        BREAK_IF(err != E_OK);
      }

      ctx->configured = true;
    }

  } while(0);

  return err;
}

error_t knockwindow_reset(knockwindow_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

error_t knockwindow_get_runtime_data_ptr(knockwindow_ctx_t *ctx, knockwindow_runtime_ctx_t **runtime_data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(runtime_data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *runtime_data = &ctx->runtime;

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC void knockwindow_signal_update_callback(knockwindow_ctx_t *ctx)
{
  error_t err;
  timing_base_crankshaft_mode_t crankshaft_mode;
  const ecu_config_engine_calibration_t *calibration_config;
  const timing_base_data_crankshaft_t *crankshaft_data;
  const timing_base_data_t *timing_base_data;
  const knockwindow_config_t *config;
  const knockwindow_config_setup_ctx_t *config_setup;
  const knockwindow_config_setup_cylinder_t *cy_config;
  const knockwindow_runtime_input_ctx_t *inputs;
  knockwindow_runtime_ctx_t *runtime;
  knockwindow_setup_runtime_ctx_t *setup_runtime;
  knockwindow_setup_runtime_cylinder_ctx_t *runtime_cy;
  knockwindow_setup_ctx_t *setup;
  uint16_t samples_requested;
  time_float_delta_us_t window_delta;
  ecu_bank_t bank_cy;

  uint32_t cylinders_count;
  float window_start, window_end, window_prepare;

  do {
    config = &ctx->config;

    BREAK_IF(ctx->configured == false);
    BREAK_IF(config->enabled == false);

    // TODO: assign proper instance
    err = ecu_timings_base_get_data_ptr(ECU_TIMING_BASE_1, &timing_base_data);
    BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
    BREAK_IF_ACTION(timing_base_data == NULL, err = E_FAULT);

    crankshaft_mode = timing_base_data->crankshaft.mode;
    BREAK_IF(crankshaft_mode < TIMING_CRANKSHAFT_MODE_VALID);

    calibration_config = ctx->init.calibration_config;
    cylinders_count = calibration_config->cylinders.cylinders_count;

    for(knockwindow_config_setup_t s = 0; s < config->setups_count; s++) {
      setup = &ctx->setups[s];
      BREAK_IF(setup->pulsedadc_ctx == NULL);

      setup->timing_base_data = timing_base_data;
      config_setup = &config->setups[s];

      runtime = &ctx->runtime;

      setup_runtime = &setup->runtime;

      for(ecu_cylinder_t i = 0, cy; i < cylinders_count; i++) {
        cy_config = &config_setup->cylinders_supported[i];

        cy = cy_config->cy;

        CONTINUE_IF(cy_config->enabled == false && setup_runtime->cylinder_occupied != cy);

        bank_cy = calibration_config->cylinders.cylinders[cy].bank;
        inputs = &runtime->input_banked[bank_cy];
        CONTINUE_IF(inputs->allowed.valid != true);
        CONTINUE_IF(inputs->allowed.value != ECU_RUNTIME_PARAMETER_TRUE);

        CONTINUE_IF(inputs->knock_window_start.valid != true);
        CONTINUE_IF(inputs->knock_window_end.valid != true);

        window_start = inputs->knock_window_start.value;
        window_end = inputs->knock_window_end.value;
        window_prepare = window_start - config->window_prepare_advance;

        crankshaft_data = &timing_base_data->sequentialed[TIMING_RUNTIME_CYLINDER_SEQUENTIAL].cylinders[cy].crankshaft_data;
        runtime_cy = &setup_runtime->cylinder[cy];

        if(runtime_cy->state == KNOCKWINDOW_STATE_SYNC) {
          if(crankshaft_data->sensor_data.current.position > window_end || crankshaft_data->sensor_data.current.position < window_prepare) {
            runtime_cy->state = KNOCKWINDOW_STATE_WAITING;
          }
        } else if(runtime_cy->state == KNOCKWINDOW_STATE_WAITING) {
          if(setup_runtime->working == false) {
            if(crankshaft_data->sensor_data.current.position >= window_prepare && crankshaft_data->sensor_data.current.position < window_end) {
              if(crankshaft_data->sensor_data.current.position < window_start) {
                window_delta = (window_end - window_start) * crankshaft_data->sensor_data.us_per_degree_pulsed;
                samples_requested = window_delta * setup->pulsedadc_ctx->sampling_frequency * TIME_S_IN_US;
                setup_runtime->cylinder_occupied = cy;
                setup_runtime->position_start = window_start;
                setup_runtime->position_cplt = window_end;
                setup_runtime->samples_requested = samples_requested;
                setup_runtime->cplt_irq = false;
                setup_runtime->error_irq = false;
                err = pulsedadc_prepare(setup->pulsedadc_ctx, samples_requested);
                if(err == E_OVERFLOW) {
                  runtime_cy->state = KNOCKWINDOW_STATE_SYNC;
                } else  if(err != E_OK) {
                  runtime_cy->state = KNOCKWINDOW_STATE_SYNC;
                  // TODO: error handling
                  break;
                }
                runtime_cy->state = KNOCKWINDOW_STATE_PREPARED;
                setup_runtime->working = true;
              } else {
                runtime_cy->state = KNOCKWINDOW_STATE_SYNC;
                // TODO: OVERSHOOT
                break;
              }
            }
          }
        } else if(setup_runtime->working == true && setup_runtime->cylinder_occupied == cy) {
          if(runtime_cy->state == KNOCKWINDOW_STATE_PREPARED) {
            if(crankshaft_data->sensor_data.current.position >= window_start) {
              if(crankshaft_data->sensor_data.current.position < window_end) {
                err = pulsedadc_start(setup->pulsedadc_ctx);
                if(err != E_OK) {
                  runtime_cy->state = KNOCKWINDOW_STATE_SYNC;
                  setup_runtime->cylinder_occupied = ECU_CYLINDER_MAX;
                  setup_runtime->working = false;
                  // TODO: error handling
                  break;
                }
                runtime_cy->state = KNOCKWINDOW_STATE_SAMPLING;
              } else {
                runtime_cy->state = KNOCKWINDOW_STATE_SYNC;
                setup_runtime->cylinder_occupied = ECU_CYLINDER_MAX;
                setup_runtime->working = false;
                // TODO: OVERSHOOT
                break;
              }
            }
          } else if(runtime_cy->state == KNOCKWINDOW_STATE_SAMPLING) {
            if(setup_runtime->cplt_irq) {
              setup->sampling_cplt_ctx.position_start = setup_runtime->position_start;
              setup->sampling_cplt_ctx.position_cplt = setup_runtime->position_cplt;
              setup->sampling_cplt_ctx.cylinder = setup_runtime->cylinder_occupied;
              setup->sampling_cplt_ctx.setup_index = s;

              if(ctx->init.callback != NULL) {
                ctx->init.callback(ctx->init.callback_usrdata, &setup->sampling_cplt_ctx);
              }

              runtime_cy->state = KNOCKWINDOW_STATE_SYNC;
              setup_runtime->cplt_irq = false;
              setup_runtime->cylinder_occupied = ECU_CYLINDER_MAX;
              setup_runtime->working = false;
            } else if(setup_runtime->error_irq) {
              runtime_cy->state = KNOCKWINDOW_STATE_SYNC;
              setup_runtime->error_irq = false;
              setup_runtime->cylinder_occupied = ECU_CYLINDER_MAX;
              setup_runtime->working = false;
            }

            // TODO: timeout handling
            /*
            if(crankshaft_data->sensor_data.current.position >= window_end) {
              runtime_cy->state = KNOCKWINDOW_STATE_SYNC;
              setup_runtime->working = false;
            }
            */
          }
        }
      }
    }
  } while(0);
}

ITCM_FUNC static void knockwindow_pulsedadc_sampling_cplt_cb_t(void *usrdata, const pulsedadc_sampling_cplt_ctx_t *cplt_ctx)
{
  knockwindow_setup_ctx_t *setup = (knockwindow_setup_ctx_t *)usrdata;

  setup->sampling_cplt_ctx.samples_buffer = cplt_ctx->samples_buffer;
  setup->sampling_cplt_ctx.samples_count = cplt_ctx->samples_count;
  setup->sampling_cplt_ctx.time_start = cplt_ctx->time_start;
  setup->sampling_cplt_ctx.time_cplt = cplt_ctx->time_cplt;

  setup->runtime.cplt_irq = true;
}

ITCM_FUNC static void knockwindow_pulsedadc_sampling_error_cb_t(void *usrdata)
{
  knockwindow_setup_ctx_t *setup = (knockwindow_setup_ctx_t *)usrdata;

  setup->runtime.error_irq = true;
}
