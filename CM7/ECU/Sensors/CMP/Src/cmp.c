/*
 * cmp.c
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#include "cmp.h"
#include "cmp_signals.h"
#include "compiler.h"
#include "interpolation.h"
#include <string.h>

static const cmp_signal_ref_cfg_t cmp_signal_ref_cfg[CMP_CONFIG_SIGNAL_REF_TYPE_MAX] = {
    {
        .func_init_cb = cmp_signal_singlepulse_init,
        .func_signal_cb = cmp_signal_singlepulse_signal,
        .func_main_cb = cmp_signal_singlepulse_loop_main,
        .func_slow_cb = cmp_signal_singlepulse_loop_slow,
        .func_fast_cb = cmp_signal_singlepulse_loop_fast,
    }, //CMP_CONFIG_SIGNAL_REF_TYPE_REGULAR_60_2
};

error_t cmp_init(cmp_ctx_t *ctx, const cmp_init_ctx_t *init_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(cmp_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(cmp_init_ctx_t));

    ctx->ready = true;

  } while(0);

  return err;
}

ITCM_FUNC static void cmp_gpio_input_cb(ecu_gpio_input_pin_t pin, ecu_gpio_input_level_t level, void *usrdata)
{
  cmp_ctx_t *ctx = (cmp_ctx_t *)usrdata;
  cmp_data_t data;
  cmp_diag_t diag;
  uint32_t prim;

  if(ctx != NULL && ctx->configured != false) {
    time_msmt_start(&ctx->load_signal_cb);
    if(ctx->signal_ref_type_ctx.cfg->func_signal_cb != NULL) {
      ctx->signal_ref_type_ctx.cfg->func_signal_cb(ctx, level, ctx->signal_ref_type_ctx.usrdata);
    }
    time_msmt_stop(&ctx->load_signal_cb);

    if(data.synchronized) {
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

error_t cmp_configure(cmp_ctx_t *ctx, const cmp_config_t *config)
{
  error_t err = E_OK;
  bool support = false;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->signal_ref_type >= CMP_CONFIG_SIGNAL_REF_TYPE_MAX, err = E_PARAM);

    BREAK_IF_ACTION(ctx->ready == false, err = E_NOTRDY);

    if(ctx->configured != false) {
      err = cmp_reset(ctx);
      BREAK_IF(err != E_OK);
    }

    ctx->configured = false;

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(cmp_config_t));
    }

    if(ctx->config.enabled == true) {
      ctx->signal_ref_type_ctx.cfg = &cmp_signal_ref_cfg[ctx->config.signal_ref_type];
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
      err = ecu_config_gpio_input_register_callback(ctx->config.input_pin, cmp_gpio_input_cb);
      BREAK_IF(err != E_OK);

      ctx->configured = true;
    }

  } while(0);

  if(err != E_OK) {
    (void)cmp_reset(ctx);
  }

  return err;
}

error_t cmp_reset(cmp_ctx_t *ctx)
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

void cmp_loop_main(cmp_ctx_t *ctx)
{
  if(ctx != NULL) {
    if(ctx->configured != false && ctx->started != false) {
      if(ctx->signal_ref_type_ctx.cfg->func_main_cb != NULL) {
        ctx->signal_ref_type_ctx.cfg->func_main_cb(ctx, ctx->signal_ref_type_ctx.usrdata);
      }
    }
  }
}

void cmp_loop_slow(cmp_ctx_t *ctx)
{
  time_us_t now = time_get_current_us();

  do {
    BREAK_IF(ctx == NULL);
    if(ctx->configured != false) {
      if(ctx->started != false) {
        if(ctx->signal_ref_type_ctx.cfg->func_slow_cb != NULL) {
          ctx->signal_ref_type_ctx.cfg->func_slow_cb(ctx, ctx->signal_ref_type_ctx.usrdata);
        }
      } else if(time_diff(now, ctx->startup_time) > ctx->config.boot_time) {
        ctx->started = true;
      }
    } else {
      ctx->startup_time = now;
    }
  } while(0);
}

ITCM_FUNC void cmp_loop_fast(cmp_ctx_t *ctx)
{
  if(ctx != NULL) {
    if(ctx->configured != false && ctx->started != false) {
      if(ctx->signal_ref_type_ctx.cfg->func_fast_cb != NULL) {
        ctx->signal_ref_type_ctx.cfg->func_fast_cb(ctx, ctx->signal_ref_type_ctx.usrdata);
      }
    }
  }
}

ITCM_FUNC error_t cmp_get_diag(cmp_ctx_t *ctx, cmp_diag_t *diag)
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
