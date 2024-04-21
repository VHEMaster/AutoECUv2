/*
 * interpolation.c
 *
 *  Created on: 10 мар. 2022 г.
 *      Author: VHEMaster
 */

#include "interpolation_extended.h"
#include "math_misc.h"
#include "compiler.h"
#include <limits.h>

INLINE sMathInterpolateInput math_interpolate_input_u8(float value, const uint8_t *table, uint32_t size, const sMathInterpolationTransform *transform)
{
  float table_float[size];

  for(int i = 0; i < size; i++) {
    table_float[i] = table[i] * transform->gain + transform->offset;
  }

  return math_interpolate_input(value, table_float, size);
}

INLINE sMathInterpolateInput math_interpolate_input_u16(float value, const uint16_t *table, uint32_t size, const sMathInterpolationTransform *transform)
{
  float table_float[size];

  for(int i = 0; i < size; i++) {
    table_float[i] = table[i] * transform->gain + transform->offset;
  }

  return math_interpolate_input(value, table_float, size);
}

INLINE sMathInterpolateInput math_interpolate_input_u32(float value, const uint32_t *table, uint32_t size, const sMathInterpolationTransform *transform)
{
  float table_float[size];

  for(int i = 0; i < size; i++) {
    table_float[i] = table[i] * transform->gain + transform->offset;
  }

  return math_interpolate_input(value, table_float, size);
}

INLINE sMathInterpolateInput math_interpolate_input_s8(float value, const int8_t *table, uint32_t size, const sMathInterpolationTransform *transform)
{
  float table_float[size];

  for(int i = 0; i < size; i++) {
    table_float[i] = table[i] * transform->gain + transform->offset;
  }

  return math_interpolate_input(value, table_float, size);
}

INLINE sMathInterpolateInput math_interpolate_input_s16(float value, const int16_t *table, uint32_t size, const sMathInterpolationTransform *transform)
{
  float table_float[size];

  for(int i = 0; i < size; i++) {
    table_float[i] = table[i] * transform->gain + transform->offset;
  }

  return math_interpolate_input(value, table_float, size);
}

INLINE sMathInterpolateInput math_interpolate_input_s32(float value, const int32_t *table, uint32_t size, const sMathInterpolationTransform *transform)
{
  float table_float[size];

  for(int i = 0; i < size; i++) {
    table_float[i] = table[i] * transform->gain + transform->offset;
  }

  return math_interpolate_input(value, table_float, size);
}

INLINE float math_interpolate_1d_u8(sMathInterpolateInput input, const uint8_t *table, const sMathInterpolationTransform *transform)
{
  float result;
  float output[2];

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  result = (output[1] - output[0]) * input.mult + output[0];

  return result;
}

INLINE float math_interpolate_1d_u16(sMathInterpolateInput input, const uint16_t *table, const sMathInterpolationTransform *transform)
{
  float result;
  float output[2];

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  result = (output[1] - output[0]) * input.mult + output[0];

  return result;
}

INLINE float math_interpolate_1d_u32(sMathInterpolateInput input, const uint32_t *table, const sMathInterpolationTransform *transform)
{
  float result;
  float output[2];

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  result = (output[1] - output[0]) * input.mult + output[0];

  return result;
}

INLINE float math_interpolate_1d_s8(sMathInterpolateInput input, const int8_t *table, const sMathInterpolationTransform *transform)
{
  float result;
  float output[2];

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  result = (output[1] - output[0]) * input.mult + output[0];

  return result;
}

INLINE float math_interpolate_1d_s16(sMathInterpolateInput input, const int16_t *table, const sMathInterpolationTransform *transform)
{
  float result;
  float output[2];

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  result = (output[1] - output[0]) * input.mult + output[0];

  return result;
}

INLINE float math_interpolate_1d_s32(sMathInterpolateInput input, const int32_t *table, const sMathInterpolationTransform *transform)
{
  float result;
  float output[2];

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  result = (output[1] - output[0]) * input.mult + output[0];

  return result;
}

INLINE float math_interpolate_2d_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint8_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  float output_1d[2];
  float input_2d[2][2];

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  result = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];

  return result;
}


INLINE float math_interpolate_2d_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint16_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  float output_1d[2];
  float input_2d[2][2];

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  result = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];

  return result;
}


INLINE float math_interpolate_2d_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint32_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  float output_1d[2];
  float input_2d[2][2];

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  result = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];

  return result;
}

INLINE float math_interpolate_2d_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int8_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  float output_1d[2];
  float input_2d[2][2];

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  result = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];

  return result;
}


