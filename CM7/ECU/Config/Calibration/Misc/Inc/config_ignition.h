/*
 * config_ignition.h
 *
 *  Created on: Jul 7, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_IGNITION_H_
#define CONFIG_INC_CONFIG_IGNITION_H_

#include "versioned_calibration.h"

error_t ecu_calibration_ignition_get_default_config(ecu_calibration_instance_t instance, ecu_config_ignition_t *config);

#endif /* CONFIG_INC_CONFIG_IGNITION_H_ */
