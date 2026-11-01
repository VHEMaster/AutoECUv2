/*
 * config_loop.c
 *
 *  Created on: Apr 24, 2024
 *      Author: VHEMaster
 */

#include "config_loop.h"
#include "compiler.h"
#include "time.h"
#include "bool.h"

#define ECU_LOOP_MAX_CALLBACKS    (32)

typedef struct {
    ecu_loop_cb_t func;
    void *usrdata;
    time_delta_us_t period;
    time_us_t last;
}ecu_loop_callback_t;

typedef struct {
    uint32_t count;
    ecu_loop_callback_t items[ECU_LOOP_MAX_CALLBACKS];
}ecu_loop_callbacks_t;

typedef struct {
    ecu_loop_callbacks_t main;
    ecu_loop_callbacks_t slow;
    ecu_loop_callbacks_t fast;
}ecu_loop_ctx_t;

static ecu_loop_ctx_t ecu_loop_ctx = {0};

ITCM_FUNC static void ecu_loop_execute(ecu_loop_callbacks_t *callbacks)
{
  time_us_t now;
  ecu_loop_callback_t *item;

  for(int i = 0; i < callbacks->count; i++) {
    item = &callbacks->items[i];

    if(item->func) {
      now = time_now_us();
      if(item->period == 0 || time_diff(now, item->last) >= item->period) {
        item->last = now;
        item->func(item->usrdata);
      }
    }
  }
}

static error_t ecu_loop_register(ecu_loop_callbacks_t *callbacks, ecu_loop_cb_t func, void *usrdata, time_delta_us_t period)
{
  error_t err = E_OK;
  uint32_t new_index;
  bool found = false;
  time_us_t now = time_now_us();

  do {
    BREAK_IF_ACTION(callbacks->count >= ECU_LOOP_MAX_CALLBACKS, err = E_OVERFLOW);

    for(int i = 0; i < callbacks->count; i++) {
      if(callbacks->items[i].func == func && callbacks->items[i].usrdata == usrdata) {
        callbacks->items[i].period = period;
        found = true;
        break;
      }
    }
    BREAK_IF(found);

    new_index = callbacks->count++;

    callbacks->items[new_index].period = period;
    callbacks->items[new_index].last = now;
    callbacks->items[new_index].usrdata = usrdata;
    callbacks->items[new_index].func = func;

  } while(0);

  return err;
}

void ecu_loop_main(void)
{
  ecu_loop_callbacks_t *callbacks = &ecu_loop_ctx.main;

  ecu_loop_execute(callbacks);
}

void ecu_loop_slow(void)
{
  ecu_loop_callbacks_t *callbacks = &ecu_loop_ctx.slow;

  ecu_loop_execute(callbacks);
}

ITCM_FUNC void ecu_loop_fast(void)
{
  ecu_loop_callbacks_t *callbacks = &ecu_loop_ctx.fast;

  ecu_loop_execute(callbacks);
}

error_t ecu_loop_register_main(ecu_loop_cb_t func, void *usrdata, time_delta_us_t period)
{
  error_t err = E_OK;
  ecu_loop_callbacks_t *callbacks = &ecu_loop_ctx.main;

  err = ecu_loop_register(callbacks, func, usrdata, period);

  return err;
}

error_t ecu_loop_register_slow(ecu_loop_cb_t func, void *usrdata, time_delta_us_t period)
{
  error_t err = E_OK;
  ecu_loop_callbacks_t *callbacks = &ecu_loop_ctx.slow;

  err = ecu_loop_register(callbacks, func, usrdata, period);

  return err;
}

error_t ecu_loop_register_fast(ecu_loop_cb_t func, void *usrdata, time_delta_us_t period)
{
  error_t err = E_OK;
  ecu_loop_callbacks_t *callbacks = &ecu_loop_ctx.fast;

  err = ecu_loop_register(callbacks, func, usrdata, period);

  return err;
}
