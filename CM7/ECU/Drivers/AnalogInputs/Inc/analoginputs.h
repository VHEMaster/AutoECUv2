/*
 * analoginputs.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef INC_ANALOGINPUTS_H_
#define INC_ANALOGINPUTS_H_

#include "../../../Drivers/Inputs/Inc/inputs.h"

typedef float analog_input_value_t;

void analog_input_configure(input_id_t channel_id, float gain, float offset);
void analog_input_get_value(input_id_t channel_id, analog_input_value_t *value);

#endif /* INC_ANALOGINPUTS_H_ */
