/*
 * cmp_signal_singlepulse.c
 *
 *  Created on: May 227, 2024
 *      Author: VHEMaster
 */

#include "cmp_signal_singlepulse.h"
#include "config_sensors.h"
#include "time.h"
#include "errors.h"
#include "compiler.h"
#include <string.h>
#include <limits.h>

typedef enum {
  CMP_SIGNAL_SINGLEPULSE_INDEX_0 = 0,
  CMP_SIGNAL_SINGLEPULSE_INDEX_1,
  CMP_SIGNAL_SINGLEPULSE_INDEX_MAX,
}cmp_signal_singlepulse_index_t;

typedef struct {
    float pulse_edge_pos[2];
    float pulse_width;
    bool pulse_edge_first_found;
    bool is_synchronized;
    bool is_rotate_index_odd;
    bool is_ckp_synced;
    bool is_found_current_cycle;
}cmp_signal_singlepulse_runtime_ctx_t;

typedef struct {
    cmp_signal_singlepulse_index_t level_index;
    cmp_signal_singlepulse_index_t level_prev;

    cmp_signal_singlepulse_runtime_ctx_t runtime;
}cmp_signal_singlepulse_ctx_t;

static cmp_signal_singlepulse_ctx_t cmp_signal_singlepulse_ctx[CMP_INSTANCE_MAX];

error_t cmp_signal_singlepulse_init(cmp_ctx_t *ctx, cmp_instance_t instance_index, void **usrdata)
{
  error_t err = E_OK;
  cmp_signal_singlepulse_ctx_t **signal_ctx = (cmp_signal_singlepulse_ctx_t **)usrdata;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    *signal_ctx = &cmp_signal_singlepulse_ctx[instance_index];

    cmp_signal_singlepulse_ctx[instance_index].level_prev = UCHAR_MAX;

  } while(0);

  return err;
}

ITCM_FUNC void cmp_signal_singlepulse_signal(cmp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata)
{
  cmp_signal_singlepulse_ctx_t *signal_ctx = (cmp_signal_singlepulse_ctx_t *)usrdata;
  error_t err;
  uint32_t prim;
  cmp_data_t data;
  bool desync_needed = false;
  uint8_t index, index_prev;
  ecu_gpio_input_level_t level_expected = ECU_IN_LEVEL_UNDEFINED;
  ckp_data_t ckp_data;
  const cmp_config_signal_ref_type_singlepulse_t *cfg_ctx;
  float pulse_width;

  prim = EnterCritical();
  data = ctx->data;
  ExitCritical(prim);

  if(ctx == NULL || signal_ctx == NULL) {
    return;
  }

  do {
    index_prev = signal_ctx->level_prev;
    switch(level) {
      case ECU_IN_LEVEL_LOW:
        index = CMP_SIGNAL_SINGLEPULSE_INDEX_0;
        break;
      case ECU_IN_LEVEL_HIGH:
        index = CMP_SIGNAL_SINGLEPULSE_INDEX_1;
        break;
      default:
        index = signal_ctx->level_index;
        break;
    }

    if(index == index_prev) {
      ctx->diag.bits.signal_sequence = true;
      desync_needed = true;
      break;
    }

    cfg_ctx = &ctx->config.signal_ref_types_config.singlepulse;

    if(!signal_ctx->runtime.pulse_edge_first_found) {
      if(cfg_ctx->polarity == CMP_CONFIG_SIGNAL_POLARITY_ACTIVE_HIGH) {
        level_expected = ECU_IN_LEVEL_HIGH;
      } else if(cfg_ctx->polarity == CMP_CONFIG_SIGNAL_POLARITY_ACTIVE_LOW) {
        level_expected = ECU_IN_LEVEL_LOW;
      }
    } else {
      if(cfg_ctx->polarity == CMP_CONFIG_SIGNAL_POLARITY_ACTIVE_HIGH) {
        level_expected = ECU_IN_LEVEL_LOW;
      } else if(cfg_ctx->polarity == CMP_CONFIG_SIGNAL_POLARITY_ACTIVE_LOW) {
        level_expected = ECU_IN_LEVEL_HIGH;
      }
    }

    if(level == level_expected || level == ECU_IN_LEVEL_UNDEFINED) {
      err = ctx->init.ckp_update_req_cb(ctx->init.ckp_update_usrdata, NULL, &ckp_data);
      if(err != E_OK) {
        ctx->diag.bits.ckp_error = true;
        desync_needed = true;
      }

      if(ckp_data.validity >= CKP_DATA_VALID) {
        if(ckp_data.current_position >= cfg_ctx->pulse_edge_pos_min && ckp_data.current_position <= cfg_ctx->pulse_edge_pos_max) {
          if(!signal_ctx->runtime.pulse_edge_first_found) {
            data.validity = CMP_DATA_DETECTED;
            signal_ctx->runtime.pulse_edge_pos[0] = ckp_data.current_position;
            signal_ctx->runtime.pulse_edge_first_found = true;
          } else {
            signal_ctx->runtime.pulse_edge_first_found = false;
            data.validity = CMP_DATA_SYNCHRONIZED;
            signal_ctx->runtime.pulse_edge_pos[1] = ckp_data.current_position;
            signal_ctx->runtime.is_ckp_synced = true;
          }
        } else {
          if(signal_ctx->runtime.is_ckp_synced) {
            ctx->diag.bits.wrong_signal = true;
            desync_needed = true;
          }
          signal_ctx->runtime.pulse_edge_first_found = false;
        }

        if(data.validity >= CMP_DATA_SYNCHRONIZED) {
          pulse_width = signal_ctx->runtime.pulse_edge_pos[1] - signal_ctx->runtime.pulse_edge_pos[0];
          if(pulse_width >= cfg_ctx->pulse_width_min && pulse_width <= cfg_ctx->pulse_width_max) {
            data.validity = CMP_DATA_VALID;
            signal_ctx->runtime.is_found_current_cycle = true;
          } else {
            ctx->diag.bits.signal_width = true;
            desync_needed = true;
          }
          signal_ctx->runtime.pulse_width = pulse_width;
          data.position = pulse_width * 0.5f + signal_ctx->runtime.pulse_edge_pos[0];
          if(signal_ctx->runtime.is_synchronized) {
            if(signal_ctx->runtime.is_rotate_index_odd != (ckp_data.revolutions_count & 1)) {
              ctx->diag.bits.extra_signal = true;
              desync_needed = true;
            }
          } else {
            signal_ctx->runtime.is_rotate_index_odd = ckp_data.revolutions_count & 1;
            signal_ctx->runtime.is_synchronized = true;
          }
        }
      } else {
        desync_needed = true;
      }
    } else {
      if(signal_ctx->runtime.is_ckp_synced) {
        ctx->diag.bits.bad_pulse = true;
      }
      signal_ctx->runtime.pulse_edge_first_found = false;
      desync_needed = true;
    }

    if(desync_needed) {
      if(ctx->config.desync_on_error) {
        data.validity = MIN(data.validity, CMP_DATA_DETECTED);
      }
    }

    prim = EnterCritical();
    ctx->data = data;
    ExitCritical(prim);

  } while(0);

  signal_ctx->level_prev = index;
  if(++index >= CMP_SIGNAL_SINGLEPULSE_INDEX_MAX) {
    index = 0;
  }
  signal_ctx->level_index = index;

}

