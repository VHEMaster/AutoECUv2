/*
 * math_misc.h
 *
 *  Created on: 12 мар. 2022 г.
 *      Author: VHEMaster
 */

#ifndef MATH_MISC_H_
#define MATH_MISC_H_

void math_minmax(const float *array, unsigned int size, float *pmin, float *pmax);
float math_median(const float *array, unsigned int size);
int math_binary_search(const float *array, int start_index, int end_index, float element);

#endif /* MATH_MISC_H_ */
