/*
 * config_sensdata.h
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_SENSDATA_H_
#define CONFIG_INC_CONFIG_SENSDATA_H_

#include "versioned_calibration.h"

error_t ecu_calibration_sensdata_get_default_config(ecu_calibration_instance_t instance, ecu_config_sensdata_t *config);

#endif /* CONFIG_INC_CONFIG_SENSDATA_H_ */
