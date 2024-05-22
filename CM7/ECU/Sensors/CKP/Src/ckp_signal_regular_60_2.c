/*
 * ckp_signal_regular_60_2.c
 *
 *  Created on: May 20, 2024
 *      Author: VHEMaster
 */

#include "ckp_signal_regular_60_2.h"
#include "time.h"
#include "errors.h"
#include "compiler.h"
#include <string.h>
#include <limits.h>

#define CKP_SIGNAL_REGULAR_60_2_DELTA_TIMEOUT     (60 * TIME_US_IN_MS)
#define CKP_SIGNAL_REGULAR_60_2_ZERO_POINT        -116.0f

#define CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX  116

#define CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT                 4

typedef enum {
  CKP_SIGNAL_REGULAR_60_2_INDEX_0 = 0,
  CKP_SIGNAL_REGULAR_60_2_INDEX_1,
  CKP_SIGNAL_REGULAR_60_2_INDEX_MAX,
}ckp_signal_regular_60_2_index_t;

typedef struct {
    time_us_t time_last;
    time_delta_us_t delta_last;

    uint8_t delta_index;
    time_delta_us_t deltas[CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT];
    time_delta_us_t delta_sum;
    uint8_t delta_count;

    bool initial_found;
}ckp_signal_regular_60_2_runtime_index_ctx_t;

typedef struct {
    ckp_signal_regular_60_2_runtime_index_ctx_t indexed[CKP_SIGNAL_REGULAR_60_2_INDEX_MAX];
    uint8_t signal_index;
}ckp_signal_regular_60_2_runtime_ctx_t;

typedef struct {
    ckp_signal_regular_60_2_index_t level_index;
    ckp_signal_regular_60_2_index_t level_prev;

    ckp_signal_regular_60_2_runtime_ctx_t runtime;
}ckp_signal_regular_60_2_ctx_t;

static ckp_signal_regular_60_2_ctx_t ckp_signal_regular_60_2_ctx[CKP_INSTANCE_MAX];

error_t ckp_signal_regular_60_2_init(ckp_ctx_t *ctx, ckp_instance_t instance_index, void **usrdata)
{
  error_t err = E_OK;
  ckp_signal_regular_60_2_ctx_t **signal_ctx = (ckp_signal_regular_60_2_ctx_t **)usrdata;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    *signal_ctx = &ckp_signal_regular_60_2_ctx[instance_index];

    ckp_signal_regular_60_2_ctx[instance_index].level_prev = UCHAR_MAX;
  } while(0);

  return err;
}

