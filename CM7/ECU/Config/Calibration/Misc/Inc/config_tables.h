/*
 * config_tables.h
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_TABLES_H_
#define CONFIG_INC_CONFIG_TABLES_H_

#include "versioned_calibration.h"

error_t ecu_calibration_tables_get_default_config(ecu_calibration_instance_t instance, ecu_config_tables_t *config);

#endif /* CONFIG_INC_CONFIG_TABLES_H_ */
