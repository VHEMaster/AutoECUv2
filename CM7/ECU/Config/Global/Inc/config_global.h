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

#include "flash.h"
#include "config_devices.h"

#include "config_flash.h"
#include "config_flexio.h"
#include "config_motor.h"
#include "config_stepper.h"
#include "config_wbls.h"

#define ECU_CONFIG_ITEM_VERSIONS_MAX     (4)

typedef enum {
  ECU_CONFIG_COMP_TYPE_FLASH = 0,
  ECU_CONFIG_COMP_TYPE_FLEXIO,
  ECU_CONFIG_COMP_TYPE_MOTOR,
  ECU_CONFIG_COMP_TYPE_STEPPER,
  ECU_CONFIG_COMP_TYPE_WBLS,
  ECU_CONFIG_COMP_TYPE_MAX
}ecu_config_component_type_t;

typedef enum {
  ECU_CONFIG_CALIB_TYPE_ID = 0,
  ECU_CONFIG_CALIB_TYPE_MAX
}ecu_config_calibration_type_t;

typedef enum {
  ECU_CONFIG_RUNTIME_TYPE_MAX
}ecu_config_runtime_type_t;

typedef enum {
  ECU_CONFIG_TYPE_COMPONENT = 0,
  ECU_CONFIG_TYPE_CALIBRATION,
  ECU_CONFIG_TYPE_RUNTIME,
  ECU_CONFIG_TYPE_MAX,
}ecu_config_type_t;

typedef enum {
  ECU_CONFIG_OP_READ = 0,
  ECU_CONFIG_OP_WRITE,
  ECU_CONFIG_OP_MAX,
}ecu_config_op_t;

typedef error_t (*ecu_config_translate_prev_to_this_func_t)(const void *src, void *dest);
typedef error_t (*ecu_config_get_default_cfg_func_t)(ecu_index_type_t index, void *config);
typedef error_t (*ecu_config_configure_func_t)(ecu_device_instance_t instance, const void *config);
typedef error_t (*ecu_config_reset_func_t)(ecu_device_instance_t instance);

typedef struct {
    uint32_t version;
    uint32_t size;
    ecu_config_translate_prev_to_this_func_t translate_func;
}ecu_config_item_version_t;

typedef struct {
    flash_section_type_t flash_section_type;
    ecu_config_get_default_cfg_func_t get_default_cfg_func;
    void *data_ptr;
    uint32_t data_size;
    uint32_t versions_count;
    ecu_config_item_version_t versions[ECU_CONFIG_ITEM_VERSIONS_MAX];
}ecu_config_generic_ctx_t;

typedef struct {
    ecu_device_type_t device_type;
    uint32_t instances_count;
    ecu_config_configure_func_t configure_func;
    ecu_config_reset_func_t reset_func;
    ecu_config_generic_ctx_t generic;
    error_t reset_errcode;
    error_t config_errcode;
}ecu_config_component_ctx_t;

typedef enum {
  ECU_CONFIG_FSM_RST_CFG_CONDITION = 0,
  ECU_CONFIG_FSM_RST_CFG_DEFINE,
  ECU_CONFIG_FSM_RST_CFG_RESET,
  ECU_CONFIG_FSM_RST_CFG_CONFIG,
}ecu_config_global_rst_cfg_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_PROCESS_CFG_RST = 0,
  ECU_CONFIG_FSM_PROCESS_MAX,
}ecu_config_global_process_fsm_t;

typedef struct {
    uint32_t components_count;
    ecu_config_component_ctx_t *components;
    uint32_t calibrations_count;
    ecu_config_generic_ctx_t *calibrations;
    uint32_t runtimes_count;
    ecu_config_generic_ctx_t *runtimes;
    bool components_ready;
    bool components_initialized;

    bool config_read_request;
    bool config_write_request;
    error_t config_rw_errcode;
    ecu_config_type_t config_rw_type;
    ecu_index_type_t config_rw_index;

    ecu_config_global_rst_cfg_fsm_t fsm_rst_cfg;
    ecu_config_global_process_fsm_t fsm_process;

    bool process_comps_init;
    error_t process_result;
    ecu_config_component_type_t process_comp_type;
    ecu_device_instance_t process_instance;
}ecu_config_global_runtime_ctx_t;

error_t ecu_config_global_init(void);
void ecu_config_global_loop_main(void);
void ecu_config_global_loop_slow(void);
void ecu_config_global_loop_fast(void);
error_t ecu_config_global_components_initialize(void);

error_t ecu_config_global_operation(ecu_config_op_t op, ecu_config_type_t type, ecu_index_type_t index);

#endif /* CONFIG_GLOBAL_INC_CONFIG_GLOBAL_H_ */
