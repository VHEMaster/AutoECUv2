/*
 * config_io.h
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_IO_H_
#define CONFIG_INC_CONFIG_IO_H_

#include "versioned_calibration.h"

error_t ecu_calibration_io_get_default_config(ecu_calibration_instance_t instance, ecu_config_io_t *config);

#endif /* CONFIG_INC_CONFIG_IO_H_ */
