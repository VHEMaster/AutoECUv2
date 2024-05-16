/*
 * time.c
 *
 *  Created on: Mar 20, 2024
 *      Author: VHEMaster
 */

#include <limits.h>
#include "time.h"
#include "compiler.h"

static volatile time_us_t *g_time_timebase = NULL;
static time_us_t g_time_mask = UINT_MAX;

void time_init_timebase(volatile time_us_t *timebase, time_us_t mask)
{
  g_time_timebase = timebase;
  g_time_mask = mask;
}

ITCM_FUNC INLINE time_us_t time_get_current_us(void)
{
  return *g_time_timebase;
}

ITCM_FUNC INLINE time_us_t time_diff(time_us_t a, time_us_t b)
{
  time_us_t retval;

  if(a >= b) {
    retval = a - b;
  } else {
    retval = (g_time_mask - b) + a;
  }

  return retval;
}

