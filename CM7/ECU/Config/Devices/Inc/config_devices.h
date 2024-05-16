/*
 * config_devices.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_DEVICES_H_
#define CONFIG_INC_CONFIG_DEVICES_H_

#include "errors.h"
#include "pulsedadc.h"
#include "max31855.h"
#include "l9966.h"
#include "cj125.h"
#include "tle4729.h"
#include "tle6240.h"
#include "l9960.h"
#include "qspi.h"

typedef uint32_t ecu_device_instance_t;

typedef enum {
  ECU_DEVICE_TYPE_PULSEDADC,
  ECU_DEVICE_TYPE_TCS,
  ECU_DEVICE_TYPE_FLEXIO,
  ECU_DEVICE_TYPE_WBLS,
  ECU_DEVICE_TYPE_STEPPER,
  ECU_DEVICE_TYPE_OUTPUT,
  ECU_DEVICE_TYPE_MOTOR,
  ECU_DEVICE_TYPE_FLASH,
  ECU_DEVICE_TYPE_MAX
}ecu_device_type_t;

typedef void (*ecu_device_loop_func_t)(void *ctx);

typedef enum {
  ECU_DEVICE_PULSEDADC_1 = 0,
  ECU_DEVICE_PULSEDADC_2,
  ECU_DEVICE_PULSEDADC_MAX
}ecu_device_pulsedadc_t;

typedef enum {
  ECU_DEVICE_TCS_1 = 0,
  ECU_DEVICE_TCS_2,
  ECU_DEVICE_TCS_MAX
}ecu_device_tcs_t;

typedef enum {
  ECU_DEVICE_FLEXIO_1 = 0,
  ECU_DEVICE_FLEXIO_2,
  ECU_DEVICE_FLEXIO_MAX
}ecu_device_flexio_t;

typedef enum {
  ECU_DEVICE_WBLS_1 = 0,
  ECU_DEVICE_WBLS_2,
  ECU_DEVICE_WBLS_MAX
}ecu_device_wbls_t;

typedef enum {
  ECU_DEVICE_STEPPER_1 = 0,
  ECU_DEVICE_STEPPER_MAX
}ecu_device_stepper_t;

typedef enum {
  ECU_DEVICE_OUTPUT_1 = 0,
  ECU_DEVICE_OUTPUT_2,
  ECU_DEVICE_OUTPUT_MAX
}ecu_device_output_t;

typedef enum {
  ECU_DEVICE_MOTOR_1 = 0,
  ECU_DEVICE_MOTOR_2,
  ECU_DEVICE_MOTOR_MAX
}ecu_device_motor_t;

typedef enum {
  ECU_DEVICE_FLASH_1 = 0,
  ECU_DEVICE_FLASH_MAX
}ecu_device_flash_t;

error_t ecu_devices_init(void);
void ecu_devices_loop_main(void);
void ecu_devices_loop_slow(void);
void ecu_devices_loop_fast(void);

error_t ecu_devices_get_device_ctx(ecu_device_type_t type, ecu_device_instance_t instance, void **ctx);
error_t ecu_devices_set_device_initialized(ecu_device_type_t type, ecu_device_instance_t instance, bool initialized);

error_t ecu_devices_get_pulsedadc_ctx(ecu_device_pulsedadc_t instance, pulsedadc_ctx_t **ctx);
error_t ecu_devices_get_tcs_ctx(ecu_device_tcs_t instance, max31855_ctx_t **ctx);
error_t ecu_devices_get_flexio_ctx(ecu_device_flexio_t instance, l9966_ctx_t **ctx);
error_t ecu_devices_get_wbls_ctx(ecu_device_wbls_t instance, cj125_ctx_t **ctx);
error_t ecu_devices_get_stepper_ctx(ecu_device_stepper_t instance, tle4729_ctx_t **ctx);
error_t ecu_devices_get_output_ctx(ecu_device_output_t instance, tle6240_ctx_t **ctx);
error_t ecu_devices_get_motor_ctx(ecu_device_motor_t instance, l9960_ctx_t **ctx);
error_t ecu_devices_get_flash_ctx(ecu_device_flash_t instance, qspi_ctx_t **ctx);

#endif /* CONFIG_INC_CONFIG_DEVICES_H_ */
