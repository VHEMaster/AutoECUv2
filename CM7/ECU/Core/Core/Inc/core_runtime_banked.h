/*
 * core_runtime_banked.h
 *
 *  Created on: Jun 5, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_RUNTIME_BANKED_H_
#define CORE_CORE_INC_CORE_RUNTIME_BANKED_H_

#include "common.h"
#include "config_engine.h"
#include "interpolation.h"

#include "core_runtime_parameters.h"

typedef struct {
    const ecu_core_runtime_value_ctx_t *sensors_aps[ECU_CONFIG_IO_APS_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_ect[ECU_CONFIG_IO_ECT_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_ckp[ECU_CONFIG_IO_CMP_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_ops[ECU_CONFIG_IO_OPS_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_ots[ECU_CONFIG_IO_OTS_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_vss[ECU_CONFIG_IO_VSS_MAX];

    const ecu_core_runtime_global_parameters_module_coolingfan_ctx_t *modules_coolingfan[ECU_CONFIG_IO_COOLINGFAN_MAX];
    const ecu_core_runtime_global_parameters_module_ignpower_ctx_t *modules_ignpower[ECU_CONFIG_IO_IGNPOWER_MAX];
    const ecu_core_runtime_global_parameters_module_indication_ctx_t *modules_indication[ECU_CONFIG_IO_INDICATION_MAX];
    const ecu_core_runtime_global_parameters_module_timing_ctx_t *modules_timing[ECU_CONFIG_IO_TIMING_MAX];
}ecu_core_runtime_banked_global_parameters_ctx_t;

typedef struct {
    const ecu_core_runtime_value_ctx_t *sensors_cmp[ECU_CONFIG_IO_CMP_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_egt[ECU_CONFIG_IO_EGT_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_iat[ECU_CONFIG_IO_IAT_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_maf[ECU_CONFIG_IO_MAF_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_map[ECU_CONFIG_IO_MAP_MAX];
    const ecu_core_runtime_value_ctx_t *sensors_tps[ECU_CONFIG_IO_TPS_MAX];

    const ecu_core_runtime_global_parameters_device_stepper_ctx_t *devices_stepper[ECU_CONFIG_IO_STEPPER_MAX];
    const ecu_core_runtime_global_parameters_device_wbls_ctx_t *devices_wbls[ECU_CONFIG_IO_WBLS_MAX];

    const ecu_core_runtime_global_parameters_module_etc_ctx_t *modules_etc[ECU_CONFIG_IO_ETC_MAX];
    const ecu_core_runtime_global_parameters_module_vvt_ctx_t *modules_vvt[ECU_CONFIG_IO_VVT_MAX];
    const ecu_core_runtime_global_parameters_module_wgcv_ctx_t *modules_wgcv[ECU_CONFIG_IO_WGCV_MAX];

}ecu_core_runtime_banked_bank_parameters_ctx_t;

typedef struct {
    ecu_core_runtime_banked_global_parameters_ctx_t global;
    ecu_core_runtime_banked_bank_parameters_ctx_t banks[ECU_BANK_MAX];
}ecu_core_runtime_banked_raw_ctx_t;

typedef struct {
    sMathInterpolateInput ctx;
    bool valid;
}ecu_core_runtime_interpolation_ctx_t;

typedef struct {
    ecu_core_runtime_value_ctx_t value;
    ecu_core_runtime_interpolation_ctx_t interpolation;
}ecu_core_runtime_banked_source_bank_input_ctx_t;

typedef struct {
    ecu_core_runtime_value_ctx_t variants[CALCDATA_OUTPUT_VARIANTED_ITEM_MAX];
}ecu_core_runtime_banked_source_bank_output_ctx_t;

typedef struct {
    float filtered;
    float unfiltered;
}ecu_core_runtime_banked_source_bank_iat_blending_ctx_t;

typedef struct {
    bool blending_valid;
    ecu_core_runtime_banked_source_bank_iat_blending_ctx_t blending[CALCDATA_IAT_ALPHA_BLENDING_FILTER_MAX];
}ecu_core_runtime_banked_source_bank_iat_ctx_t;

typedef struct {
    ecu_core_runtime_banked_source_bank_input_ctx_t inputs[CALCDATA_RELATION_INPUT_SOURCE_MAX];
    ecu_core_runtime_banked_source_bank_output_ctx_t outputs[CALCDATA_OUTPUT_MAX];

    ecu_core_runtime_banked_source_bank_iat_ctx_t data_iat;
}ecu_core_runtime_banked_source_bank_ctx_t;

typedef struct {
    ecu_core_runtime_banked_source_bank_ctx_t banks[ECU_BANK_MAX];
}ecu_core_runtime_banked_source_ctx_t;

typedef struct {
    ecu_core_runtime_banked_raw_ctx_t raw;
    ecu_core_runtime_banked_source_ctx_t source;
}ecu_core_runtime_banked_ctx_t;

#endif /* CORE_CORE_INC_CORE_RUNTIME_BANKED_H_ */
