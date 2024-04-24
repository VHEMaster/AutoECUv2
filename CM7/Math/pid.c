/*
 * pid.c
 *
 *  Created on: 10 мар. 2022 г.
 *      Author: VHEMaster
 */

#include "compiler.h"
#include "pid.h"
#include "time.h"
#include <string.h>
#include <float.h>

INLINE void math_pid_reset(math_pid_ctx_t *pid, time_us_t time)
{
  pid->Current = 0;
  pid->Target = 0;
  pid->Error = 0;
  pid->P = 0;
  pid->I = 0;
  pid->D = 0;

  pid->LastTime = time;
}

INLINE void math_pid_set_target(math_pid_ctx_t *pid, float target)
{
  pid->Target = target;
}

INLINE void math_pid_set_koffs(math_pid_ctx_t *pid, const math_pid_koffs_t *K)
{
  pid->K = *K;
}

INLINE void math_pid_set_clamp(math_pid_ctx_t *pid, float from, float to)
{
  pid->ClampFrom = from;
  pid->ClampTo = to;
}

INLINE void math_pid_init(math_pid_ctx_t *pid)
{
  memset(pid, 0, sizeof(math_pid_ctx_t));
  pid->ClampFrom = FLT_MIN;
  pid->ClampTo = FLT_MAX;
}

INLINE float math_pid_update(math_pid_ctx_t *pid, float input, time_us_t time)
{
  float error = pid->Target - input;
  float output;
  float P,I,D;
  uint32_t dt = time_diff(time, pid->LastTime);
  float dtf = dt * 0.000001f; //  / 1000000.0f
  pid->LastTime = time;

  I = pid->I;

  pid->Error = error;
  P = error * pid->K.Kp;
  I += error * dtf * pid->K.Ki;
  D = -(input - pid->Current) / dtf * pid->K.Kd;

  I = CLAMP(I, pid->ClampFrom, pid->ClampTo);

  pid->P = P;
  pid->I = I;
  pid->D = D;

  pid->Current = input;

  output = P + I + D;

  output = CLAMP(output, pid->ClampFrom, pid->ClampTo);

  pid->Output = output;

  return output;
}
