/*
 * math_misc.h
 *
 *  Created on: 12 мар. 2022 г.
 *      Author: VHEMaster
 */

#ifndef MATH_MISC_H_
#define MATH_MISC_H_

#include "compiler.h"
#include <float.h>
#include <stddef.h>

OPTIMIZE_FAST
STATIC_INLINE void math_minmax(const float *array, unsigned int size, float *pmin, float *pmax)
{
  float min = FLT_MAX;
  float max = -FLT_MAX;
  float value;

  while(size--) {
    value = *array++;
    if(value < min)
      min = value;
    if(value > max)
      max = value;
  }

  if(pmax)
    *pmax = max;
  if(pmin)
    *pmin = min;

}

OPTIMIZE_FAST
STATIC_INLINE float math_median(const float *array, unsigned int size)
{
  float value = 0;
  float sorted[size];
  unsigned char changed = 0;

  for(int i = 0; i < size; i++) {
    sorted[i] = array[i];
  }

  for (int i = 0; i < size - 1; i++) {
    changed = 0;
    for (int j = 0; j < size - i - 1; j++) {
      if (sorted[j] > sorted[j + 1]) {
        value = sorted[j];
        sorted[j] = sorted[j + 1];
        sorted[j + 1] = value;
        changed = 1;
      }
    }
    if(!changed)
      break;
  }

  value = sorted[size >> 1];

  return value;
}

/* Search ascending float keys at the start of each element. end_index bounds the left index. */
OPTIMIZE_FAST
STATIC_INLINE int math_binary_search_strided(const void *array, size_t stride, int start_index, int end_index,
    float element)
{
  const unsigned char *bytes = array;
  int iterations = 0;
  while(start_index <= end_index && ++iterations < 256) {
    int middle = start_index + ((end_index - start_index) >> 1);
    const unsigned char *entry = bytes + (size_t)middle * stride;
    float left = *(const float *)entry;
    float right = *(const float *)(entry + stride);
    if(left <= element && right > element)
      return middle;
    if(right <= element)
      start_index = middle + 1;
    else if(left >= element)
      end_index = middle - 1;
  }
  return -1;
}

OPTIMIZE_FAST
STATIC_INLINE int math_binary_search(const float *array, int start_index, int end_index, float element)
{
  return math_binary_search_strided(array, sizeof(*array), start_index, end_index, element);
}

OPTIMIZE_FAST
STATIC_INLINE float math_calc_air_density_mgcc(float temperature, float pressure)
{
  const float R = 0.00287052874f;

  temperature += 273.15f;
  return pressure / (R * temperature);
}

OPTIMIZE_FAST
STATIC_INLINE float math_calc_ntc_thermistor(float calibration_temperature, float calibration_resistance, float beta, float resistance)
{
  float value;

  value = 1.0f / beta;
  value *= logf(resistance / calibration_resistance);
  value += 1.0f / (calibration_temperature + 273.15f);
  value = 1.0f / value;
  value -= 273.15f;

  return value;
}

#endif /* MATH_MISC_H_ */
