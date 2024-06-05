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

#define CKP_SIGNAL_REGULAR_60_2_DELTA_TIMEOUT_MAX_US    (60 * TIME_US_IN_MS)
#define CKP_SIGNAL_REGULAR_60_2_DELTA_TIMEOUT_MULT      (12)
#define CKP_SIGNAL_REGULAR_60_2_ZERO_POINT              -116.0f

#define CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX        116

#define CKP_SIGNAL_REGULAR_60_2_ABS_INDEX_MAX           12

#define CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT             4

#define CKP_SIGNAL_REGULAR_60_2_USPD_COUNT              8

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

    time_delta_us_t abs_deltas[CKP_SIGNAL_REGULAR_60_2_ABS_INDEX_MAX];
    time_delta_us_t abs_delta_sum;
    uint8_t abs_delta_count;
    uint8_t abs_delta_index;

    time_delta_us_t sync_delta;

    bool initial_found;
}ckp_signal_regular_60_2_runtime_index_ctx_t;

typedef struct {
    ckp_signal_regular_60_2_runtime_index_ctx_t indexed[CKP_SIGNAL_REGULAR_60_2_INDEX_MAX];

    time_delta_us_t rpm_deltas[CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX];
    time_delta_us_t rpm_delta_sum;
    uint8_t rpm_delta_count;
    uint8_t rpm_delta_index;

    time_us_t rpm_last;

    float uspd_array[CKP_SIGNAL_REGULAR_60_2_USPD_COUNT];
    float uspd_sum;
    uint8_t uspd_count;
    uint8_t uspd_index;

    uint8_t sync_signal_index;
    uint8_t signal_index;
    bool diff_sig_valid;
    bool diff_sig_accept;
    float diff_sig_value;
    float sync_sig_value;
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

ITCM_FUNC void ckp_signal_regular_60_2_signal(ckp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata)
{
  ckp_signal_regular_60_2_ctx_t *signal_ctx = (ckp_signal_regular_60_2_ctx_t *)usrdata;
  time_us_t now = time_get_current_us();
  time_us_t time_last;
  time_delta_us_t delta, delta_last;
  time_delta_us_t rpm_delta;
  ckp_data_t data;
  uint32_t prim;
  uint8_t index, index_prev, sync_index;
  uint8_t delta_index;
  uint8_t abs_delta_index;
  uint8_t rpm_delta_index;
  uint8_t uspd_index;
  uint8_t rpm_delta_count;
  uint8_t abs_delta_count;
  uint8_t uspd_count;
  float delta_mean_prev;
  float period_delta_sum;
  float delta_float;
  float delta_diff;
  float uspd;
  float quotient;
  float pos_adder = 0;
  bool sync_pos_updated = false;
  bool initial_cur_found = false;
  bool initial_cycle = false;
  bool initial_found[2];
  bool desync_needed = false;
  bool data_updated = false;

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
      desync_needed = true;
      break;
    }

    delta_last = signal_ctx->runtime.indexed[index].delta_last;
    time_last = signal_ctx->runtime.indexed[index].time_last;
    BREAK_IF(time_last == 0);

    delta = time_diff(now, time_last);
    rpm_delta = time_diff(now, signal_ctx->runtime.rpm_last);

    delta_mean_prev = signal_ctx->runtime.indexed[index].delta_sum;
    delta_mean_prev /= CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT;

    delta_index = signal_ctx->runtime.indexed[index].delta_index;
    signal_ctx->runtime.indexed[index].delta_sum -= signal_ctx->runtime.indexed[index].deltas[delta_index];
    signal_ctx->runtime.indexed[index].deltas[delta_index] = delta;
    signal_ctx->runtime.indexed[index].delta_sum += delta;

    rpm_delta_index = signal_ctx->runtime.rpm_delta_index;
    signal_ctx->runtime.rpm_delta_sum -= signal_ctx->runtime.rpm_deltas[rpm_delta_index];
    signal_ctx->runtime.rpm_deltas[rpm_delta_index] = rpm_delta;
    signal_ctx->runtime.rpm_delta_sum += rpm_delta;

    abs_delta_index = signal_ctx->runtime.indexed[index].abs_delta_index;
    signal_ctx->runtime.indexed[index].abs_delta_sum -= signal_ctx->runtime.indexed[index].abs_deltas[abs_delta_index];
    signal_ctx->runtime.indexed[index].abs_deltas[abs_delta_index] = rpm_delta;
    signal_ctx->runtime.indexed[index].abs_delta_sum += rpm_delta;

    if(++delta_index >= CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT) {
      delta_index = 0;
    }

    if(++rpm_delta_index >= CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX) {
      rpm_delta_index = 0;
    }

    if(++abs_delta_index >= CKP_SIGNAL_REGULAR_60_2_ABS_INDEX_MAX) {
      abs_delta_index = 0;
    }

    signal_ctx->runtime.indexed[index].delta_index = delta_index;
    signal_ctx->runtime.rpm_delta_index = rpm_delta_index;
    signal_ctx->runtime.indexed[index].abs_delta_index = abs_delta_index;

    signal_ctx->runtime.indexed[index].delta_last = delta;
    BREAK_IF(delta_last == 0);

    rpm_delta_count = signal_ctx->runtime.rpm_delta_count;
    if(rpm_delta_count < CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX) {
      rpm_delta_count++;
      signal_ctx->runtime.rpm_delta_count = rpm_delta_count;
    }

    abs_delta_count = signal_ctx->runtime.indexed[index].abs_delta_count;
    if(abs_delta_count < CKP_SIGNAL_REGULAR_60_2_ABS_INDEX_MAX) {
      abs_delta_count++;
      signal_ctx->runtime.indexed[index].abs_delta_count = abs_delta_count;
    }

    if(signal_ctx->runtime.indexed[index].delta_count < CKP_SIGNAL_REGULAR_60_2_DELTA_COUNT) {
      signal_ctx->runtime.indexed[index].delta_count++;
      break;
    }

    period_delta_sum = signal_ctx->runtime.rpm_delta_sum;
    if(signal_ctx->runtime.rpm_delta_count > 0 && signal_ctx->runtime.rpm_delta_count < CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX) {
      period_delta_sum *= (float)CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX / (float)rpm_delta_count;
    }
    data.period = period_delta_sum;
    data.us_per_degree_revolution = data.period * 0.00277777778f;
    data.rpm = 60000000.0f / data.period;

    data.validity = MAX(data.validity, CKP_DATA_DETECTED);
    delta_diff = (float)delta - (float)delta_last;
    quotient = delta_diff / delta_mean_prev;

    initial_found[0] = signal_ctx->runtime.indexed[index].initial_found;
    initial_found[1] = signal_ctx->runtime.indexed[index_prev].initial_found;

    data.previous = data.current;

    if(data.validity < CKP_DATA_SYNCHRONIZED || signal_ctx->runtime.signal_index > CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX - 2) {
      if(!initial_found[0] && !initial_found[1]) {
        quotient -= 1.0f;
        initial_cycle = true;
      } else if(!initial_found[0] && initial_found[1]) {
        quotient -= 2.0f;
        initial_cycle = true;
      }
    }

    if(initial_cycle) {
      if(quotient > -0.5f && quotient < 0.5f) {
        signal_ctx->runtime.indexed[index].sync_delta = rpm_delta;
        signal_ctx->runtime.indexed[index].initial_found = true;
        initial_found[0] = true;
        initial_cur_found = true;

        if(data.validity >= CKP_DATA_SYNCHRONIZED) {
          sync_index = signal_ctx->runtime.sync_signal_index;
          signal_ctx->runtime.sync_sig_value = (float)signal_ctx->runtime.indexed[sync_index].sync_delta / (float)signal_ctx->runtime.indexed[sync_index ^ 1].sync_delta;

          if(signal_ctx->runtime.sync_signal_index == index) {
            pos_adder = 18.0f - 18.0f / (1.0f + signal_ctx->runtime.sync_sig_value);
          } else {
            pos_adder = 18.0f / (1.0f + signal_ctx->runtime.sync_sig_value);
          }
          data.current.position += pos_adder;
          sync_pos_updated = true;
          data_updated = true;
        }
      }
    }

    if(initial_found[0] && initial_found[1]) {
      signal_ctx->runtime.indexed[index].initial_found = false;
      signal_ctx->runtime.indexed[index_prev].initial_found = false;
      if(data.validity >= CKP_DATA_SYNCHRONIZED) {
        if(signal_ctx->runtime.signal_index != CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX) {
          ctx->diag.bits.signal_missing_pulse = true;
          desync_needed = true;
        }
      }
      if(!desync_needed) {
        data.validity = MAX(data.validity, CKP_DATA_SYNCHRONIZED);
      }
      signal_ctx->runtime.sync_signal_index = index;
      signal_ctx->runtime.signal_index = 0;
      data.current.timestamp = now;
      data.current.position = CKP_SIGNAL_REGULAR_60_2_ZERO_POINT;
      pos_adder = data.current.position - data.previous.position;
      signal_ctx->runtime.indexed[index].abs_delta_count = abs_delta_count = 0;
      sync_pos_updated = true;
      data_updated = true;
    }

    if(!initial_cur_found) {
      initial_cur_found = signal_ctx->runtime.indexed[index].initial_found;
      for(int i = 1; i < CKP_SIGNAL_REGULAR_60_2_INDEX_MAX; i++) {
        if(initial_cur_found != signal_ctx->runtime.indexed[index_prev].initial_found) {
          ctx->diag.bits.signal_missing_second_sync = true;
          desync_needed = true;
        }
      }
    }

    if(data.validity >= CKP_DATA_SYNCHRONIZED) {
      sync_index = signal_ctx->runtime.sync_signal_index;
      if(sync_index == index) {
        if(signal_ctx->runtime.signal_index >= 2 + CKP_SIGNAL_REGULAR_60_2_ABS_INDEX_MAX && signal_ctx->runtime.signal_index < CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX - 4) {
          if(signal_ctx->runtime.indexed[index].abs_delta_count >= CKP_SIGNAL_REGULAR_60_2_ABS_INDEX_MAX &&
              signal_ctx->runtime.indexed[index_prev].abs_delta_count >= CKP_SIGNAL_REGULAR_60_2_ABS_INDEX_MAX) {
            signal_ctx->runtime.diff_sig_value = (float)signal_ctx->runtime.indexed[index].abs_delta_sum / (float)signal_ctx->runtime.indexed[index_prev].abs_delta_sum;
            signal_ctx->runtime.diff_sig_valid = true;
          }
        }
      }

      if(!signal_ctx->runtime.diff_sig_valid) {
        signal_ctx->runtime.diff_sig_accept = false;
      }

      if(!sync_pos_updated) {
        if(signal_ctx->runtime.diff_sig_accept && signal_ctx->runtime.signal_index > 2) {
          if(signal_ctx->runtime.sync_signal_index == index) {
            pos_adder = 6.0f - 6.0f / (1.0f + signal_ctx->runtime.diff_sig_value);
          } else {
            pos_adder = 6.0f / (1.0f + signal_ctx->runtime.diff_sig_value);
          }
          sync_pos_updated = true;
          data_updated = true;
        } else if(signal_ctx->runtime.sync_signal_index == index) {
          pos_adder += 6.0f;
          sync_pos_updated = true;
          data_updated = true;

          if(signal_ctx->runtime.diff_sig_valid) {
            signal_ctx->runtime.diff_sig_accept = true;
          }
        } else {
          signal_ctx->runtime.diff_sig_accept = false;
        }
        data.current.position += pos_adder;
      }
    } else {
      sync_pos_updated = false;
    }

    if(sync_pos_updated) {
      data.current.timestamp = now;
      if(data.previous.valid) {
        delta_float = time_diff(now, data.previous.timestamp);

        uspd = delta_float / pos_adder;
        uspd_index = signal_ctx->runtime.uspd_index;
        uspd_count = signal_ctx->runtime.uspd_count;
        signal_ctx->runtime.uspd_sum -= signal_ctx->runtime.uspd_array[uspd_index];
        signal_ctx->runtime.uspd_array[uspd_index] = uspd;
        signal_ctx->runtime.uspd_sum += uspd;

        if(++uspd_index >= CKP_SIGNAL_REGULAR_60_2_USPD_COUNT) {
          uspd_index = 0;
        }

        if(uspd_count < CKP_SIGNAL_REGULAR_60_2_USPD_COUNT) {
          uspd_count++;
        }

        data.us_per_degree_pulsed = signal_ctx->runtime.uspd_sum / uspd_count;
        signal_ctx->runtime.uspd_index = uspd_index;
        signal_ctx->runtime.uspd_count = uspd_count;

        data.validity = MAX(data.validity, CKP_DATA_VALID);
      }
      data.current.valid = true;
      data_updated = true;
    }

    if(data.validity >= CKP_DATA_VALID) {
      if(data.current.position >= 180.0f) {
        data.current.position -= 360.0f;
        data.revs_count++;
        data.odd_rev = data.revs_count & 1;
      }
      data.current_position = data.current.position;
    }

    if(++signal_ctx->runtime.signal_index > CKP_SIGNAL_REGULAR_60_2_SIGNAL_INDEX_MAX) {
      signal_ctx->runtime.signal_index = 0;
      ctx->diag.bits.signal_extra_pulse = true;
      desync_needed = true;
    }

    if(desync_needed) {
      if(ctx->config.desync_on_error) {
        data.validity = MIN(data.validity, CKP_DATA_DETECTED);
        data_updated = true;
      }
    }

    if(data_updated || data.validity < CKP_DATA_VALID) {
      prim = EnterCritical();
      ctx->data = data;
      ExitCritical(prim);
    }

  } while(0);

  signal_ctx->runtime.indexed[index].time_last = now;
  signal_ctx->runtime.rpm_last = now;
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
  float expected_delta;

  for(int i = 0; i < CKP_SIGNAL_REGULAR_60_2_INDEX_MAX; i++) {
    time_last = signal_ctx->runtime.indexed[i].time_last;
    if(time_last != 0) {
      now = time_get_current_us();
      delta = time_diff(now, time_last);

      if(signal_ctx->runtime.rpm_delta_count > 16) {
        expected_delta = (float)signal_ctx->runtime.rpm_delta_sum / (float)signal_ctx->runtime.rpm_delta_count;
        expected_delta *= CKP_SIGNAL_REGULAR_60_2_DELTA_TIMEOUT_MULT;
        expected_delta = MIN(expected_delta, CKP_SIGNAL_REGULAR_60_2_DELTA_TIMEOUT_MAX_US);
      } else {
        expected_delta = CKP_SIGNAL_REGULAR_60_2_DELTA_TIMEOUT_MAX_US;
      }

      if(delta >= expected_delta) {
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


