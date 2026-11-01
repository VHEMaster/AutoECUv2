/*
 * math_fast.h
 *
 *  Created on: 23 апр. 2022 г.
 *      Author: VHEMaster
 */

#ifndef MATH_FAST_H_
#define MATH_FAST_H_

#include "compiler.h"


STATIC_INLINE float fast_sqrt_asm(float number)
{
  float retval;

  __ASM("VSQRT.F32 %0,%1" : "=t"(retval) : "t"(number));

  return retval;
}

STATIC_INLINE float fast_sqrt_accurate(float number)
{
  const float half = 0.5f;

  union {
    float f;
    uint32_t i;
  } conv = {number};

  conv.i = 0x1FBD1DF5 + (conv.i >> 1);
  conv.f = half * (conv.f + number / conv.f);
  conv.f = half * (conv.f + number / conv.f);
  return conv.f;
}

STATIC_INLINE float fast_rsqrt_accurate(float number)
{
  const float x2 = number * 0.5f;
  const float threehalfs = 1.5f;

  union {
    float f;
    uint32_t i;
  } conv = {number};

  conv.i = 0x5F3759DF - (conv.i >> 1);
  conv.f *= threehalfs - x2 * conv.f * conv.f;
  conv.f *= threehalfs - x2 * conv.f * conv.f;
  return conv.f;
}

#define fast_sqrt(x) fast_sqrt_asm(x)
#define fast_rsqrt(x) fast_rsqrt_accurate(x)

#endif /* MATH_FAST_H_ */
