/*
 * config_output.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_OUTPUT_H_
#define CONFIG_INC_CONFIG_OUTPUT_H_

#include "config_devices.h"
#include "tle6240.h"

error_t ecu_devices_output_init(ecu_device_output_t instance, tle6240_ctx_t *ctx);

#endif /* CONFIG_INC_CONFIG_OUTPUT_H_ */
