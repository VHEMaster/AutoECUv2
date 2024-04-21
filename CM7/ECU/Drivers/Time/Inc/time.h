/*
 * time.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef COMMON_INC_TIME_H_
#define COMMON_INC_TIME_H_

#include <stdint.h>

#define TIME_NOW    time_get_current_us

#define TIME_US_IN_MS   (1000u)
#define TIME_MS_IN_S    (1000u)
#define TIME_US_IN_S    (TIME_US_IN_MS * TIME_MS_IN_S)

typedef uint32_t time_us_t;
typedef uint32_t time_ms_t;
typedef uint32_t time_s_t;
typedef float time_float_s_t;

typedef uint32_t time_delta_us_t;
typedef uint32_t time_delta_ms_t;
typedef uint32_t time_delta_s_t;
typedef float time_float_delta_s_t;

void time_init_timebase(volatile time_us_t *timebase, time_us_t mask);
time_us_t time_get_current_us(void);
time_us_t time_diff(time_us_t a, time_us_t b);

#endif /* COMMON_INC_TIME_H_ */