void cmp_signal_singlepulse_loop_main(cmp_ctx_t *ctx, void *usrdata)
{
  cmp_signal_singlepulse_ctx_t *signal_ctx = (cmp_signal_singlepulse_ctx_t *)usrdata;

  (void)signal_ctx;

}

void cmp_signal_singlepulse_loop_slow(cmp_ctx_t *ctx, void *usrdata)
{
  cmp_signal_singlepulse_ctx_t *signal_ctx = (cmp_signal_singlepulse_ctx_t *)usrdata;

  (void)signal_ctx;

}

ITCM_FUNC void cmp_signal_singlepulse_loop_fast(cmp_ctx_t *ctx, void *usrdata)
{
  cmp_signal_singlepulse_ctx_t *signal_ctx = (cmp_signal_singlepulse_ctx_t *)usrdata;

  (void)signal_ctx;

}

ITCM_FUNC void cmp_signal_singlepulse_ckp_update(cmp_ctx_t *ctx, void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag)
{
  cmp_signal_singlepulse_ctx_t *signal_ctx = (cmp_signal_singlepulse_ctx_t *)usrdata;
  const cmp_config_signal_ref_type_singlepulse_t *cfg_ctx;
  bool clean_trigger = false;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(signal_ctx == NULL);

    cfg_ctx = &ctx->config.signal_ref_types_config.singlepulse;

    if(data->validity < CKP_DATA_VALID) {
      clean_trigger = true;
    } else {
      if(signal_ctx->runtime.pulse_edge_first_found) {
        if(data->current_position > cfg_ctx->pulse_edge_pos_max || data->current_position < cfg_ctx->pulse_edge_pos_min) {
          signal_ctx->runtime.pulse_edge_first_found = false;
          if(signal_ctx->runtime.is_ckp_synced) {
            ctx->diag.bits.bad_pulse = true;
          }
        }
      } else if(signal_ctx->runtime.is_synchronized) {
        if(!signal_ctx->runtime.is_found_current_cycle) {
          if(signal_ctx->runtime.is_rotate_index_odd == (data->revolutions_count & 1) &&
              data->current_position > cfg_ctx->pulse_edge_pos_max) {
            if(signal_ctx->runtime.is_ckp_synced) {
              ctx->diag.bits.signal_lost = true;
              clean_trigger = true;
            }
          }
        } else {
          if(signal_ctx->runtime.is_rotate_index_odd != (data->revolutions_count & 1)) {
            signal_ctx->runtime.is_found_current_cycle = false;
          }
        }
      } else {
        if(data->revolutions_count >= 2 && data->current_position > cfg_ctx->pulse_edge_pos_max) {
          ctx->diag.bits.no_signal = true;
          clean_trigger = true;
        }
      }

      if(!signal_ctx->runtime.is_ckp_synced) {
        if(data->current.position < data->previous.position) {
          signal_ctx->runtime.is_ckp_synced = true;
        }
      }
    }

    if(clean_trigger) {
      memset(&signal_ctx->runtime, 0, sizeof(signal_ctx->runtime));
      memset(&ctx->data, 0, sizeof(ctx->data));
    }

  } while(0);

}
