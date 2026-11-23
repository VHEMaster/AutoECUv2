/*
 * interpolation.h
 *
 *  Created on: 10 мар. 2022 г.
 *      Author: VHEMaster
 */

#ifndef INTERPOLATION_H_
#define INTERPOLATION_H_

#include "stdint.h"
#include "bool.h"

typedef struct {
    uint32_t indexes[2];
    float mult;
}sMathInterpolateInput;

typedef struct {
    float gain;
    float offset;
}sMathInterpolationTransform;

void math_interpolate_test(void);

sMathInterpolateInput math_interpolate_input(float value, const float *table, uint32_t size);

float math_interpolate_1d(const sMathInterpolateInput *input, const float *table);
float math_interpolate_1d_ex(const sMathInterpolateInput *input, const float *table, bool limit);
float math_interpolate_2d(const sMathInterpolateInput *input_x, const sMathInterpolateInput *input_y,
    uint32_t x_size, const float (*table)[x_size]);
float math_interpolate_2d_ex(const sMathInterpolateInput *input_x, const sMathInterpolateInput *input_y,
    uint32_t x_size, const float (*table)[x_size], bool limit);
float math_interpolate_2d_point(const sMathInterpolateInput *input_x, const sMathInterpolateInput *input_y,
    uint32_t x_size, const float (*table)[x_size]);

float math_interpolate_1d_set(const sMathInterpolateInput *input, float *table, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set(const sMathInterpolateInput *input_x, const sMathInterpolateInput *input_y,
    uint32_t x_size, float (*table)[x_size], float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_point(const sMathInterpolateInput *input_x, const sMathInterpolateInput *input_y,
    uint32_t x_size, float (*table)[x_size], float new_value, float limit_l, float limit_h);

float math_interpolate_1d_offset(const sMathInterpolateInput *input, const float *table, uint32_t offset);

float math_interpolate(float input_left, float input_mid, float input_right, float base_left, float base_right);

float math_interpolate_1d_direct(float value, const float *input, const float *output, uint32_t size);

#endif /* INTERPOLATION_H_ */
