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

volatile time_tick_t *g_time_tickbase = NULL;
time_tick_t g_tick_mask = UINT_MAX;

volatile float g_time_mean_lpf = 0.01f;

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

ITCM_FUNC OPTIMIZE_FAST INLINE time_us_t time_now_us(void)
{
  return *g_time_timebase;
}

ITCM_FUNC OPTIMIZE_FAST INLINE time_us_t time_mask_us(void)
{
  return g_time_mask;
}

ITCM_FUNC OPTIMIZE_FAST INLINE time_tick_t time_now_tick(void)
{
  return *g_time_tickbase;
}

ITCM_FUNC OPTIMIZE_FAST INLINE time_tick_t time_tick_mask(void)
{
  return g_tick_mask;
}

ITCM_FUNC OPTIMIZE_FAST INLINE time_delta_us_t time_diff(time_us_t a, time_us_t b)
{
  time_us_t retval;

  if(a >= b) {
    retval = a - b;
  } else {
    retval = (g_time_mask - b) + a;
  }

  return retval;
}

ITCM_FUNC OPTIMIZE_FAST INLINE time_delta_tick_t time_tick_diff(time_tick_t a, time_tick_t b)
{
  time_us_t retval;

  if(a >= b) {
    retval = a - b;
  } else {
    retval = (g_tick_mask - b) + a;
  }

  return retval;
}

ITCM_FUNC OPTIMIZE_FAST INLINE void time_msmt_start(time_msmnt_item_t *item)
{
  const time_tick_t tick = time_now_tick();

  item->period = time_tick_diff(tick, item->last_tick) * TIME_US_IN_TICK;

  item->reduce_us = 0;
  item->last_tick = time_now_tick();
}

ITCM_FUNC OPTIMIZE_FAST INLINE void time_msmt_stop(time_msmnt_item_t *item)
{
  const time_tick_t tick = time_now_tick();
  const time_float_delta_us_t load_tick_nr = time_tick_diff(tick, item->last_tick) * TIME_US_IN_TICK;
  const time_float_delta_us_t load_tick = load_tick_nr - item->reduce_us;
  const float mean_blend = g_time_mean_lpf;

  if(item->load_max < load_tick || item->load_max == 0) {
    item->load_max = load_tick;
  }
  if(item->load_min > load_tick || item->load_min == 0) {
    item->load_min = load_tick;
  }
  if(item->load_mean == 0.0f) {
    item->load_mean = load_tick;
  } else{
    item->load_mean = BLEND(item->load_mean, load_tick, mean_blend);
  }

  item->load_last_nr = load_tick_nr;
  item->load_last = load_tick;
}

typedef struct {
    uint32_t bitmap;
    time_msmnt_item_t *items[32];
}time_msmnt_nested_protection_t;

static time_msmnt_nested_protection_t time_msmnt_nested_protection_ctx = {0};

ITCM_FUNC OPTIMIZE_FAST void time_msmt_start_nested_protected(time_msmnt_item_np_t *item)
{
  uint32_t prim = EnterCritical();
  uint32_t bitmap = time_msmnt_nested_protection_ctx.bitmap;
  uint32_t pos = 32 - __CLZ(bitmap);
  uint32_t mask = 1 << (pos);

  bitmap |= mask;
  item->bitmap_mask = mask;
  item->array_pos = pos;
  time_msmnt_nested_protection_ctx.bitmap = bitmap;
  time_msmnt_nested_protection_ctx.items[pos] = &item->generic;

  time_msmt_start(&item->generic);
  ExitCritical(prim);
}

ITCM_FUNC OPTIMIZE_FAST void time_msmt_stop_nested_protected(time_msmnt_item_np_t *item)
{
  uint32_t prim = EnterCritical();
  time_msmt_stop(&item->generic);
  const time_float_delta_us_t reduce_us = item->generic.reduce_us;
  const float mean_blend = g_time_mean_lpf;

  if(item->reduce_us_mean == 0.0f) {
    item->reduce_us_mean = reduce_us;
  } else{
    item->reduce_us_mean = BLEND(item->reduce_us_mean, reduce_us, mean_blend);
  }

  uint32_t bitmap = time_msmnt_nested_protection_ctx.bitmap;
  uint32_t mask = item->bitmap_mask;
  uint32_t pos = item->array_pos;
  time_float_delta_us_t load_last;
  time_float_delta_us_t load_last_nr;

  bitmap &= ~mask;
  time_msmnt_nested_protection_ctx.bitmap = bitmap;
  time_msmnt_nested_protection_ctx.items[pos] = NULL;

  load_last = item->generic.load_last;
  load_last_nr = item->generic.load_last_nr;
  for(uint32_t i = 0; i < pos; i++) {
    time_msmnt_nested_protection_ctx.items[i]->reduce_us += load_last;
  }

  if(item->load_mean_reduced == 0.0f) {
    item->load_mean_reduced = load_last;
  } else{
    item->load_mean_reduced = BLEND(item->load_mean_reduced, load_last, mean_blend);
  }

  if(item->load_mean_nreduced == 0.0f) {
    item->load_mean_nreduced = load_last_nr;
  } else{
    item->load_mean_nreduced = BLEND(item->load_mean_nreduced, load_last_nr, mean_blend);
  }

  ExitCritical(prim);
}
