/*
 * ckp_signal_regular_60_2.c
 *
 *  Created on: May 20, 2024
 *      Author: VHEMaster
 */

#include "ckp_signal_regular_60_2.h"
#include "errors.h"
#include "compiler.h"

static ckp_signal_regular_60_2_ctx_t ckp_signal_regular_60_2_ctx[CKP_INSTANCE_MAX];

error_t ckp_signal_regular_60_2_init(ckp_ctx_t *ctx, ckp_instance_t instance_index, ckp_signal_regular_60_2_ctx_t **signal_ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    *signal_ctx = &ckp_signal_regular_60_2_ctx[instance_index];
  } while(0);

  return err;
}

void ckp_signal_regular_60_2_signal(ckp_ctx_t *ctx, ckp_instance_t instance_index, ecu_gpio_input_level_t level, ckp_signal_regular_60_2_ctx_t *signal_ctx)
{
  time_us_t now = time_get_current_us();
  time_delta_us_t delta;

}

void ckp_signal_regular_60_2_loop_main(ckp_ctx_t *ctx, ckp_signal_regular_60_2_ctx_t *signal_ctx)
{

}

void ckp_signal_regular_60_2_loop_slow(ckp_ctx_t *ctx, ckp_signal_regular_60_2_ctx_t *signal_ctx)
{

}

ITCM_FUNC void ckp_signal_regular_60_2_loop_fast(ckp_ctx_t *ctx, ckp_signal_regular_60_2_ctx_t *signal_ctx)
{

}

