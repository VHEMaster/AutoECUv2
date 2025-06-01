/*
 * interpolation.h
 *
 *  Created on: 10 мар. 2022 г.
 *      Author: VHEMaster
 */

#ifndef INTERPOLATION_H_
#define INTERPOLATION_H_

#include "stdint.h"

typedef struct {
    float values[2];
    uint32_t indexes[2];
    uint32_t size;
    float input;
    float mult;
}sMathInterpolateInput;

typedef struct {
    float gain;
    float offset;
}sMathInterpolationTransform;

void math_interpolate_test(void);

sMathInterpolateInput math_interpolate_input(float value, const float *table, uint32_t size);

float math_interpolate_1d(sMathInterpolateInput input, const float *table);
float math_interpolate_2d(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const float (*table)[x_size]);
float math_interpolate_2d_limit(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const float (*table)[x_size]);
float math_interpolate_2d_point(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const float (*table)[x_size]);

float math_interpolate_1d_set(sMathInterpolateInput input, float *table, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, float (*table)[x_size], float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_point(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, float (*table)[x_size], float new_value, float limit_l, float limit_h);

float math_interpolate_1d_offset(sMathInterpolateInput input, const float *table, uint32_t offset);


float math_interpolate(float input_left, float input_mid, float input_right, float base_left, float base_right);

#endif /* INTERPOLATION_H_ */
