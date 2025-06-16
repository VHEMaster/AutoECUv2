/*
 * ckp.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "ckp.h"
#include "ckp_signals.h"
#include "compiler.h"
#include "interpolation.h"
#include <string.h>

static const ckp_signal_ref_cfg_t ckp_signal_ref_cfg[CKP_CONFIG_SIGNAL_REF_TYPE_MAX] = {
    {
        .func_init_cb = ckp_signal_regular_60_2_init,
        .func_signal_cb = ckp_signal_regular_60_2_signal,
        .func_main_cb = ckp_signal_regular_60_2_loop_main,
        .func_slow_cb = ckp_signal_regular_60_2_loop_slow,
        .func_fast_cb = ckp_signal_regular_60_2_loop_fast,
    }, //CKP_CONFIG_SIGNAL_REF_TYPE_REGULAR_60_2
};

error_t ckp_init(ckp_ctx_t *ctx, const ckp_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(ckp_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(ckp_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC static void ckp_gpio_input_cb(ecu_gpio_input_pin_t pin, ecu_gpio_input_level_t level, void *usrdata)
{
  ckp_ctx_t *ctx = (ckp_ctx_t *)usrdata;
  ckp_data_t data;
  ckp_diag_t diag;
  uint32_t prim;

  if(ctx != NULL && ctx->configured != false) {
    time_msmt_start(&ctx->load_signal_cb);
    if(ctx->signal_ref_type_ctx.cfg->func_signal_cb != NULL) {
      ctx->signal_ref_type_ctx.cfg->func_signal_cb(ctx, level, ctx->signal_ref_type_ctx.usrdata);
    }
    time_msmt_stop(&ctx->load_signal_cb);

    if(ctx->data.validity == CKP_DATA_DETECTED || ctx->data.validity >= CKP_DATA_VALID) {
      time_msmt_start(&ctx->load_update_cb);
      if(ctx->init.signal_update_cb != NULL) {
        prim = EnterCritical();
        data = ctx->data;
        diag = ctx->diag;
        ExitCritical(prim);

        ctx->init.signal_update_cb(ctx->init.signal_update_usrdata, &data, &diag);
      }
      time_msmt_stop(&ctx->load_update_cb);
    }
  }
}

error_t ckp_configure(ckp_ctx_t *ctx, const ckp_config_t *config)
{
  error_t err = E_OK;
  bool support = false;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->signal_ref_type >= CKP_CONFIG_SIGNAL_REF_TYPE_MAX, err = E_PARAM);

    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = ckp_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(ckp_config_t));
    }

    if(ctx->config.enabled == true) {
      ctx->signal_ref_type_ctx.cfg = &ckp_signal_ref_cfg[ctx->config.signal_ref_type];
      ctx->signal_ref_type_ctx.usrdata = NULL;

      if(ctx->signal_ref_type_ctx.cfg->func_init_cb != NULL) {
        err = ctx->signal_ref_type_ctx.cfg->func_init_cb(ctx, ctx->init.instance_index, &ctx->signal_ref_type_ctx.usrdata);
        BREAK_IF(err != E_OK);
      }

      err = ecu_config_gpio_input_get_id(ctx->config.input_pin, &ctx->input_id);
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_input_lock(ctx->config.input_pin);
      BREAK_IF(err != E_OK);
      ctx->pin_locked = true;

      err = ecu_config_gpio_input_has_mode_support(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_CAPTURE, &support);
      BREAK_IF(err != E_OK);
      if(support != false) {
        err = ecu_config_gpio_input_set_mode(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_CAPTURE);
        BREAK_IF(err != E_OK);
      } else {
        err = ecu_config_gpio_input_has_mode_support(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_DIGITAL, &support);
        BREAK_IF(err != E_OK);
        if(support != false) {
          err = ecu_config_gpio_input_set_mode(ctx->config.input_pin, ECU_GPIO_INPUT_TYPE_DIGITAL);
          BREAK_IF(err != E_OK);
        } else {
          err = E_NOTSUPPORT;
        }
      }
      BREAK_IF(err != E_OK);

      err = ecu_config_gpio_input_set_capture_edge(ctx->config.input_pin, ECU_IN_CAPTURE_EDGE_BOTH);
      BREAK_IF(err != E_OK);
      err = ecu_config_gpio_input_set_usrdata(ctx->config.input_pin, ctx);
      BREAK_IF(err != E_OK);
      err = ecu_config_gpio_input_register_callback(ctx->config.input_pin, ckp_gpio_input_cb);
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)ckp_reset(ctx);
  }

  return err;
}

error_t ckp_reset(ckp_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    ctx->configured = false;

    if(ctx->pin_locked != false) {
      (void)ecu_config_gpio_input_unlock(ctx->config.input_pin);
      ctx->pin_locked = false;
    }

  } while(0);

  return err;
}

void ckp_loop_main(ckp_ctx_t *ctx)
{
  if(ctx != NULL) {
    if(ctx->configured != false && ctx->started != false) {
      if(ctx->signal_ref_type_ctx.cfg->func_main_cb != NULL) {
        ctx->signal_ref_type_ctx.cfg->func_main_cb(ctx, ctx->signal_ref_type_ctx.usrdata);
      }
    }
  }
}

void ckp_loop_slow(ckp_ctx_t *ctx)
{
  time_us_t now = time_now_us();
  ckp_data_t data;
  ckp_diag_t diag;
  uint32_t prim;

  do {
    BREAK_IF(ctx == NULL);
    if(ctx->configured != false) {
      if(ctx->started != false) {
        if(ctx->signal_ref_type_ctx.cfg->func_slow_cb != NULL) {
          ctx->signal_ref_type_ctx.cfg->func_slow_cb(ctx, ctx->signal_ref_type_ctx.usrdata);
        }

        prim = EnterCritical();
        data = ctx->data;
        diag = ctx->diag;
        ExitCritical(prim);
        if(data.validity <= CKP_DATA_DETECTED) {
          if(ctx->init.signal_update_cb != NULL) {
            ctx->init.signal_update_cb(ctx->init.signal_update_usrdata, &data, &diag);
          }
        }

      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      ctx->startup_time = now;
    }
  } while(0);
}

ITCM_FUNC void ckp_loop_fast(ckp_ctx_t *ctx)
{
  if(ctx != NULL) {
    if(ctx->configured != false && ctx->started != false) {
      if(ctx->signal_ref_type_ctx.cfg->func_fast_cb != NULL) {
        ctx->signal_ref_type_ctx.cfg->func_fast_cb(ctx, ctx->signal_ref_type_ctx.usrdata);
      }
    }
  }
}

OPTIMIZE_FAST
ITCM_FUNC INLINE error_t ckp_calculate_current_position(ckp_ctx_t *ctx, ckp_req_t *req_ctx, ckp_data_t *data)
{
  error_t err = E_OK;
  float pos, pos_prev, mult, time_delta;
  ckp_data_t data_cur;
  time_us_t now;
  uint32_t prim;

  prim = EnterCritical();
  data_cur = ctx->data;
  ExitCritical(prim);

  now = time_now_us();

  pos = data_cur.current.position;

  if(data_cur.validity >= CKP_DATA_VALID) {
    if(req_ctx == NULL || req_ctx->position_valid) {
      if(data_cur.current.timestamp != data_cur.previous.timestamp) {
        time_delta = time_diff(data_cur.current.timestamp, data_cur.previous.timestamp);
        now = time_diff(now, data_cur.previous.timestamp);

        if(data_cur.current.position < data_cur.previous.position) {
          data_cur.current.position += 360.0f;
        }

        pos = data_cur.current.position - data_cur.previous.position;
        mult = pos / time_delta;
        pos = mult * now + data_cur.previous.position;
      }

      while(pos >= 180.0f) {
        pos -= 360.0f;
      }

      if(req_ctx != NULL) {
        pos_prev = req_ctx->position_prev;

        if((pos - pos_prev < 0.0f && pos - pos_prev > -90.0f) || pos - pos_prev > 90.0f) {
          pos = pos_prev;
        }
      }
    } else {
      pos = data_cur.current.position;
    }

    //Check for NaNs
    if(pos != pos) {
      prim = EnterCritical();
      ctx->diag.bits.pos_calc_nan = true;
      data->validity = MIN(data->validity, CKP_DATA_SYNCHRONIZED);
      ExitCritical(prim);
      pos = 0.0f;
    }

    if(req_ctx != NULL) {
      req_ctx->position_prev = pos;
      req_ctx->position_valid = true;
    }
  }

  if(data != NULL) {
    data_cur.current_position = pos;
    *data = data_cur;
  }

  return err;
}

ITCM_FUNC error_t ckp_get_value(ckp_ctx_t *ctx, ckp_data_t *data)
{
  error_t err = E_OK;
  uint32_t prim;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    prim = EnterCritical();
    *data = ctx->data;
    ExitCritical(prim);

  } while(0);

  return err;
}

ITCM_FUNC error_t ckp_get_diag(ckp_ctx_t *ctx, ckp_diag_t *diag)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->configured == false, err = E_NOTRDY);

    *diag = ctx->diag;

  } while(0);

  return err;
}
