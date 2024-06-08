/*
 * analoginputs.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_ANALOGINPUTS_INC_ANALOGINPUTS_H_
#define DRIVERS_ANALOGINPUTS_INC_ANALOGINPUTS_H_

#include "inputs.h"

typedef float analog_input_value_t;

error_t analog_input_configure(input_id_t channel_id, float gain, float offset);
error_t analog_input_get_value(input_id_t channel_id, analog_input_value_t *value);

#endif /* DRIVERS_ANALOGINPUTS_INC_ANALOGINPUTS_H_ */
