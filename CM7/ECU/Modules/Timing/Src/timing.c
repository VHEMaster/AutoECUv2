/*
 * timing.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include "timing.h"
#include "timing_internal.h"
#include "compiler.h"
#include <string.h>

error_t timing_init(timing_ctx_t *ctx, const timing_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(timing_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(timing_init_ctx_t));


    ctx->ready = true;

  } while(0);

  return err;
}

error_t timing_configure(timing_ctx_t *ctx, const timing_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(timing_config_t));
    }

    ctx->configured = true;

  } while(0);

  return err;
}

error_t timing_reset(timing_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

  } while(0);

  return err;
}

ITCM_FUNC INLINE void timing_position_clamp(float input, bool phased, float *output)
{
  float out_value = input;

  if(output == NULL) {
    return;
  }

  if(phased) {
    while(out_value >= 360.0f) {
      out_value -= 720.0f;
    }
    while(out_value < -360.0f) {
      out_value += 720.0f;
    }
  } else {
    while(out_value >= 180.0f) {
      out_value -= 360.0f;
    }
    while(out_value < -180.0f) {
      out_value += 360.0f;
    }
  }

  *output = out_value;
}

ITCM_FUNC error_t timing_roughtest_set(timing_ctx_t *ctx, bool synchronized, bool sync_at_odd_rev)
{
  error_t err = E_OK;
  timing_data_crankshaft_t *crankshaft;

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

ITCM_FUNC void timing_ckp_signal_update(timing_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag)
{
  const timing_config_crankshaft_t *crankshaft_config;
  timing_data_crankshaft_t *crankshaft;
  timing_data_camshafts_t *camshafts;
  ckp_data_t ckp_sensor_data;
  float *position_values[2];
  bool odd_rev[2];
  float pos_temp;
  uint32_t prim;
  bool synchronized, sync_at_odd_rev;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

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
        if(ctx->config.phased_syncmode > TIMING_CONFIG_PHASEDSYNCMODE_DISABLED) {

          prim = EnterCritical();
          switch(ctx->config.phased_syncmode) {
            case TIMING_CONFIG_PHASEDSYNCMODE_CAMSHAFT:
              synchronized = camshafts->synchronized;
              sync_at_odd_rev = camshafts->sync_at_odd_rev;
              break;
            case TIMING_CONFIG_PHASEDSYNCMODE_ROUGHTEST:
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

ITCM_FUNC void timing_cmp_signal_update(timing_ctx_t *ctx, ecu_sensor_cmp_t cmp_instance, const cmp_data_t *data, const cmp_diag_t *diag)
{
  const timing_config_camshaft_t *camshaft_config;
  timing_data_camshafts_t *camshafts;
  timing_data_camshaft_t *camshaft;
  timing_data_crankshaft_t *crankshaft;
  uint32_t prim;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(cmp_instance >= ECU_SENSOR_CMP_MAX);
    BREAK_IF(ctx->configured == false);
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

error_t timing_get_data(timing_ctx_t *ctx, timing_data_t *data)
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

error_t timing_get_diag(timing_ctx_t *ctx, timing_diag_t *diag)
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

ITCM_FUNC error_t timing_calculate_current_position(timing_ctx_t *ctx, float offset, bool phased, timing_req_t *req_ctx, timing_data_crankshaft_t *data)
{
  error_t err = E_OK;
  float pos, pos_prev, mult, time_delta, current, previous;
  timing_data_crankshaft_t data_cur;
  time_us_t now;
  uint32_t prim;
  float *positions[2];
  float pos_temp;
  bool phased_internal = phased;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    prim = EnterCritical();
    data_cur = ctx->data.crankshaft;
    ExitCritical(prim);

    now = time_get_current_us();

    positions[0] = &data_cur.sensor_data.current.position;
    positions[1] = &data_cur.sensor_data.previous.position;

    if(data_cur.mode != TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
      phased_internal = false;
    } else if(req_ctx != NULL && req_ctx->phased == false && ctx->config.phased_only == false) {
      phased_internal = false;
    }

    for(int i = 0; i < ITEMSOF(positions); i++) {
      pos_temp = *positions[i];
      pos_temp += offset;
      timing_position_clamp(pos_temp, phased_internal, &pos_temp);

      *positions[i] = pos_temp;
    }

    pos = data_cur.sensor_data.current.position;

    if(data_cur.valid && data_cur.mode >= TIMING_CRANKSHAFT_MODE_VALID) {

      if(req_ctx == NULL || req_ctx->position_valid) {
        if(data_cur.sensor_data.current.timestamp != data_cur.sensor_data.previous.timestamp) {
          time_delta = time_diff(data_cur.sensor_data.current.timestamp, data_cur.sensor_data.previous.timestamp);
          now = time_diff(now, data_cur.sensor_data.previous.timestamp);

          current = data_cur.sensor_data.current.position;
          previous = data_cur.sensor_data.previous.position;

          if(current < previous) {
            if(phased_internal) {
              current += 720.0f;
            } else {
              current += 360.0f;
            }
          }

          pos = current - previous;
          mult = pos / time_delta;
          pos = mult * now + previous;
        }

        timing_position_clamp(pos, phased_internal, &pos);

        if(req_ctx != NULL) {
          pos_prev = req_ctx->position_prev;

          if((pos - pos_prev < 0.0f && pos - pos_prev > -90.0f) || pos - pos_prev > 90.0f) {
            pos = pos_prev;
          }

          if(phased && !phased_internal && data_cur.mode == TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
            if(pos > 90.0f) {
              req_ctx->phased = true;
              phased_internal = true;
            }
          } else if(req_ctx->phased && !phased_internal && data_cur.mode < TIMING_CRANKSHAFT_MODE_VALID_PHASED) {
            pos_temp = req_ctx->position_prev;
            if(pos_temp > 90.0f || pos_temp <= -180.0f) {
              req_ctx->phased = false;

              timing_position_clamp(pos_temp, false, &pos_temp);
              req_ctx->position_prev = pos_temp;

              timing_position_clamp(pos, false, &pos);

              for(int i = 0; i < ITEMSOF(positions); i++) {
                pos_temp = *positions[i];
                pos_temp += offset;

                timing_position_clamp(pos_temp, false, &pos_temp);
                *positions[i] = pos_temp;
              }
            }
          }
        }
      } else {
        pos = data_cur.sensor_data.current.position;
      }

      //Check for NaNs
      if(pos != pos) {
        ctx->diag.crankshaft.bits.pos_calc_nan = true;
        data_cur.valid = false;
        pos = 0.0f;
      }

      if(req_ctx != NULL) {
        req_ctx->position_prev = pos;
        req_ctx->position_valid = true;
      }

      if(!phased_internal) {
        data_cur.mode = MIN(data_cur.mode, TIMING_CRANKSHAFT_MODE_VALID);
      } else if(data_cur.mode > TIMING_CRANKSHAFT_MODE_VALID) {
        data_cur.mode = TIMING_CRANKSHAFT_MODE_VALID_PHASED;
      }

    } else {
      if(req_ctx != NULL) {
        req_ctx->position_prev = pos;
        req_ctx->position_valid = false;
        req_ctx->phased = false;
      }
    }

    data_cur.sensor_data.current_position = pos;
    data_cur.pos_phased = pos;

    if(data != NULL) {
      *data = data_cur;
    }
  } while(0);

  return err;
}
