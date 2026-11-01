/*
 * interpolation_extended.h
 *
 *  Created on: 19 фев. 2024 г.
 *      Author: VHEMaster
 */

#ifndef INTERPOLATION_EXTENDED_H_
#define INTERPOLATION_EXTENDED_H_

#include "interpolation.h"

sMathInterpolateInput math_interpolate_input_u8(float value, const uint8_t *table, uint32_t size, const sMathInterpolationTransform *transform);
sMathInterpolateInput math_interpolate_input_u16(float value, const uint16_t *table, uint32_t size, const sMathInterpolationTransform *transform);
sMathInterpolateInput math_interpolate_input_u32(float value, const uint32_t *table, uint32_t size, const sMathInterpolationTransform *transform);
sMathInterpolateInput math_interpolate_input_s8(float value, const int8_t *table, uint32_t size, const sMathInterpolationTransform *transform);
sMathInterpolateInput math_interpolate_input_s16(float value, const int16_t *table, uint32_t size, const sMathInterpolationTransform *transform);
sMathInterpolateInput math_interpolate_input_s32(float value, const int32_t *table, uint32_t size, const sMathInterpolationTransform *transform);

float math_interpolate_1d_u8(sMathInterpolateInput input, const uint8_t *table, const sMathInterpolationTransform *transform);
float math_interpolate_1d_u16(sMathInterpolateInput input, const uint16_t *table, const sMathInterpolationTransform *transform);
float math_interpolate_1d_u32(sMathInterpolateInput input, const uint32_t *table, const sMathInterpolationTransform *transform);
float math_interpolate_1d_s8(sMathInterpolateInput input, const int8_t *table, const sMathInterpolationTransform *transform);
float math_interpolate_1d_s16(sMathInterpolateInput input, const int16_t *table, const sMathInterpolationTransform *transform);
float math_interpolate_1d_s32(sMathInterpolateInput input, const int32_t *table, const sMathInterpolationTransform *transform);

float math_interpolate_2d_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint8_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint16_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint32_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int8_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int16_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int32_t (*table)[x_size], const sMathInterpolationTransform *transform);

float math_interpolate_2d_limit_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint8_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_limit_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint16_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_limit_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint32_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_limit_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int8_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_limit_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int16_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_limit_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int32_t (*table)[x_size], const sMathInterpolationTransform *transform);

float math_interpolate_2d_point_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint8_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_point_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint16_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_point_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const uint32_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_point_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int8_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_point_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int16_t (*table)[x_size], const sMathInterpolationTransform *transform);
float math_interpolate_2d_point_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, const int32_t (*table)[x_size], const sMathInterpolationTransform *transform);

float math_interpolate_2d_set_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint8_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint16_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint32_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int8_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int16_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int32_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);

float math_interpolate_2d_set_point_u8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint8_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_point_u16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint16_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_point_u32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, uint32_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_point_s8(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int8_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_point_s16(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int16_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_2d_set_point_s32(sMathInterpolateInput input_x, sMathInterpolateInput input_y,
    uint32_t x_size, int32_t (*table)[x_size], const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);

float math_interpolate_1d_set_u8(sMathInterpolateInput input, uint8_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_1d_set_u16(sMathInterpolateInput input, uint16_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_1d_set_u32(sMathInterpolateInput input, uint32_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_1d_set_s8(sMathInterpolateInput input, int8_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_1d_set_s16(sMathInterpolateInput input, int16_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);
float math_interpolate_1d_set_s32(sMathInterpolateInput input, int32_t *table, const sMathInterpolationTransform *transform, float new_value, float limit_l, float limit_h);

#endif /* INTERPOLATION_EXTENDED_H_ */
