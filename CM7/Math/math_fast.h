/*
 * math_fast.h
 *
 *  Created on: 23 апр. 2022 г.
 *      Author: VHEMaster
 */

#ifndef MATH_FAST_H_
#define MATH_FAST_H_

#include "compiler.h"

STATIC_INLINE float fast_sqrt(float number)
{
  union {
    float f;
    uint32_t i;
  } conv = {number};

  conv.i = 0x1FBD1DF5 + (conv.i >> 1);
  return conv.f;
}

STATIC_INLINE float fast_rsqrt(float number)
{
  const float x2 = number * 0.5F;
  const float threehalfs = 1.5F;

  union {
    float f;
    uint32_t i;
  } conv = {number};

  conv.i = 0x5F3759DF - (conv.i >> 1);
  conv.f *= threehalfs - x2 * conv.f * conv.f;
  return conv.f;
}

#endif /* MATH_FAST_H_ */
