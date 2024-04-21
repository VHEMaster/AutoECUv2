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
    float ClampFrom;
    float ClampTo;
    float Kp;
    float Ki;
    float Kd;
    float P;
    float I;
    float D;
    float Current;
    float Target;
    float Output;
    float Error;
    time_us_t LastTime;
}sMathPid;

void math_pid_reset(sMathPid *pid, time_us_t time);
void math_pid_set_target(sMathPid *pid, float target);
void math_pid_set_koffs(sMathPid *pid, float Kp, float Ki, float Kd);
void math_pid_set_clamp(sMathPid *pid, float from, float to);
void math_pid_init(sMathPid *pid);

float math_pid_update(sMathPid *pid, float input, time_us_t time);

#endif /* PID_H_ */
