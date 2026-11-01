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

typedef enum {
  ECU_DEVICE_OUTPUT_READ_PARAM_MAX
}ecu_device_output_read_params_t;

typedef enum {
  ECU_DEVICE_OUTPUT_WRITE_PARAM_MAX
}ecu_device_output_write_params_t;

error_t ecu_devices_output_init(ecu_device_output_t instance, tle6240_ctx_t *ctx);

#endif /* CONFIG_INC_CONFIG_OUTPUT_H_ */