INLINE float math_interpolate_2d_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int16_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  float output_1d[2];
  float input_2d[2][2];

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  result = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];

  return result;
}


INLINE float math_interpolate_2d_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int32_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  float output_1d[2];
  float input_2d[2][2];

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  result = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];

  return result;
}

INLINE float math_interpolate_2d_limit_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint8_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  input_x.mult = CLAMP(input_x.mult, 0.0f, 1.0f);
  input_y.mult = CLAMP(input_y.mult, 0.0f, 1.0f);

  return math_interpolate_2d_u8(input_x, input_y, x_size, table, transform);
}

INLINE float math_interpolate_2d_limit_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint16_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  input_x.mult = CLAMP(input_x.mult, 0.0f, 1.0f);
  input_y.mult = CLAMP(input_y.mult, 0.0f, 1.0f);

  return math_interpolate_2d_u16(input_x, input_y, x_size, table, transform);
}

INLINE float math_interpolate_2d_limit_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint32_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  input_x.mult = CLAMP(input_x.mult, 0.0f, 1.0f);
  input_y.mult = CLAMP(input_y.mult, 0.0f, 1.0f);

  return math_interpolate_2d_u32(input_x, input_y, x_size, table, transform);
}

INLINE float math_interpolate_2d_limit_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int8_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  input_x.mult = CLAMP(input_x.mult, 0.0f, 1.0f);
  input_y.mult = CLAMP(input_y.mult, 0.0f, 1.0f);

  return math_interpolate_2d_s8(input_x, input_y, x_size, table, transform);
}

INLINE float math_interpolate_2d_limit_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int16_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  input_x.mult = CLAMP(input_x.mult, 0.0f, 1.0f);
  input_y.mult = CLAMP(input_y.mult, 0.0f, 1.0f);

  return math_interpolate_2d_s16(input_x, input_y, x_size, table, transform);
}

INLINE float math_interpolate_2d_limit_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int32_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  input_x.mult = CLAMP(input_x.mult, 0.0f, 1.0f);
  input_y.mult = CLAMP(input_y.mult, 0.0f, 1.0f);

  return math_interpolate_2d_s32(input_x, input_y, x_size, table, transform);
}

INLINE float math_interpolate_2d_point_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint8_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  result = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;

  return result;
}

INLINE float math_interpolate_2d_point_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint16_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  result = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;

  return result;
}

INLINE float math_interpolate_2d_point_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint32_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  result = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;

  return result;
}

INLINE float math_interpolate_2d_point_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int8_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  result = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;

  return result;
}

INLINE float math_interpolate_2d_point_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int16_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  result = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;

  return result;
}

INLINE float math_interpolate_2d_point_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int32_t (*table)[x_size], const sMathInterpolationTransform *transform)
{
  float result = 0.0f;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  result = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;

  return result;
}

