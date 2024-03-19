/*
 * time.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef COMMON_INC_TIME_H_
#define COMMON_INC_TIME_H_

#include <stdint.h>

#define TIME_US_IN_MS   (1000u)
#define TIME_MS_IN_S    (1000u)
#define TIME_US_IN_S    (TIME_US_IN_MS * TIME_MS_IN_S)

#define TIME_MS_IN_US   (1.0f / TIME_US_IN_MS)
#define TIME_S_IN_MS    (1.0f / TIME_MS_IN_S)
#define TIME_S_IN_US    (1.0f / TIME_US_IN_S)

typedef uint32_t time_us_t;
typedef uint32_t time_ms_t;
typedef uint32_t time_s_t;
typedef float time_float_s_t;

typedef uint32_t time_delta_us_t;
typedef uint32_t time_delta_ms_t;
typedef uint32_t time_delta_s_t;
typedef float time_float_delta_s_t;

#endif /* COMMON_INC_TIME_H_ */
