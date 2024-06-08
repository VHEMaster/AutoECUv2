/*
 * time.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_TIME_INC_TIME_H_
#define DRIVERS_TIME_INC_TIME_H_

#include <stdint.h>

#define TIME_NOW    time_get_current_us

#define TIME_US_IN_MS   (1000u)
#define TIME_MS_IN_S    (1000u)
#define TIME_US_IN_S    (TIME_US_IN_MS * TIME_MS_IN_S)

#define TIME_US_IN_TICK (0.0125f)

typedef uint32_t time_tick_t;
typedef uint32_t time_us_t;
typedef uint32_t time_ms_t;
typedef uint32_t time_s_t;
typedef float time_float_us_t;
typedef float time_float_ms_t;
typedef float time_float_s_t;

typedef uint32_t time_delta_tick_t;
typedef uint32_t time_delta_us_t;
typedef uint32_t time_delta_ms_t;
typedef uint32_t time_delta_s_t;
typedef float time_float_delta_us_t;
typedef float time_float_delta_ms_t;
typedef float time_float_delta_s_t;

typedef struct {
    time_tick_t last_tick;
    time_float_delta_us_t period;

    time_float_delta_us_t load_last;
    time_float_delta_us_t load_min;
    time_float_delta_us_t load_max;
}time_msmnt_item_t;

void time_init_timebase(volatile time_us_t *timebase, time_us_t mask);
time_us_t time_get_current_us(void);
time_delta_us_t time_diff(time_us_t a, time_us_t b);

void time_init_tickbase(volatile time_tick_t *tickbase, time_tick_t mask);
time_tick_t time_get_current_tick(void);
time_delta_tick_t time_tick_diff(time_tick_t a, time_tick_t b);


void time_msmt_start(time_msmnt_item_t *item);
void time_msmt_stop(time_msmnt_item_t *item);

#endif /* DRIVERS_TIME_INC_TIME_H_ */