INLINE float math_interpolate_2d_set_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint8_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output_1d[2];
  float input_2d[2][2];
  float val;

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  previous = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];
  diff = new_value - previous;

  limit_h = (limit_h - transform->offset) / transform->gain;
  limit_l = (limit_l - transform->offset) / transform->gain;

  limit_h = CLAMP(limit_h, 0, UCHAR_MAX);
  limit_l = CLAMP(limit_l, 0, UCHAR_MAX);

  val = input_2d[0][0] + diff * (1.0f - input_x.mult) * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[0]] = val;

  val = input_2d[0][1] + diff * input_x.mult * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[1]] = val;

  val = input_2d[1][0] + diff * (1.0f - input_x.mult) * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[0]] = val;

  val = input_2d[1][1] + diff * input_x.mult * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_2d_set_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint16_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output_1d[2];
  float input_2d[2][2];
  float val;

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  previous = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];
  diff = new_value - previous;

  limit_h = (limit_h - transform->offset) / transform->gain;
  limit_l = (limit_l - transform->offset) / transform->gain;

  limit_h = CLAMP(limit_h, 0, USHRT_MAX);
  limit_l = CLAMP(limit_l, 0, USHRT_MAX);

  val = input_2d[0][0] + diff * (1.0f - input_x.mult) * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[0]] = val;

  val = input_2d[0][1] + diff * input_x.mult * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[1]] = val;

  val = input_2d[1][0] + diff * (1.0f - input_x.mult) * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[0]] = val;

  val = input_2d[1][1] + diff * input_x.mult * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_2d_set_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint32_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output_1d[2];
  float input_2d[2][2];
  float val;

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  previous = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];
  diff = new_value - previous;

  limit_h = (limit_h - transform->offset) / transform->gain;
  limit_l = (limit_l - transform->offset) / transform->gain;

  limit_h = CLAMP(limit_h, 0, UINT_MAX);
  limit_l = CLAMP(limit_l, 0, UINT_MAX);

  val = input_2d[0][0] + diff * (1.0f - input_x.mult) * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[0]] = val;

  val = input_2d[0][1] + diff * input_x.mult * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[1]] = val;

  val = input_2d[1][0] + diff * (1.0f - input_x.mult) * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[0]] = val;

  val = input_2d[1][1] + diff * input_x.mult * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_2d_set_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int8_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output_1d[2];
  float input_2d[2][2];
  float val;

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  previous = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];
  diff = new_value - previous;

  limit_h = (limit_h - transform->offset) / transform->gain;
  limit_l = (limit_l - transform->offset) / transform->gain;

  limit_h = CLAMP(limit_h, SCHAR_MIN, SCHAR_MAX);
  limit_l = CLAMP(limit_l, SCHAR_MIN, SCHAR_MAX);

  val = input_2d[0][0] + diff * (1.0f - input_x.mult) * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[0]] = val;

  val = input_2d[0][1] + diff * input_x.mult * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[1]] = val;

  val = input_2d[1][0] + diff * (1.0f - input_x.mult) * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[0]] = val;

  val = input_2d[1][1] + diff * input_x.mult * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_2d_set_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int16_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output_1d[2];
  float input_2d[2][2];
  float val;

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  previous = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];
  diff = new_value - previous;

  limit_h = (limit_h - transform->offset) / transform->gain;
  limit_l = (limit_l - transform->offset) / transform->gain;

  limit_h = CLAMP(limit_h, SHRT_MIN, SHRT_MAX);
  limit_l = CLAMP(limit_l, SHRT_MIN, SHRT_MAX);

  val = input_2d[0][0] + diff * (1.0f - input_x.mult) * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[0]] = val;

  val = input_2d[0][1] + diff * input_x.mult * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[1]] = val;

  val = input_2d[1][0] + diff * (1.0f - input_x.mult) * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[0]] = val;

  val = input_2d[1][1] + diff * input_x.mult * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_2d_set_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int32_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output_1d[2];
  float input_2d[2][2];
  float val;

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]] * transform->gain + transform->offset;
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]] * transform->gain + transform->offset;
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]] * transform->gain + transform->offset;

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  previous = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];
  diff = new_value - previous;

  limit_h = (limit_h - transform->offset) / transform->gain;
  limit_l = (limit_l - transform->offset) / transform->gain;

  limit_h = CLAMP(limit_h, INT_MIN, INT_MAX);
  limit_l = CLAMP(limit_l, INT_MIN, INT_MAX);

  val = input_2d[0][0] + diff * (1.0f - input_x.mult) * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[0]] = val;

  val = input_2d[0][1] + diff * input_x.mult * (1.0f - input_y.mult);
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[0]][input_x.indexes[1]] = val;

  val = input_2d[1][0] + diff * (1.0f - input_x.mult) * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[0]] = val;

  val = input_2d[1][1] + diff * input_x.mult * input_y.mult;
  val = CLAMP(val, limit_l, limit_h);
  table[input_y.indexes[1]][input_x.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_2d_set_point_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint8_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  previous = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;
  diff = new_value - previous;

  new_value = CLAMP(new_value, limit_l, limit_h);
  new_value = (new_value - transform->offset) / transform->gain;
  new_value = CLAMP(new_value, 0, UCHAR_MAX);

  table[input_y.indexes[index_y]][input_x.indexes[index_x]] = new_value;

  return diff;
}

INLINE float math_interpolate_2d_set_point_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint16_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  previous = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;
  diff = new_value - previous;

  new_value = CLAMP(new_value, limit_l, limit_h);
  new_value = (new_value - transform->offset) / transform->gain;
  new_value = CLAMP(new_value, 0, USHRT_MAX);

  table[input_y.indexes[index_y]][input_x.indexes[index_x]] = new_value;

  return diff;
}

INLINE float math_interpolate_2d_set_point_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint32_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  previous = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;
  diff = new_value - previous;

  new_value = CLAMP(new_value, limit_l, limit_h);
  new_value = (new_value - transform->offset) / transform->gain;
  new_value = CLAMP(new_value, 0, UINT_MAX);

  table[input_y.indexes[index_y]][input_x.indexes[index_x]] = new_value;

  return diff;
}

