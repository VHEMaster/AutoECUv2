/*
 * timing_base.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include "timing_common.h"
#include "timing_base.h"
#include "timing_ignition.h"
#include "timing_injection.h"
#include "timing_rough.h"
#include "config_hw.h"
#include "config_timings.h"

error_t timing_base_init(timing_base_ctx_t *ctx, const timing_base_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx->calibration_config == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(timing_base_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(timing_base_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t timing_base_configure(timing_base_ctx_t *ctx, const timing_base_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(timing_base_config_t));
    }

    if(ctx->config.enabled) {

      ctx->configured = true;
    }

  } while(0);

  return err;
}

error_t timing_base_reset(timing_base_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

ITCM_FUNC error_t timing_base_roughtest_set(timing_base_ctx_t *ctx, bool synchronized, bool sync_at_odd_rev)
{
  error_t err = E_OK;
  timing_base_data_crankshaft_t *crankshaft;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    crankshaft = &ctx->data.crankshaft;
    crankshaft->roughtest.synchronized = synchronized;
    crankshaft->roughtest.sync_at_odd_rev = sync_at_odd_rev;

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC void timing_base_ckp_signal_update(timing_base_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag)
{
  const timing_base_config_crankshaft_t *crankshaft_config;
  timing_base_data_crankshaft_t *crankshaft;
  timing_base_data_camshafts_t *camshafts;
  ckp_data_t ckp_sensor_data;
  float *position_values[2];
  bool odd_rev[2];
  float pos_temp;
  uint32_t prim;
  bool synchronized, sync_at_odd_rev;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);
    BREAK_IF(ctx->config.enabled == false);

    crankshaft = &ctx->data.crankshaft;
    crankshaft_config = &ctx->config.crankshaft;
    camshafts = &ctx->data.camshafts;

    ckp_sensor_data = *data;
    ctx->diag.crankshaft.bits.ckp_failure = diag->data ? true : false;

    position_values[0] = &ckp_sensor_data.current.position;
    position_values[1] = &ckp_sensor_data.previous.position;
    odd_rev[0] = ckp_sensor_data.current.odd_rev;
    odd_rev[1] = ckp_sensor_data.previous.odd_rev;


    if(ckp_sensor_data.validity == CKP_DATA_VALID) {
      if(crankshaft->mode < TIMING_CRANKSHAFT_MODE_VALID) {
        crankshaft->mode = TIMING_CRANKSHAFT_MODE_VALID;
      }

      if(crankshaft->mode >= TIMING_CRANKSHAFT_MODE_VALID) {
        if(ctx->config.phased_syncmode > TIMING_BASE_CONFIG_PHASEDSYNCMODE_DISABLED) {

          prim = EnterCritical();
          switch(ctx->config.phased_syncmode) {
            case TIMING_BASE_CONFIG_PHASEDSYNCMODE_CAMSHAFT:
              synchronized = camshafts->synchronized;
              sync_at_odd_rev = camshafts->sync_at_odd_rev;
              break;
            case TIMING_BASE_CONFIG_PHASEDSYNCMODE_ROUGHTEST:
              synchronized = crankshaft->roughtest.synchronized;
              sync_at_odd_rev = crankshaft->roughtest.sync_at_odd_rev;
              break;
            default:
              synchronized = false;
              sync_at_odd_rev = false;
              break;
          }

          if(camshafts->synchronized && crankshaft->roughtest.synchronized) {
            if(camshafts->sync_at_odd_rev != crankshaft->roughtest.sync_at_odd_rev) {
              ctx->diag.crankshaft.bits.roughtest_to_camshaft_missmatch = true;
            }
          }

          ExitCritical(prim);

          if(synchronized) {
            if(crankshaft->mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
              for(int i = 0; i < ITEMSOF(position_values); i++) {
                if(sync_at_odd_rev != odd_rev[i]) {
                  if(*position_values[i] < 0.0f) {
                    *position_values[i] += 360.0f;
                  } else {
                    *position_values[i] -= 360.0f;
                  }
                }
              }
            } else if(sync_at_odd_rev == ckp_sensor_data.odd_rev) {
              crankshaft->mode = TIMING_CRANKSHAFT_MODE_VALID_PHASED;
            }
          } else {
            crankshaft->mode = TIMING_CRANKSHAFT_MODE_VALID;
          }
        } else {
          crankshaft->mode = TIMING_CRANKSHAFT_MODE_VALID;
        }
      }

      for(int i = 0; i < ITEMSOF(position_values); i++) {
        pos_temp = *position_values[i];
        pos_temp += crankshaft_config->offset;

        timing_position_clamp(pos_temp, crankshaft->mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED, &pos_temp);
        *position_values[i] = pos_temp;
      }

      ckp_sensor_data.current_position = *position_values[0];
      if(ctx->config.phased_only) {
        if(crankshaft->mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
          crankshaft->valid = true;
        } else {
          crankshaft->valid = false;
        }
      } else {
        if(crankshaft->mode >= TIMING_CRANKSHAFT_MODE_VALID) {
          crankshaft->valid = true;
        } else {
          crankshaft->valid = false;
        }
      }
    } else {
      if(ckp_sensor_data.validity == CKP_DATA_DETECTED ||
        ckp_sensor_data.validity == CKP_DATA_SYNCHRONIZED) {
        crankshaft->mode = TIMING_CRANKSHAFT_MODE_DETECTED;
      } else {
        crankshaft->mode = TIMING_CRANKSHAFT_MODE_IDLE;
      }
      memset(camshafts->instances, 0, sizeof(camshafts->instances));
      camshafts->synchronized = false;
      crankshaft->valid = false;
    }

    prim = EnterCritical();
    crankshaft->sensor_data = ckp_sensor_data;
    crankshaft->pos_phased = *position_values[0];
    ExitCritical(prim);

  } while(0);
}

OPTIMIZE_FAST
ITCM_FUNC void timing_base_cmp_signal_update(timing_base_ctx_t *ctx, ecu_sensor_cmp_t cmp_instance, const cmp_data_t *data, const cmp_diag_t *diag)
{
  const timing_base_config_camshaft_t *camshaft_config;
  timing_base_data_camshafts_t *camshafts;
  timing_base_data_camshaft_t *camshaft;
  timing_base_data_crankshaft_t *crankshaft;
  uint32_t prim;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(cmp_instance >= ECU_SENSOR_CMP_MAX);
    BREAK_IF(ctx->configured == false);
    BREAK_IF(ctx->config.enabled == false);
    BREAK_IF(ctx->config.camshafts[cmp_instance].enabled == false);

    camshafts = &ctx->data.camshafts;
    camshaft = &camshafts->instances[cmp_instance];
    camshaft_config = &ctx->config.camshafts[cmp_instance];
    crankshaft = &ctx->data.crankshaft;

    camshaft->sensor_data = *data;
    ctx->diag.camshafts[cmp_instance].bits.cmp_failure = diag->data ? true : false;

    if(crankshaft->mode >= TIMING_CRANKSHAFT_MODE_VALID) {
      if(camshaft->sensor_data.validity == CMP_DATA_VALID) {
        if(camshaft_config->use_for_phased_sync) {
          if(crankshaft->mode == TIMING_CRANKSHAFT_MODE_VALID) {

            prim = EnterCritical();
            camshafts->valid = true;
            camshafts->synchronized = true;
            crankshaft->sync_phase_at_odd_rev = camshaft->sensor_data.sync_at_odd_rev;
            camshafts->sync_at_odd_rev = camshaft->sensor_data.sync_at_odd_rev;
            camshafts->sync_camshaft_instance = cmp_instance;
            ExitCritical(prim);

          } else if(camshafts->sync_at_odd_rev != camshaft->sensor_data.sync_at_odd_rev) {
            if(camshafts->sync_camshaft_instance == cmp_instance) {
              ctx->diag.camshafts[cmp_instance].bits.sync_lost = true;
            } else {
              ctx->diag.camshafts[cmp_instance].bits.sync_bad = true;
            }
          }
        }

        camshaft->pos_absolute = camshaft->sensor_data.position;
        camshaft->pos_relative = camshaft->pos_absolute - camshaft_config->pos_relative;

        if(camshaft->pos_relative >= 180.0f) {
          camshaft->pos_relative -= 360.0f;
        } else if(camshaft->pos_relative < -180.0f) {
          camshaft->pos_relative += 360.0f;
        }
        if(camshaft->pos_relative > camshaft_config->pos_max) {
          ctx->diag.camshafts[cmp_instance].bits.pos_too_late = true;
        } else if(camshaft->pos_relative < camshaft_config->pos_min) {
          ctx->diag.camshafts[cmp_instance].bits.pos_too_early = true;
        }
        camshaft->valid = true;
      } else if(camshaft->sensor_data.validity < CMP_DATA_DETECTED) {
        if(camshafts->synchronized) {
          ctx->diag.camshafts[cmp_instance].bits.signal_lost = true;
        }
      }
    } else {
      memset(camshafts->instances, 0, sizeof(camshafts->instances));
      camshafts->synchronized = false;
      camshafts->valid = false;
      camshaft->valid = false;
    }

  } while(0);
}

OPTIMIZE_FAST
ITCM_FUNC void timing_base_signal_update_cb(void *usrdata, const timing_base_data_t *data, const timing_base_diag_t *diag)
{
  timing_base_ctx_t *ctx = (timing_base_ctx_t *)usrdata;
  ignition_ctx_t *ignition_ctx = NULL;
  injection_ctx_t *injection_ctx = NULL;
  rough_ctx_t *rough_ctx = NULL;
  const ecu_config_engine_calibration_t *calibration_config = NULL;

  timing_base_cylinder_sequentialed_cylinder_ctx_t *sequentialed;
  float pos_offset;
  error_t err;
  bool enabled;
  ignition_config_group_mode_t group_mode_ign;
  injection_config_group_mode_t group_mode_inj;
  bool semisequential_needed = false;
  bool sequential_needed = false;
  bool ignition_update_trigger = false;
  bool injection_update_trigger = false;
  bool rough_update_trigger = false;
  uint32_t process_update_trigger_counter = ctx->process_update_trigger_counter;
  uint8_t process_update_trigger_counter_1of2 = process_update_trigger_counter & 1;

  do {
    BREAK_IF(ctx->configured == false);
    BREAK_IF(ctx->config.enabled == false);

    if(ctx->runtime.configured != true) {
      // TODO: assign proper instance
      err = ecu_timings_get_ignition_ctx(ECU_TIMING_IGNITION_1, &ignition_ctx);
      BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
      BREAK_IF_ACTION(ignition_ctx == NULL, err = E_FAULT);
      // TODO: assign proper instance
      err = ecu_timings_get_injection_ctx(ECU_TIMING_INJECTION_1, &injection_ctx);
      BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
      BREAK_IF_ACTION(injection_ctx == NULL, err = E_FAULT);
      // TODO: assign proper instance
      err = ecu_timings_get_rough_ctx(ECU_TIMING_ROUGH_1, &rough_ctx);
      BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
      BREAK_IF_ACTION(rough_ctx == NULL, err = E_FAULT);

      ctx->runtime.ignition_ctx = ignition_ctx;
      ctx->runtime.injection_ctx = injection_ctx;
      ctx->runtime.rough_ctx = rough_ctx;
      ctx->runtime.configured = true;
    } else {
      ignition_ctx = ctx->runtime.ignition_ctx;
      injection_ctx = ctx->runtime.injection_ctx;
      rough_ctx = ctx->runtime.rough_ctx;
    }
    calibration_config = ctx->init.calibration_config;

    if(ctx->data.crankshaft.mode >= TIMING_CRANKSHAFT_MODE_VALID) {
      ecu_config_set_ignition_enabled(true);

      err = ecu_timings_get_timing_enabled(ECU_TIMING_TYPE_IGNITION, ECU_TIMING_IGNITION_1, &enabled);
      if(err == E_OK && enabled) {
        if(ignition_ctx->config.process_update_trigger == IGNITION_CONFIG_PROCESS_UPDATE_TRIGGER_ALWAYS) {
          ignition_update_trigger = true;
        } else if(ignition_ctx->config.process_update_trigger == IGNITION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_1ST) {
          ignition_update_trigger = !process_update_trigger_counter_1of2;
        } else if(ignition_ctx->config.process_update_trigger == IGNITION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_2ND) {
          ignition_update_trigger = process_update_trigger_counter_1of2;
        }
      }

      err = ecu_timings_get_timing_enabled(ECU_TIMING_TYPE_INJECTION, ECU_TIMING_INJECTION_1, &enabled);
      if(err == E_OK && enabled) {
        if(injection_ctx->config.process_update_trigger == INJECTION_CONFIG_PROCESS_UPDATE_TRIGGER_ALWAYS) {
          injection_update_trigger = true;
        } else if(injection_ctx->config.process_update_trigger == INJECTION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_1ST) {
          injection_update_trigger = !process_update_trigger_counter_1of2;
        } else if(injection_ctx->config.process_update_trigger == INJECTION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_2ND) {
          injection_update_trigger = process_update_trigger_counter_1of2;
        }
      }

      err = ecu_timings_get_timing_enabled(ECU_TIMING_TYPE_ROUGH, ECU_TIMING_ROUGH_1, &enabled);
      if(err == E_OK && enabled) {
        if(rough_ctx->config.rough_measure_range != 0.0f) {
          rough_update_trigger = true;
          sequential_needed = true;
        }
      }

      if(ignition_update_trigger) {
        for(ignition_config_group_t gr = 0; gr < IGNITION_CONFIG_GROUP_MAX; gr++) {
          if(!semisequential_needed || !sequential_needed) {
            if(ignition_ctx->config.groups[gr].enabled) {
              group_mode_ign = ignition_ctx->config.groups[gr].mode;
              if(!semisequential_needed) {
                if(group_mode_ign == IGNITION_CONFIG_GROUP_MODE_DISTRIBUTOR ||
                    group_mode_ign == IGNITION_CONFIG_GROUP_MODE_SEMISEQUENTIAL_ONLY ||
                    group_mode_ign == IGNITION_CONFIG_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
                  semisequential_needed = true;
                }
              }
              if(!sequential_needed) {
                if(group_mode_ign == IGNITION_CONFIG_GROUP_MODE_SEQUENTIAL_ONLY ||
                    group_mode_ign == IGNITION_CONFIG_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
                  sequential_needed = true;
                }
              }
            }
          } else {
            break;
          }
        }
      }

      if(injection_update_trigger) {
        for(injection_config_group_t gr = 0; gr < INJECTION_CONFIG_GROUP_MAX; gr++) {
          if(!semisequential_needed || !sequential_needed) {
            if(injection_ctx->config.groups[gr].enabled) {
              group_mode_inj = injection_ctx->config.groups[gr].mode;
              if(!semisequential_needed) {
                if(group_mode_inj == INJECTION_CONFIG_GROUP_MODE_MONO ||
                    group_mode_inj == INJECTION_CONFIG_GROUP_MODE_SEMISEQUENTIAL_ONLY ||
                    group_mode_inj == INJECTION_CONFIG_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
                  semisequential_needed = true;
                }
              }
              if(!sequential_needed) {
                if(group_mode_inj == INJECTION_CONFIG_GROUP_MODE_SEQUENTIAL_ONLY ||
                    group_mode_inj == INJECTION_CONFIG_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL) {
                  sequential_needed = true;
                }
              }
            }
          } else {
            break;
          }
        }
      }

      if(sequential_needed) {
        for(ecu_cylinder_t cy = 0; cy < calibration_config->cylinders.cylinders_count; cy++) {
          pos_offset = calibration_config->cylinders.cylinders[cy].pos_offset;

          sequentialed = &ctx->data.sequentialed[TIMING_RUNTIME_CYLINDER_SEQUENTIAL].cylinders[cy];
          err = timing_calculate_offset_position(ctx,
              pos_offset, true,
              &sequentialed->timing_req,
              &sequentialed->crankshaft_data);
          BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
        }
      }

      if(semisequential_needed) {
        for(ecu_cylinder_t cy = 0; cy < calibration_config->cylinders.cylinders_count; cy++) {
          pos_offset = calibration_config->cylinders.cylinders[cy].pos_offset;

          sequentialed = &ctx->data.sequentialed[TIMING_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR].cylinders[cy];
          err = timing_calculate_offset_position(ctx,
              pos_offset, false,
              &sequentialed->timing_req,
              &sequentialed->crankshaft_data);
          BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
        }
      }
    } else {
      ecu_config_set_ignition_enabled(false);

      for(ignition_config_group_t gr = 0; gr < IGNITION_CONFIG_GROUP_MAX; gr++) {
        if(ignition_ctx->config.groups[gr].enabled) {
          for(ecu_cylinder_t cy = 0; cy < calibration_config->cylinders.cylinders_count; cy++) {
            err = ecu_config_gpio_output_write(ignition_ctx->config.groups[gr].cylinders[cy].output_pin, 0);
            BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
          }
        }
      }

      for(injection_config_group_t gr = 0; gr < INJECTION_CONFIG_GROUP_MAX; gr++) {
        if(injection_ctx->config.groups[gr].enabled) {
          for(ecu_cylinder_t cy = 0; cy < calibration_config->cylinders.cylinders_count; cy++) {
            err = ecu_config_gpio_output_write(injection_ctx->config.groups[gr].cylinders[cy].output_pin, 0);
            BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
          }
        }
      }

      ignition_update_trigger = true;
      injection_update_trigger = true;
      rough_update_trigger = true;
    }

    if(ignition_update_trigger) {
      ignition_signal_update_callback(ignition_ctx);
    }
    if(injection_update_trigger) {
      injection_signal_update_callback(injection_ctx);
    }
    if(rough_update_trigger) {
      rough_signal_update_callback(rough_ctx);
    }
  } while(0);

  ctx->process_update_trigger_counter = process_update_trigger_counter + 1;
}

ITCM_FUNC error_t timing_base_get_crankshaft_data(timing_base_ctx_t *ctx, timing_base_data_crankshaft_t *data)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    prim = EnterCritical();
    *data = ctx->data.crankshaft;
    ExitCritical(prim);

  } while(0);

  return err;
}

ITCM_FUNC error_t timing_base_get_data(timing_base_ctx_t *ctx, timing_base_data_t *data)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    prim = EnterCritical();
    *data = ctx->data;
    ExitCritical(prim);

  } while(0);

  return err;
}

ITCM_FUNC error_t timing_base_get_data_ptr(timing_base_ctx_t *ctx, const timing_base_data_t **data)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    *data = &ctx->data;

  } while(0);

  return err;
}

ITCM_FUNC error_t timing_base_get_diag(timing_base_ctx_t *ctx, timing_base_diag_t *diag)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    prim = EnterCritical();
    *diag = ctx->diag;
    ExitCritical(prim);

  } while(0);

  return err;
}
