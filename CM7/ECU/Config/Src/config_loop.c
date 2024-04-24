/*
 * config_loop.c
 *
 *  Created on: Apr 24, 2024
 *      Author: VHEMaster
 */

#include "config_loop.h"
#include "compiler.h"
#include "time.h"

#define ECU_LOOP_MAX_CALLBACKS    (32)

typedef struct {
    ecu_config_loop_cb_t func;
    void *usrdata;
    time_delta_us_t period;
    time_us_t last;
}ecu_config_loop_callback_t;

typedef struct {
    uint32_t count;
    ecu_config_loop_callback_t items[ECU_LOOP_MAX_CALLBACKS];
}ecu_config_loop_callbacks_t;

typedef struct {
    ecu_config_loop_callbacks_t main;
    ecu_config_loop_callbacks_t slow;
    ecu_config_loop_callbacks_t fast;
}ecu_config_loop_ctx_t;

static ecu_config_loop_ctx_t ecu_config_loop_ctx = {0};

static void ecu_config_loop_execute(ecu_config_loop_callbacks_t *callbacks)
{
  time_us_t now;
  ecu_config_loop_callback_t *item;

  for(int i = 0; i < callbacks->count; i++) {
    item = &callbacks->items[i];

    if(item->func) {
      now = time_get_current_us();
      if(item->period == 0 || time_diff(now, item->last) >= item->period) {
        item->last = now;
        item->func(item->usrdata);
      }
    }
  }
}

static error_t ecu_config_loop_register(ecu_config_loop_callbacks_t *callbacks, ecu_config_loop_cb_t func, void *usrdata, time_delta_us_t period)
{
  error_t err = E_OK;
  uint32_t new_index;
  time_us_t now = time_get_current_us();

  do {
    BREAK_IF_ACTION(callbacks->count >= ECU_LOOP_MAX_CALLBACKS, err = E_OVERFLOW);

    for(int i = 0; i < callbacks->count; i++) {
      if(callbacks->items[i].func == func) {
        err = E_EXISTS;
        break;
      }
    }
    BREAK_IF(err != E_OK);

    new_index = callbacks->count++;

    callbacks->items[new_index].period = period;
    callbacks->items[new_index].last = now;
    callbacks->items[new_index].usrdata = usrdata;
    callbacks->items[new_index].func = func;

  } while(0);

  return err;
}

void ecu_config_loop_main(void)
{
  ecu_config_loop_callbacks_t *callbacks = &ecu_config_loop_ctx.main;

  ecu_config_loop_execute(callbacks);
}

void ecu_config_loop_slow(void)
{
  ecu_config_loop_callbacks_t *callbacks = &ecu_config_loop_ctx.slow;

  ecu_config_loop_execute(callbacks);
}

void ecu_config_loop_fast(void)
{
  ecu_config_loop_callbacks_t *callbacks = &ecu_config_loop_ctx.fast;

  ecu_config_loop_execute(callbacks);
}

error_t ecu_config_loop_register_main(ecu_config_loop_cb_t func, void *usrdata, time_delta_us_t period)
{
  error_t err = E_OK;
  ecu_config_loop_callbacks_t *callbacks = &ecu_config_loop_ctx.main;

  err = ecu_config_loop_register(callbacks, func, usrdata, period);

  return err;
}

error_t ecu_config_loop_register_slow(ecu_config_loop_cb_t func, void *usrdata, time_delta_us_t period)
{
  error_t err = E_OK;
  ecu_config_loop_callbacks_t *callbacks = &ecu_config_loop_ctx.slow;

  err = ecu_config_loop_register(callbacks, func, usrdata, period);

  return err;
}

error_t ecu_config_loop_register_fast(ecu_config_loop_cb_t func, void *usrdata, time_delta_us_t period)
{
  error_t err = E_OK;
  ecu_config_loop_callbacks_t *callbacks = &ecu_config_loop_ctx.fast;

  err = ecu_config_loop_register(callbacks, func, usrdata, period);

  return err;
}