INLINE float math_interpolate_2d_set_point_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int8_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  previous = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;
  diff = new_value - previous;

  new_value = CLAMP(new_value, limit_l, limit_h);
  new_value = (new_value - transform->offset) / transform->gain;
  new_value = CLAMP(new_value, SCHAR_MIN, SCHAR_MAX);

  table[input_y.indexes[index_y]][input_x.indexes[index_x]] = new_value;

  return diff;
}

INLINE float math_interpolate_2d_set_point_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int16_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  previous = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;
  diff = new_value - previous;

  new_value = CLAMP(new_value, limit_l, limit_h);
  new_value = (new_value - transform->offset) / transform->gain;
  new_value = CLAMP(new_value, SHRT_MIN, SHRT_MAX);

  table[input_y.indexes[index_y]][input_x.indexes[index_x]] = new_value;

  return diff;
}

INLINE float math_interpolate_2d_set_point_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int32_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  previous = table[input_y.indexes[index_y]][input_x.indexes[index_x]] * transform->gain + transform->offset;
  diff = new_value - previous;

  new_value = CLAMP(new_value, limit_l, limit_h);
  new_value = (new_value - transform->offset) / transform->gain;
  new_value = CLAMP(new_value, INT_MIN, INT_MAX);

  table[input_y.indexes[index_y]][input_x.indexes[index_x]] = new_value;

  return diff;
}

INLINE float math_interpolate_1d_set_u8(sMathInterpolateInput input, uint8_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output[2];
  float val;

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  previous = (output[1] - output[0]) * input.mult + output[0];

  diff = new_value - previous;

  val = output[0] + diff * (1.0f - input.mult);
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, 0, UCHAR_MAX);
  table[input.indexes[0]] = val;

  val = output[1] + diff * input.mult;
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, 0, UCHAR_MAX);
  table[input.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_1d_set_u16(sMathInterpolateInput input, uint16_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output[2];
  float val;

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  previous = (output[1] - output[0]) * input.mult + output[0];

  diff = new_value - previous;

  val = output[0] + diff * (1.0f - input.mult);
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, 0, USHRT_MAX);
  table[input.indexes[0]] = val;

  val = output[1] + diff * input.mult;
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, 0, USHRT_MAX);
  table[input.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_1d_set_u32(sMathInterpolateInput input, uint32_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output[2];
  float val;

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  previous = (output[1] - output[0]) * input.mult + output[0];

  diff = new_value - previous;

  val = output[0] + diff * (1.0f - input.mult);
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, 0, UINT_MAX);
  table[input.indexes[0]] = val;

  val = output[1] + diff * input.mult;
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, 0, UINT_MAX);
  table[input.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_1d_set_s8(sMathInterpolateInput input, int8_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output[2];
  float val;

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  previous = (output[1] - output[0]) * input.mult + output[0];

  diff = new_value - previous;

  val = output[0] + diff * (1.0f - input.mult);
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, SCHAR_MIN, SCHAR_MAX);
  table[input.indexes[0]] = val;

  val = output[1] + diff * input.mult;
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, SCHAR_MIN, SCHAR_MAX);
  table[input.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_1d_set_s16(sMathInterpolateInput input, int16_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output[2];
  float val;

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  previous = (output[1] - output[0]) * input.mult + output[0];

  diff = new_value - previous;

  val = output[0] + diff * (1.0f - input.mult);
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, SHRT_MIN, SHRT_MAX);
  table[input.indexes[0]] = val;

  val = output[1] + diff * input.mult;
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, SHRT_MIN, SHRT_MAX);
  table[input.indexes[1]] = val;

  return diff;
}

INLINE float math_interpolate_1d_set_s32(sMathInterpolateInput input, int32_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output[2];
  float val;

  output[0] = table[input.indexes[0]] * transform->gain + transform->offset;
  output[1] = table[input.indexes[1]] * transform->gain + transform->offset;

  previous = (output[1] - output[0]) * input.mult + output[0];

  diff = new_value - previous;

  val = output[0] + diff * (1.0f - input.mult);
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, INT_MIN, INT_MAX);
  table[input.indexes[0]] = val;

  val = output[1] + diff * input.mult;
  val = CLAMP(val, limit_l, limit_h);
  val = (val - transform->offset) * transform->gain;
  val = CLAMP(val, INT_MIN, INT_MAX);
  table[input.indexes[1]] = val;

  return diff;
}

