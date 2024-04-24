/*
 * pid.h
 *
 *  Created on: 9 мар. 2022 г.
 *      Author: VHEMaster
 */

#ifndef PID_H_
#define PID_H_

#include "time.h"

typedef struct {
    float Kp;
    float Ki;
    float Kd;
}math_pid_koffs_t;

typedef struct {
    math_pid_koffs_t K;
    float ClampFrom;
    float ClampTo;
    float P;
    float I;
    float D;
    float Current;
    float Target;
    float Output;
    float Error;
    time_us_t LastTime;
}math_pid_ctx_t;

void math_pid_reset(math_pid_ctx_t *pid, time_us_t time);
void math_pid_set_target(math_pid_ctx_t *pid, float target);
void math_pid_set_koffs(math_pid_ctx_t *pid, const math_pid_koffs_t *K);
void math_pid_set_clamp(math_pid_ctx_t *pid, float from, float to);
void math_pid_init(math_pid_ctx_t *pid);

float math_pid_update(math_pid_ctx_t *pid, float input, time_us_t time);

#endif /* PID_H_ */
