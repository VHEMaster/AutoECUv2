/*
 * cmp_signal_singlepulse.c
 *
 *  Created on: May 227, 2024
 *      Author: VHEMaster
 */

#include "cmp_signal_singlepulse.h"
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
  time_us_t now = time_get_current_us();
  uint32_t prim;
  cmp_data_t data;
  bool desync_needed = false;
  bool data_updated = false;
  uint8_t index, index_prev;



  prim = EnterCritical();
  data = ctx->data;
  ExitCritical(prim);

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






    if(data_updated) {
      prim = EnterCritical();
      ctx->data = data;
      if(desync_needed) {
        if(ctx->config.desync_on_error) {
          ctx->data.synchronized = false;
        }
      }
      ExitCritical(prim);
    }
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
  time_us_t now, time_last;
  time_delta_us_t delta;
  bool clean_trigger = false;




  if(clean_trigger != false) {
    memset(&signal_ctx->runtime, 0, sizeof(signal_ctx->runtime));
    memset(&ctx->data, 0, sizeof(ctx->data));
  }
}

ITCM_FUNC void cmp_signal_singlepulse_loop_fast(cmp_ctx_t *ctx, void *usrdata)
{
  cmp_signal_singlepulse_ctx_t *signal_ctx = (cmp_signal_singlepulse_ctx_t *)usrdata;

  (void)signal_ctx;

}


