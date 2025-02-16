/*
 * core_timing_common.c
 *
 *  Created on: Aug 14, 2024
 *      Author: VHEMaster
 */

#include "core_timing_common.h"
#include "common.h"
#include "queuedpulses.h"

#define CORE_OUTPUT_ACTIVE      1
#define CORE_OUTPUT_INACTIVE    0

ITCM_FUNC error_t core_timing_pulse_schedule(ecu_core_ctx_t *ctx, ecu_gpio_output_pin_t output_pin, time_us_t pulse_start, time_us_t pulse_end)
{
  time_us_t now;
  time_us_t time_mask, time_mask_2;
  output_id_t pin;
  error_t err;
  error_t ret;
  time_delta_us_t delta;

  do {
    ret = E_OK;
    time_mask = time_mask_us();
    time_mask_2 = time_mask >> 1;

    err = ecu_config_gpio_output_get_id(output_pin, &pin);
    BREAK_IF_ACTION(ret == E_OK && err != E_OK, ret = err);

    err = E_OK;
    now = time_now_us();
    delta = time_diff(pulse_start, now);
    if(delta < time_mask_2) {
      err = queuedpulses_enqueue_ex(pin, delta, CORE_OUTPUT_INACTIVE);
    } else {
      err = output_set_value(pin, CORE_OUTPUT_ACTIVE);
    }
    if(ret == E_OK && err != E_OK) {
      (void)output_set_value(pin, CORE_OUTPUT_INACTIVE);
      ret = err;
      break;
    }

    err = E_OK;
    delta = time_diff(pulse_end, pulse_start);
    if(delta < time_mask_2) {
      err = queuedpulses_enqueue_ex(pin, delta, CORE_OUTPUT_ACTIVE);
    } else {
      err = output_set_value(pin, CORE_OUTPUT_INACTIVE);
    }
    if(ret == E_OK && err != E_OK) {
      (void)output_set_value(pin, CORE_OUTPUT_INACTIVE);
      ret = err;
      break;
    }
  } while(0);

  return ret;
}
