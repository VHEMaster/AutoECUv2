/*
 * config_global.h
 *
 *  Created on: Apr 30, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_GLOBAL_INC_CONFIG_GLOBAL_H_
#define CONFIG_GLOBAL_INC_CONFIG_GLOBAL_H_

#include "bool.h"
#include "errors.h"
#include <stdint.h>
#include <stddef.h>

#include "config_devices.h"

#include "config_flexio.h"
#include "config_motor.h"
#include "config_stepper.h"
#include "config_wbls.h"
#include "versioned_flexio.h"
#include "versioned_motor.h"
#include "versioned_stepper.h"
#include "versioned_wbls.h"

#define ECU_CONFIG_ITEM_VERSIONS_MAX     (4)

typedef enum {
  ECU_CONFIG_COMP_TYPE_FLEXIO = 0,
  ECU_CONFIG_COMP_TYPE_MOTOR,
  ECU_CONFIG_COMP_TYPE_STEPPER,
  ECU_CONFIG_COMP_TYPE_WBLS,
  ECU_CONFIG_COMP_TYPE_MAX
}ecu_config_component_type_t;

typedef enum {
  ECU_CONFIG_CALIB_TYPE_MAX,
}ecu_config_calibration_type_t;

typedef error_t (*ecu_config_translate_prev_to_this_func_t)(const void *src, void *dest);
typedef error_t (*ecu_config_get_default_cfg_func_t)(ecu_device_instance_t instance, void *config);
typedef error_t (*ecu_config_configure_func_t)(ecu_device_instance_t instance, const void *config);
typedef error_t (*ecu_config_reset_func_t)(ecu_device_instance_t instance);

typedef struct {
    uint32_t version;
    uint32_t size;
    ecu_config_translate_prev_to_this_func_t translate_func;
}ecu_config_item_version_t;

typedef struct {
    ecu_device_type_t device_type;
    uint32_t instances_count;
    ecu_config_get_default_cfg_func_t get_default_cfg_func;
    ecu_config_configure_func_t configure_func;
    ecu_config_reset_func_t reset_func;
    uint32_t versions_count;
    ecu_config_item_version_t versions[ECU_CONFIG_ITEM_VERSIONS_MAX];

    error_t reset_errcode;
    error_t config_errcode;
}ecu_config_component_ctx_t;

typedef struct {
    uint32_t versions_count;
    ecu_config_item_version_t versions[ECU_CONFIG_ITEM_VERSIONS_MAX];
}ecu_config_calibration_ctx_t;

typedef enum {
  ECU_CONFIG_PROC_TRIG_TYPE_NONE = 0,
  ECU_CONFIG_PROC_TRIG_TYPE_RESET,
  ECU_CONFIG_PROC_TRIG_TYPE_CONFIGURE,
}ecu_config_global_process_trigger_type_t;

typedef struct {
    uint32_t components_count;
    ecu_config_component_ctx_t *components;
    uint32_t calibrations_count;
    ecu_config_calibration_ctx_t *calibrations;
    bool components_ready;
    bool components_initialized;
    bool components_configured;
    ecu_config_global_process_trigger_type_t process_trigger_type;
    error_t process_result;
    ecu_config_component_type_t process_comp_type;
    ecu_device_instance_t process_instance;
}ecu_config_global_runtime_ctx_t;

error_t ecu_config_global_init(void);
void ecu_config_global_loop_main(void);
void ecu_config_global_loop_slow(void);
void ecu_config_global_loop_fast(void);
error_t ecu_config_global_components_reset(void);
error_t ecu_config_global_components_configure(void);

#endif /* CONFIG_GLOBAL_INC_CONFIG_GLOBAL_H_ */
