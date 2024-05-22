/*
 * time.c
 *
 *  Created on: Mar 20, 2024
 *      Author: VHEMaster
 */

#include <limits.h>
#include "time.h"
#include "compiler.h"

volatile time_us_t *g_time_timebase = NULL;
time_us_t g_time_mask = UINT_MAX;

volatile time_us_t *g_time_tickbase = NULL;
time_us_t g_tick_mask = UINT_MAX;

void time_init_timebase(volatile time_tick_t *timebase, time_us_t mask)
{
  g_time_timebase = timebase;
  g_time_mask = mask;
}

void time_init_tickbase(volatile time_tick_t *tickbase, time_us_t mask)
{
  g_time_tickbase = tickbase;
  g_tick_mask = mask;
}

ITCM_FUNC INLINE time_us_t time_get_current_us(void)
{
  return *g_time_timebase;
}

ITCM_FUNC INLINE time_tick_t time_get_current_tick(void)
{
  return *g_time_tickbase;
}

ITCM_FUNC INLINE time_delta_us_t time_diff(time_us_t a, time_us_t b)
{
  time_us_t retval;

  if(a >= b) {
    retval = a - b;
  } else {
    retval = (g_time_mask - b) + a;
  }

  return retval;
}

ITCM_FUNC INLINE time_delta_tick_t time_tick_diff(time_us_t a, time_us_t b)
{
  time_us_t retval;

  if(a >= b) {
    retval = a - b;
  } else {
    retval = (g_tick_mask - b) + a;
  }

  return retval;
}

ITCM_FUNC void time_msmt_start(time_msmnt_item_t *item)
{
  time_tick_t tick = time_get_current_tick();

  item->period = time_tick_diff(tick, item->last_tick) * TIME_US_IN_TICK;
  item->last_tick = tick;
}

ITCM_FUNC void time_msmt_stop(time_msmnt_item_t *item)
{
  time_tick_t tick = time_get_current_tick();
  time_float_delta_us_t load_tick = time_tick_diff(tick, item->last_tick) * TIME_US_IN_TICK;

  if(item->load_max < load_tick || item->load_max == 0) {
    item->load_max = load_tick;
  }
  if(item->load_min > load_tick || item->load_min == 0) {
    item->load_min = load_tick;
  }
  item->load_last = load_tick;
}
