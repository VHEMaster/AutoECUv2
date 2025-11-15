/*
 * config_devices_types.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_DEVICES_TYPES_H_
#define CONFIG_INC_CONFIG_DEVICES_TYPES_H_

#include "common.h"

#define ECU_DEVICE_NONE     (255)
#define ECU_DEVICE_INSTANCE_MAX (2)

#define ECU_DEVICES_MAX (       \
    ECU_DEVICE_PULSEDADC_MAX  + \
    ECU_DEVICE_TCS_MAX        + \
    ECU_DEVICE_FLEXIO_MAX     + \
    ECU_DEVICE_WBLS_MAX       + \
    ECU_DEVICE_STEPPER_MAX    + \
    ECU_DEVICE_OUTPUT_MAX     + \
    ECU_DEVICE_MOTOR_MAX      + \
    ECU_DEVICE_FLASH_MAX)

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

#endif /* CONFIG_INC_CONFIG_DEVICES_TYPES_H_ */
