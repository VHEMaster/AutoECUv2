/*
 * interpolation.c
 *
 *  Created on: 10 мар. 2022 г.
 *      Author: VHEMaster
 */

#include "interpolation.h"
#include "math_misc.h"
#include "compiler.h"

INLINE sMathInterpolateInput math_interpolate_input(float value, const float *table, uint32_t size)
{
  sMathInterpolateInput result = {0};
  int find_index = -1;

  if(value != value)
    return result;

  result.input = value;
  result.size = size;

  if(size == 1)
  {
    result.indexes[0] = 0;
    result.indexes[1] = 0;
    result.values[0] = table[0];
    result.values[1] = table[0];
  }
  else if(value <= table[1])
  {
    result.indexes[0] = 0;
    result.indexes[1] = 1;
    result.values[0] = table[result.indexes[0]];
    result.values[1] = table[result.indexes[1]];
  }
  else if(value >= table[size - 2])
  {
    result.indexes[0] = size - 2;
    result.indexes[1] = size - 1;
    result.values[0] = table[result.indexes[0]];
    result.values[1] = table[result.indexes[1]];
  }
  else
  {
    find_index = math_binary_search(table, 0, size - 1, value);
    if(find_index >= 0) {
      result.indexes[0] = find_index;
      result.indexes[1] = find_index + 1;
      result.values[0] = table[find_index];
      result.values[1] = table[find_index+1];
    }
  }

  if(result.values[1] != result.values[0])
    result.mult = (value - result.values[0]) / (result.values[1] - result.values[0]);
  else result.mult = 1.0f;

  return result;
}

INLINE float math_interpolate_1d(sMathInterpolateInput input, const float *table)
{
  float result;
  float output[2];

  output[0] = table[input.indexes[0]];
  output[1] = table[input.indexes[1]];

  result = (output[1] - output[0]) * input.mult + output[0];

  return result;
}

INLINE float math_interpolate_2d(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const float (*table)[x_size])
{
  float result = 0.0f;
  float output_1d[2];
  float input_2d[2][2];

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]];
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]];
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]];
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]];

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  result = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];

  return result;
}

INLINE float math_interpolate_2d_limit(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const float (*table)[x_size])
{
  input_x.mult = CLAMP(input_x.mult, 0.0f, 1.0f);
  input_y.mult = CLAMP(input_y.mult, 0.0f, 1.0f);

  return math_interpolate_2d(input_x, input_y, x_size, table);
}

INLINE float math_interpolate_2d_point(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const float (*table)[x_size])
{
  float result = 0.0f;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  result = table[input_y.indexes[index_y]][input_x.indexes[index_x]];

  return result;
}

INLINE float math_interpolate_1d_set(sMathInterpolateInput input, float *table, float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output[2];

  output[0] = table[input.indexes[0]];
  output[1] = table[input.indexes[1]];

  previous = (output[1] - output[0]) * input.mult + output[0];

  diff = new_value - previous;

  table[input.indexes[0]] = output[0] + diff * (1.0f - input.mult);
  table[input.indexes[1]] = output[1] + diff * input.mult;


  for(int i = 0; i < 2; i++) {
    if(table[input.indexes[i]] > limit_h)
      table[input.indexes[i]] = limit_h;
    else if(table[input.indexes[i]] < limit_l)
      table[input.indexes[i]] = limit_l;
  }

  return diff;
}

INLINE float math_interpolate_2d_set(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, float (*table)[x_size], float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  float output_1d[2];
  float input_2d[2][2];

  input_2d[0][0] = table[input_y.indexes[0]][input_x.indexes[0]];
  input_2d[0][1] = table[input_y.indexes[0]][input_x.indexes[1]];
  input_2d[1][0] = table[input_y.indexes[1]][input_x.indexes[0]];
  input_2d[1][1] = table[input_y.indexes[1]][input_x.indexes[1]];

  output_1d[0] = (input_2d[0][1] - input_2d[0][0]) * input_x.mult + input_2d[0][0];
  output_1d[1] = (input_2d[1][1] - input_2d[1][0]) * input_x.mult + input_2d[1][0];
  previous = (output_1d[1] - output_1d[0]) * input_y.mult + output_1d[0];
  diff = new_value - previous;

  table[input_y.indexes[0]][input_x.indexes[0]] = input_2d[0][0] + diff * (1.0f - input_x.mult) * (1.0f - input_y.mult);
  table[input_y.indexes[0]][input_x.indexes[1]] = input_2d[0][1] + diff * input_x.mult * (1.0f - input_y.mult);
  table[input_y.indexes[1]][input_x.indexes[0]] = input_2d[1][0] + diff * (1.0f - input_x.mult) * input_y.mult;
  table[input_y.indexes[1]][input_x.indexes[1]] = input_2d[1][1] + diff * input_x.mult * input_y.mult;

  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 2; j++) {
      if(table[input_y.indexes[i]][input_x.indexes[j]] > limit_h)
        table[input_y.indexes[i]][input_x.indexes[j]] = limit_h;
      else if(table[input_y.indexes[i]][input_x.indexes[j]] < limit_l)
        table[input_y.indexes[i]][input_x.indexes[j]] = limit_l;
    }
  }

  return diff;
}

INLINE float math_interpolate_2d_set_point(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, float (*table)[x_size], float new_value, float limit_l, float limit_h)
{
  float previous;
  float diff;
  uint8_t index_x = 0;
  uint8_t index_y = 0;

  if(input_x.mult >= 0.5f)
    index_x = 1;
  if(input_y.mult >= 0.5f)
    index_y = 1;

  previous = table[input_y.indexes[index_y]][input_x.indexes[index_x]];
  diff = new_value - previous;

  if(new_value > limit_h)
    new_value = limit_h;
  else if(new_value < limit_l)
    new_value = limit_l;

  table[input_y.indexes[index_y]][input_x.indexes[index_x]] = new_value;

  return diff;
}

float math_interpolate_1d_offset(sMathInterpolateInput input, const float *table, uint32_t offset)
{
  float result;
  float output[2];
  uint32_t offset_items = offset / sizeof(*table);

  output[0] = table[input.indexes[0] * offset_items];
  output[1] = table[input.indexes[1] * offset_items];

  result = (output[1] - output[0]) * input.mult + output[0];

  return result;
}

INLINE float math_interpolate(float input_left, float input_mid, float input_right, float base_left, float base_right)
{
  float result;
  float mult;

  if(input_right == input_left) {
    result = (base_left + base_right) * 0.5f;
  } else {
    mult = (input_mid - input_left) / (input_right - input_left);
    result = (base_right - base_left) * mult + base_left;
  }

  return result;
}