void ckp_signal_regular_60_2_signal(ckp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata)
{
  ckp_signal_regular_60_2_ctx_t *signal_ctx = (ckp_signal_regular_60_2_ctx_t *)usrdata;
  time_us_t now = time_get_current_us();
  time_us_t time_last;
  time_delta_us_t delta, delta_last;
  ckp_data_t data;
  uint32_t prim;
  uint8_t index, index_prev;
  uint8_t delta_index;
  float delta_mean_prev;
  float delta_diff;
  float quotient;
  bool sync_pos_updated = false;
  bool initial_cur_found = false;
  bool initial_cycle = false;
  bool initial_found[2];

  prim = EnterCritical();
  data = ctx->data;
  ExitCritical(prim);

  do {
    index_prev = signal_ctx->level_prev;
    switch(level) {
      case ECU_IN_LEVEL_LOW:
        index = CKP_SIGNAL_REGULAR_60_2_INDEX_0;
        break;
      case ECU_IN_LEVEL_HIGH:
        index = CKP_SIGNAL_REGULAR_60_2_INDEX_1;
        break;
      default:
        index = signal_ctx->level_index;
        break;
    }

    if(index == index_prev) {
      ctx->diag.bits.signal_sequence = true;
      break;
    } else {
      ctx->diag.bits.signal_sequence = false;
    }

    delta_last = signal_ctx->runtime.indexed[index].delta_last;
    time_last = signal_ctx->runtime.indexed[index].time_last;
    BREAK_IF(time_last == 0);

    delta = time_diff(now, time_last);

    delta_mean_prev = signal_ctx->runtime.indexed[index].delta_sum;
    delta_mean_prev /= CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT;

    delta_index = signal_ctx->runtime.indexed[index].delta_index;
    signal_ctx->runtime.indexed[index].delta_sum -= signal_ctx->runtime.indexed[index].deltas[delta_index];
    signal_ctx->runtime.indexed[index].deltas[delta_index] = delta;
    signal_ctx->runtime.indexed[index].delta_sum += delta;

    if(++delta_index >= CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT) {
      delta_index = 0;
    }
    signal_ctx->runtime.indexed[index].delta_index = delta_index;

    signal_ctx->runtime.indexed[index].delta_last = delta;
    BREAK_IF(delta_last == 0);

    if(signal_ctx->runtime.indexed[index].delta_count < CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT) {
      signal_ctx->runtime.indexed[index].delta_count++;
      break;
    }

    data.detected = true;
    delta_diff = (float)delta - (float)delta_last;
    quotient = delta_diff / delta_mean_prev;

    initial_found[0] = signal_ctx->runtime.indexed[index].initial_found;
    initial_found[1] = signal_ctx->runtime.indexed[index_prev].initial_found;

    data.previous = data.current;

    if(!initial_found[0] && !initial_found[1]) {
      quotient -= 1.0f;
      initial_cycle = true;
    } else if(!initial_found[0] && initial_found[1]) {
      quotient -= 2.0f;
      initial_cycle = true;
    }

    if(initial_cycle != false) {
      if(quotient > -0.2f && quotient < 0.5f) {
        signal_ctx->runtime.indexed[index].initial_found = true;
        initial_found[0] = true;
        initial_cur_found = true;
      }
    }

    if(initial_found[0] && initial_found[1]) {
      signal_ctx->runtime.indexed[index].initial_found = false;
      signal_ctx->runtime.indexed[index_prev].initial_found = false;
      if(data.synchronized) {
        if(signal_ctx->runtime.signal_index != CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX) {
          ctx->diag.bits.signal_missing_pulse = true;
        }
      }
      sync_pos_updated = true;
      data.synchronized = true;
      data.current.timestamp = now;
      data.current.position = CKP_SIGNAL_REGULAR_60_2_ZERO_POINT;
      signal_ctx->runtime.signal_index = 0;
    }

    if(!initial_cur_found) {
      initial_cur_found = signal_ctx->runtime.indexed[index].initial_found;
      for(int i = 1; i < CKP_SIGNAL_REGULAR_60_2_INDEX_MAX; i++) {
        if(initial_cur_found != signal_ctx->runtime.indexed[index_prev].initial_found) {
          ctx->diag.bits.signal_missing_second_sync = true;
        }
      }
    }

    if(data.synchronized) {
      if(!sync_pos_updated) {

        data.valid = true;
      }
    } else {
      data.valid = false;
    }

    if(++signal_ctx->runtime.signal_index > CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX) {
      signal_ctx->runtime.signal_index = 0;
      ctx->diag.bits.signal_extra_pulse = true;
    }

    prim = EnterCritical();
    ctx->data = data;
    ExitCritical(prim);

  } while(0);

  signal_ctx->runtime.indexed[index].time_last = now;
  signal_ctx->level_prev = index;
  if(++index >= CKP_SIGNAL_REGULAR_60_2_INDEX_MAX) {
    index = 0;
  }
  signal_ctx->level_index = index;

}

void ckp_signal_regular_60_2_loop_main(ckp_ctx_t *ctx, void *usrdata)
{
  ckp_signal_regular_60_2_ctx_t *signal_ctx = (ckp_signal_regular_60_2_ctx_t *)usrdata;

  (void)signal_ctx;

}

void ckp_signal_regular_60_2_loop_slow(ckp_ctx_t *ctx, void *usrdata)
{
  ckp_signal_regular_60_2_ctx_t *signal_ctx = (ckp_signal_regular_60_2_ctx_t *)usrdata;
  time_us_t now, time_last;
  time_delta_us_t delta;
  bool clean_trigger = false;

  for(int i = 0; i < CKP_SIGNAL_REGULAR_60_2_INDEX_MAX; i++) {
    time_last = signal_ctx->runtime.indexed[i].time_last;
    if(time_last != 0) {
      now = time_get_current_us();
      delta = time_diff(now, time_last);
      if(delta >= CKP_SIGNAL_REGULAR_60_2_DELTA_TIMEOUT) {
        clean_trigger = true;
      }
    }
  }

  if(clean_trigger != false) {
    memset(&signal_ctx->runtime, 0, sizeof(signal_ctx->runtime));
    memset(&ctx->data, 0, sizeof(ctx->data));
  }
}

ITCM_FUNC void ckp_signal_regular_60_2_loop_fast(ckp_ctx_t *ctx, void *usrdata)
{
  ckp_signal_regular_60_2_ctx_t *signal_ctx = (ckp_signal_regular_60_2_ctx_t *)usrdata;

  (void)signal_ctx;

}

