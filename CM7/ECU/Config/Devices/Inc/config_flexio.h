/*
 * config_flexio.h
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_FLEXIO_H_
#define CONFIG_INC_CONFIG_FLEXIO_H_

#include "config_devices.h"
#include "l9966.h"

typedef enum {
  ECU_DEVICE_FLEXIO_READ_PARAM_MAX
}ecu_device_flexio_read_params_t;

typedef enum {
  ECU_DEVICE_FLEXIO_WRITE_PARAM_MAX
}ecu_device_flexio_write_params_t;

error_t ecu_devices_flexio_init(ecu_device_flexio_t instance, l9966_ctx_t *ctx);
error_t ecu_devices_flexio_get_default_config(ecu_device_flexio_t instance, l9966_config_t *config);
error_t ecu_devices_flexio_configure(ecu_device_flexio_t instance, const l9966_config_t *config);
error_t ecu_devices_flexio_reset(ecu_device_flexio_t instance);

#endif /* CONFIG_INC_CONFIG_FLEXIO_H_ */
