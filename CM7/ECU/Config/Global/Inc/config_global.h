/*
 * config_global.h
 *
 *  Created on: Apr 30, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_GLOBAL_INC_CONFIG_GLOBAL_H_
#define CONFIG_GLOBAL_INC_CONFIG_GLOBAL_H_

#include "common.h"

#include "flash.h"
#include "config_calcdata.h"
#include "config_devices.h"
#include "config_sensors.h"
#include "config_modules.h"
#include "config_timings.h"
#include "config_calibration.h"

#include "config_flash.h"
#include "config_flexio.h"
#include "config_motor.h"
#include "config_stepper.h"
#include "config_wbls.h"
#include "config_output.h"
#include "config_tcs.h"

#include "config_ckp.h"
#include "config_cmp.h"
#include "config_ect.h"
#include "config_egt.h"
#include "config_iat.h"
#include "config_maf.h"
#include "config_map.h"
#include "config_tps.h"
#include "config_vss.h"
#include "config_aps.h"
#include "config_ops.h"
#include "config_ots.h"

#include "config_etc.h"
#include "config_vvt.h"
#include "config_fuelpump.h"
#include "config_coolingfan.h"
#include "config_ignpower.h"
#include "config_indication.h"
#include "config_wgcv.h"

#include "config_timing_base.h"
#include "config_ignition.h"
#include "config_injection.h"
#include "config_rough.h"

#include "config_id.h"
#include "config_io.h"
#include "config_powermoding.h"
#include "config_cylinders.h"
#include "config_injection.h"
#include "config_ignition.h"
#include "config_tables.h"

#include "config_router.h"
#include "config_can.h"
#include "config_kwp.h"
#include "config_isotp.h"
#include "config_uds.h"
#include "config_obd2.h"

#include "config_engine.h"
#include "config_common.h"

#define ECU_CONFIG_ITEM_VERSIONS_MAX     (4)

typedef uint32_t ecu_config_item_type_t;
typedef uint32_t ecu_index_type_t;
typedef uint32_t ecu_instance_t;

typedef enum {
  ECU_CONFIG_DEV_TYPE_FLEXIO = 0,
  ECU_CONFIG_DEV_TYPE_MOTOR,
  ECU_CONFIG_DEV_TYPE_STEPPER,
  ECU_CONFIG_DEV_TYPE_WBLS,
  ECU_CONFIG_DEV_TYPE_ALL,
  ECU_CONFIG_DEV_TYPE_MAX
}ecu_config_device_type_t;

typedef enum {
  ECU_CONFIG_SENSOR_TYPE_CKP = 0,
  ECU_CONFIG_SENSOR_TYPE_CMP,
  ECU_CONFIG_SENSOR_TYPE_ECT,
  ECU_CONFIG_SENSOR_TYPE_EGT,
  ECU_CONFIG_SENSOR_TYPE_IAT,
  ECU_CONFIG_SENSOR_TYPE_MAF,
  ECU_CONFIG_SENSOR_TYPE_MAP,
  ECU_CONFIG_SENSOR_TYPE_TPS,
  ECU_CONFIG_SENSOR_TYPE_VSS,
  ECU_CONFIG_SENSOR_TYPE_APS,
  ECU_CONFIG_SENSOR_TYPE_OTS,
  ECU_CONFIG_SENSOR_TYPE_OPS,
  ECU_CONFIG_SENSOR_TYPE_ALL,
  ECU_CONFIG_SENSOR_TYPE_MAX
}ecu_config_sensor_type_t;

typedef enum {
  ECU_CONFIG_MODULE_TYPE_ETC = 0,
  ECU_CONFIG_MODULE_TYPE_VVT,
  ECU_CONFIG_MODULE_TYPE_FUELPUMP,
  ECU_CONFIG_MODULE_TYPE_COOLINGFAN,
  ECU_CONFIG_MODULE_TYPE_IGNPOWER,
  ECU_CONFIG_MODULE_TYPE_INDICATION,
  ECU_CONFIG_MODULE_TYPE_WGCV,
  ECU_CONFIG_MODULE_TYPE_ALL,
  ECU_CONFIG_MODULE_TYPE_MAX
}ecu_config_module_type_t;

typedef enum {
  ECU_CONFIG_TIMING_TYPE_BASE = 0,
  ECU_CONFIG_TIMING_TYPE_IGNITION,
  ECU_CONFIG_TIMING_TYPE_INJECTION,
  ECU_CONFIG_TIMING_TYPE_ROUGH,
  ECU_CONFIG_TIMING_TYPE_ALL,
  ECU_CONFIG_TIMING_TYPE_MAX
}ecu_config_timing_type_t;

typedef enum {
  ECU_CONFIG_CORE_COMPONENT_TYPE_CYLINDERS = 0,
  ECU_CONFIG_CORE_COMPONENT_TYPE_ALL,
  ECU_CONFIG_CORE_COMPONENT_TYPE_MAX
}ecu_config_core_component_type_t;

typedef enum {
  ECU_CONFIG_CALIB_TYPE_ID = 0,
  ECU_CONFIG_CALIB_TYPE_IO,
  ECU_CONFIG_CALIB_TYPE_POWERMODING,
  ECU_CONFIG_CALIB_TYPE_CYLINDERS,
  ECU_CONFIG_CALIB_TYPE_CALCDATA,
  ECU_CONFIG_CALIB_TYPE_TABLES,
  ECU_CONFIG_CALIB_TYPE_ALL,
  ECU_CONFIG_CALIB_TYPE_MAX
}ecu_config_calibration_type_t;

typedef enum {
  ECU_CONFIG_RUNTIME_TYPE_CORRECTIONS = 0,
  ECU_CONFIG_RUNTIME_TYPE_ALL,
  ECU_CONFIG_RUNTIME_TYPE_MAX
}ecu_config_runtime_type_t;

typedef enum {
  ECU_CONFIG_COMM_TYPE_CAN = 0,
  ECU_CONFIG_COMM_TYPE_KWP,
  ECU_CONFIG_COMM_TYPE_ISOTP,
  ECU_CONFIG_COMM_TYPE_UDS,
  ECU_CONFIG_COMM_TYPE_OBD2,
  ECU_CONFIG_COMM_TYPE_ROUTER,
  ECU_CONFIG_COMM_TYPE_ALL,
  ECU_CONFIG_COMM_TYPE_MAX
}ecu_config_comm_type_t;

typedef enum {
  ECU_CONFIG_TYPE_DEVICE = 0,
  ECU_CONFIG_TYPE_SENSOR,
  ECU_CONFIG_TYPE_MODULE,
  ECU_CONFIG_TYPE_TIMING,
  ECU_CONFIG_TYPE_CALIBRATION,
  ECU_CONFIG_TYPE_RUNTIME,
  ECU_CONFIG_TYPE_COMM,
  ECU_CONFIG_TYPE_ALL,
  ECU_CONFIG_TYPE_MAX,
}ecu_config_type_t;

typedef enum {
  ECU_CONFIG_OP_NONE = 0,
  ECU_CONFIG_OP_READ,
  ECU_CONFIG_OP_WRITE,
  ECU_CONFIG_OP_MAX,
}ecu_config_op_t;

#define ECU_CONFIG_PARAMETERS_MAX ( \
    (ECU_DEVICES_PARAMETERS_MAX)  + \
    (ECU_SENSORS_PARAMETERS_MAX)  + \
    (ECU_MODULES_PARAMETERS_MAX)  + \
    (ECU_TIMINGS_PARAMETERS_MAX) )

typedef error_t (*ecu_config_translate_prev_to_this_func_t)(const void *src, void *dest, uint32_t dest_bytes);
typedef error_t (*ecu_config_get_default_cfg_func_t)(ecu_index_type_t index, void *config);
typedef error_t (*ecu_config_configure_func_t)(ecu_instance_t instance, const void *config);
typedef error_t (*ecu_config_reset_func_t)(ecu_instance_t instance);

typedef struct {
    uint32_t version;
    uint32_t size;
    ecu_config_translate_prev_to_this_func_t translate_func;
}ecu_config_item_version_t;

typedef struct {
    flash_section_type_t flash_section_type;
    ecu_config_get_default_cfg_func_t get_default_cfg_func;
    void *data_ptr;
    void *double_data_ptr;
    uint32_t data_size;
    uint32_t versions_count;
    ecu_config_item_version_t versions[ECU_CONFIG_ITEM_VERSIONS_MAX];
}ecu_config_generic_config_t;

typedef struct {
    flash_section_type_t flash_section_type;
    ecu_config_get_default_cfg_func_t get_default_cfg_func;
    uint16_t version_current;
    const ecu_config_item_version_t *versions;
    void *data_ptr;
    void *final_data_ptr;
    uint32_t data_size;
    uint32_t data_dma_datasize;
    uint32_t data_dma_limit;
    uint32_t data_dma_length;
    uint32_t data_dma_left;
    uint32_t data_dma_pos;
}ecu_op_req_ctx_t;

typedef struct {
    ecu_config_item_type_t device_type;
    uint32_t instances_count;
    ecu_config_configure_func_t configure_func;
    ecu_config_reset_func_t reset_func;
    ecu_config_generic_config_t generic;
}ecu_config_device_config_t;

typedef struct {
    const ecu_config_generic_config_t *config;
}ecu_config_generic_ctx_t;

typedef struct {
    const ecu_config_device_config_t *config;
    error_t reset_errcode;
    error_t config_errcode;
}ecu_config_executive_ctx_t;

typedef enum {
  ECU_CONFIG_FSM_DEVICE_CFG_CONDITION = 0,
  ECU_CONFIG_FSM_DEVICE_CFG_DEFINE,
  ECU_CONFIG_FSM_DEVICE_CFG_RESET,
  ECU_CONFIG_FSM_DEVICE_CFG_CONFIG,
  ECU_CONFIG_FSM_DEVICE_CFG_MAX
}ecu_config_global_device_cfg_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_SENSOR_CFG_CONDITION = 0,
  ECU_CONFIG_FSM_SENSOR_CFG_DEFINE,
  ECU_CONFIG_FSM_SENSOR_CFG_RESET,
  ECU_CONFIG_FSM_SENSOR_CFG_CONFIG,
  ECU_CONFIG_FSM_SENSOR_CFG_MAX
}ecu_config_global_sens_cfg_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_MODULE_CFG_CONDITION = 0,
  ECU_CONFIG_FSM_MODULE_CFG_DEFINE,
  ECU_CONFIG_FSM_MODULE_CFG_RESET,
  ECU_CONFIG_FSM_MODULE_CFG_CONFIG,
  ECU_CONFIG_FSM_MODULE_CFG_MAX
}ecu_config_global_module_cfg_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_TIMING_CFG_CONDITION = 0,
  ECU_CONFIG_FSM_TIMING_CFG_DEFINE,
  ECU_CONFIG_FSM_TIMING_CFG_RESET,
  ECU_CONFIG_FSM_TIMING_CFG_CONFIG,
  ECU_CONFIG_FSM_TIMING_CFG_MAX
}ecu_config_global_timing_cfg_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_COMM_CFG_CONDITION = 0,
  ECU_CONFIG_FSM_COMM_CFG_DEFINE,
  ECU_CONFIG_FSM_COMM_CFG_RESET,
  ECU_CONFIG_FSM_COMM_CFG_CONFIG,
  ECU_CONFIG_FSM_COMM_CFG_MAX
}ecu_config_global_comm_cfg_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_FLASH_CONDITION = 0,
  ECU_CONFIG_FSM_FLASH_DEFINE,
  ECU_CONFIG_FSM_FLASH_RESET,
  ECU_CONFIG_FSM_FLASH_MAX
}ecu_config_global_flash_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_FLASH_ERASE_CONDITION = 0,
  ECU_CONFIG_FSM_FLASH_ERASE_DEFINE,
  ECU_CONFIG_FSM_FLASH_ERASE_ACQUIRE,
  ECU_CONFIG_FSM_FLASH_ERASE_LOCK,
  ECU_CONFIG_FSM_FLASH_ERASE_PERFORM,
  ECU_CONFIG_FSM_FLASH_ERASE_SYNC,
  ECU_CONFIG_FSM_FLASH_ERASE_UNLOCK,
  ECU_CONFIG_FSM_FLASH_ERASE_MAX
}ecu_config_global_flash_erase_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_OPERATION_CONDITION = 0,
  ECU_CONFIG_FSM_OPERATION_LOCK,
  ECU_CONFIG_FSM_OPERATION_DEFINE,
  ECU_CONFIG_FSM_OPERATION_DOUBLEBUFFER_REQ,
  ECU_CONFIG_FSM_OPERATION_DOUBLEBUFFER_POLL,
  ECU_CONFIG_FSM_OPERATION_REQUEST,
  ECU_CONFIG_FSM_OPERATION_TRANSLATE_ACQUIRE,
  ECU_CONFIG_FSM_OPERATION_TRANSLATE_CHECK,
  ECU_CONFIG_FSM_OPERATION_TRANSLATE_LOCK,
  ECU_CONFIG_FSM_OPERATION_TRANSLATE_MAPPING_ENABLE,
  ECU_CONFIG_FSM_OPERATION_TRANSLATE,
  ECU_CONFIG_FSM_OPERATION_TRANSLATE_MAPPING_DISABLE,
  ECU_CONFIG_FSM_OPERATION_TRANSLATE_UNLOCK,
  ECU_CONFIG_FSM_OPERATION_TRANSLATE_WRITE,
  ECU_CONFIG_FSM_OPERATION_UNLOCK,
  ECU_CONFIG_FSM_OPERATION_MAX
}ecu_config_global_operation_fsm_t;

typedef enum {
  ECU_CONFIG_FSM_PROCESS_FLASH_INIT = 0,
  ECU_CONFIG_FSM_PROCESS_FLASH_ERASE,
  ECU_CONFIG_FSM_PROCESS_DEVICE_CFG,
  ECU_CONFIG_FSM_PROCESS_SENSOR_CFG,
  ECU_CONFIG_FSM_PROCESS_MODULE_CFG,
  ECU_CONFIG_FSM_PROCESS_TIMING_CFG,
  ECU_CONFIG_FSM_PROCESS_COMM_CFG,
  ECU_CONFIG_FSM_PROCESS_OPERATION,
  ECU_CONFIG_FSM_PROCESS_MAX
}ecu_config_global_process_fsm_t;

typedef enum {
  ECU_CONFIG_PROCESS_TYPE_NONE = 0,
  ECU_CONFIG_PROCESS_TYPE_FLASH_INIT,
  ECU_CONFIG_PROCESS_TYPE_DEVICE_INIT,
  ECU_CONFIG_PROCESS_TYPE_SENSOR_INIT,
  ECU_CONFIG_PROCESS_TYPE_MODULE_INIT,
  ECU_CONFIG_PROCESS_TYPE_TIMING_INIT,
  ECU_CONFIG_PROCESS_TYPE_COMM_INIT,
  ECU_CONFIG_PROCESS_TYPE_FLASH_ERASE,
  ECU_CONFIG_PROCESS_TYPE_MAX,
}ecu_config_global_process_type_t;

typedef struct {
    const ecu_config_device_config_t *flash_config;
    ecu_config_executive_ctx_t *flash_ctx;
    uint32_t devices_count;
    const ecu_config_device_config_t *devices_config;
    ecu_config_executive_ctx_t *devices_ctx;
    uint32_t sensors_count;
    const ecu_config_device_config_t *sensors_config;
    ecu_config_executive_ctx_t *sensors_ctx;
    uint32_t modules_count;
    const ecu_config_device_config_t *modules_config;
    ecu_config_executive_ctx_t *modules_ctx;
    uint32_t timings_count;
    const ecu_config_device_config_t *timings_config;
    ecu_config_executive_ctx_t *timings_ctx;
    uint32_t calibrations_count;
    const ecu_config_generic_config_t *calibrations_config;
    ecu_config_generic_ctx_t *calibrations_ctx;
    uint32_t runtimes_count;
    const ecu_config_generic_config_t *runtimes_config;
    ecu_config_generic_ctx_t *runtimes_ctx;
    uint32_t comms_count;
    const ecu_config_device_config_t *comms_config;
    ecu_config_executive_ctx_t *comms_ctx;
    bool global_ready;
    bool devices_initialized;
    bool flash_initialized;
    bool sensors_initialized;
    bool modules_initialized;
    bool timings_initialized;
    bool comms_initialized;
    bool core_components_initialized;

    DMA_HandleTypeDef *hdma;
    error_t dma_errode;

    ecu_config_op_t op_request;
    error_t op_req_errcode_internal;
    error_t op_req_errcode;
    ecu_config_type_t op_req_type;
    ecu_index_type_t op_req_index;
    ecu_instance_t op_req_instance;
    ecu_op_req_ctx_t op_req_ctx;
    qspi_ctx_t *qspi_ctx;

    ecu_config_type_t op_type_index;
    ecu_index_type_t op_index_max;
    ecu_index_type_t op_index;
    ecu_instance_t op_instance_max;
    ecu_instance_t op_instance;
    flash_payload_version_t op_version;
    uint32_t op_translate_ptr;

    ecu_config_global_flash_fsm_t fsm_flash;
    ecu_config_global_flash_erase_fsm_t fsm_flash_erase;
    ecu_config_global_device_cfg_fsm_t fsm_device_cfg;
    ecu_config_global_sens_cfg_fsm_t fsm_sensor_cfg;
    ecu_config_global_module_cfg_fsm_t fsm_module_cfg;
    ecu_config_global_timing_cfg_fsm_t fsm_timing_cfg;
    ecu_config_global_comm_cfg_fsm_t fsm_comm_cfg;
    ecu_config_global_operation_fsm_t fsm_operation;
    ecu_config_global_process_fsm_t fsm_process;

    ecu_config_global_process_type_t process_type;
    error_t process_result;
    ecu_config_device_type_t process_device_type;
    ecu_config_sensor_type_t process_sensor_type;
    ecu_config_module_type_t process_module_type;
    ecu_config_timing_type_t process_timing_type;
    ecu_config_comm_type_t process_comm_type;
    ecu_device_instance_t process_instance;
}ecu_config_global_runtime_ctx_t;

error_t ecu_config_global_init(void);
void ecu_config_global_loop_main(void);
void ecu_config_global_loop_slow(void);
void ecu_config_global_loop_fast(void);

error_t ecu_config_global_flash_initialize(void);
error_t ecu_config_global_devices_initialize(void);
error_t ecu_config_global_sensors_initialize(void);
error_t ecu_config_global_modules_initialize(void);
error_t ecu_config_global_timings_initialize(void);
error_t ecu_config_global_comm_initialize(void);

error_t ecu_config_global_flash_erase(void);

error_t ecu_config_global_operation(ecu_config_op_t op, ecu_config_type_t type, ecu_index_type_t index, ecu_instance_t instance);

#endif /* CONFIG_GLOBAL_INC_CONFIG_GLOBAL_H_ */
