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

ITCM_FUNC void timing_ckp_signal_update(timing_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag)
{
  const timing_config_crankshaft_t *crankshaft_config;
  timing_data_crankshaft_t *crankshaft;
  timing_data_camshafts_t *camshafts;
  bool phased = false;
  float *position_values[2];
  float pos_temp;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    crankshaft = &ctx->data.crankshaft;
    crankshaft_config = &ctx->config.crankshaft;
    camshafts = &ctx->data.camshafts;

    crankshaft->sensor_data = *data;
    crankshaft->sensor_diag = *diag;

    position_values[0] = &crankshaft->sensor_data.current.position;
    position_values[1] = &crankshaft->sensor_data.previous.position;

    if(crankshaft->sensor_data.validity == CKP_DATA_VALID) {
      if(crankshaft->mode < TIMING_CRANKSHAFT_MODE_VALID) {
        crankshaft->mode = TIMING_CRANKSHAFT_MODE_VALID;
      }

      if(crankshaft->mode >= TIMING_CRANKSHAFT_MODE_VALID) {
        if(camshafts->synchronized) {
          if(camshafts->sync_at_odd_rev != crankshaft->sensor_data.odd_rev) {
            for(int i = 0; i < ITEMSOF(position_values); i++) {
              if(*position_values[i] < 0.0f) {
                *position_values[i] += 360.0f;
              } else {
                *position_values[i] -= 360.0f;
              }
            }
          }
          crankshaft->mode = TIMING_CRANKSHAFT_MODE_VALID_PHASED;
          phased = true;
        } else {
          crankshaft->mode = TIMING_CRANKSHAFT_MODE_VALID;
        }
      }

      for(int i = 0; i < ITEMSOF(position_values); i++) {
        pos_temp = *position_values[i];
        pos_temp += crankshaft_config->offset;
        if(phased) {
          if(pos_temp >= 360.0f) {
            pos_temp -= 720.0f;
          } else if(pos_temp < -360.0f) {
            pos_temp += 720.0f;
          }
        } else {
          if(pos_temp >= 180.0f) {
            pos_temp -= 360.0f;
          } else if(pos_temp < -180.0f) {
            pos_temp += 360.0f;
          }
        }
        *position_values[i] = pos_temp;
      }

      crankshaft->sensor_data.current_position = *position_values[0];
    } else {
      if(crankshaft->sensor_data.validity == CKP_DATA_DETECTED ||
        crankshaft->sensor_data.validity == CKP_DATA_SYNCHRONIZED) {
        crankshaft->mode = TIMING_CRANKSHAFT_MODE_DETECTED;
      } else {
        crankshaft->mode = TIMING_CRANKSHAFT_MODE_IDLE;
      }
      memset(camshafts->instances, 0, sizeof(camshafts->instances));
      camshafts->synchronized = false;
    }

    crankshaft->pos_phased = *position_values[0];

  } while(0);
}

ITCM_FUNC void timing_cmp_signal_update(timing_ctx_t *ctx, ecu_sensor_cmp_t cmp_instance, const cmp_data_t *data, const cmp_diag_t *diag)
{
  const timing_config_camshaft_t *camshaft_config;
  timing_data_camshafts_t *camshafts;
  timing_data_camshaft_t *camshaft;

  timing_data_crankshaft_t *crankshaft;

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
    camshaft->sensor_diag = *diag;
    if(crankshaft->mode >= TIMING_CRANKSHAFT_MODE_VALID) {
      if(camshaft->sensor_data.validity == CMP_DATA_VALID) {
        if(camshaft_config->use_for_phased_sync) {
          if(crankshaft->mode == TIMING_CRANKSHAFT_MODE_VALID) {
            camshafts->synchronized = true;
            camshafts->sync_at_odd_rev = camshaft->sensor_data.sync_at_odd_rev;
            camshafts->sync_camshaft_instance = cmp_instance;
          } else if(camshafts->sync_at_odd_rev != camshaft->sensor_data.sync_at_odd_rev) {
            if(camshafts->sync_camshaft_instance == cmp_instance) {
              //TODO: error handling
            } else {
              //TODO: error handling
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
          //TODO: error handling
        } else if(camshaft->pos_relative < camshaft_config->pos_min) {
          //TODO: error handling
        }
      } else if(camshaft->sensor_data.validity < CMP_DATA_DETECTED) {
        if(camshafts->synchronized) {
          //TODO: error handling
        }
      }
    } else {
      memset(camshafts->instances, 0, sizeof(camshafts->instances));
      camshafts->synchronized = false;
    }

  } while(0);
}

void timing_loop_main(timing_ctx_t *ctx)
{

}

void timing_loop_slow(timing_ctx_t *ctx)
{

}

ITCM_FUNC void timing_loop_fast(timing_ctx_t *ctx)
{

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
