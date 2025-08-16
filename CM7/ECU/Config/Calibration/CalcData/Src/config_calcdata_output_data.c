/*
 * config_calcdata_output_data.c
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_calcdata_output_data_varianted_items_t cfg_output_data = {
    .items = {
        {
            .data_failsafe = { .value = 1.0f },
            .variations = 2,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_2D_16X16_2,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_12,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IAT_ALPHA_BLENDING
        {
            .data_failsafe = { .value = 1.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_13,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IAT_MANIFOLD,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IAT_DIRECT_INFLUENCE
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 2,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_2D_32X32_0,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_2D_32X32_1,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_TPS,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_VOLUMETRIC_EFFICIENCY
        {
            .data_failsafe = { .value = 10.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_2D_32X32_2,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IGNITION_ADVANCE
        {
            .data_failsafe = { .value = 12.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_2D_16X16_0,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_AIR_TO_FUEL_RATIO
        {
            .data_failsafe = { .value = 360.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_2D_16X16_1,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_INJECTION_PHASE
        {
            .data_failsafe = { .value = 10.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_14,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_IGNITION_ADVANCE
        {
            .data_failsafe = { .value = 100.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_15,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_INJECTION_PHASE
        {
            .data_failsafe = { .value = 1.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_16,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_POS_INJ_INFLUENCE
        {
            .data_failsafe = { .value = 130.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_17,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_LARGE_INJ_CHARGE
        {
            .data_failsafe = { .value = 20.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_18,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_SMALL_INJ_CHARGE
        {
            .data_failsafe = { .value = 0.5f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_19,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_RUNUP_INJ_DURATION
        {
            .data_failsafe = { .value = 0.5f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_20,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_DURATION
        {
            .data_failsafe = { .value = 0.5f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_55,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_RUNUP_IGN_ADVANCE
        {
            .data_failsafe = { .value = 1.20f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_21,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_WARMUP_COLD_INJ_CORR
        {
            .data_failsafe = { .value = 1.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_22,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_COLD_INJ_TIME
        {
            .data_failsafe = { .value = 5.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_23,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STARTUP_ECON_BYPASS_TIME
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_24,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_WARMUP_IDLE_INJ_CORR
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_25,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_L
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_26,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_RPM_THR_FLAG_H
        {
            .data_failsafe = { .value = 1000.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_27,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_TARGET_RPM
        {
            .data_failsafe = { .value = 10.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_28,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_TARGET_IGNITION_ADVANCE
        {
            .data_failsafe = { .value = 20.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_29,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_TARGET_MASS_AIR_FLOW
        {
            .data_failsafe = { .value = 10.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = false,
                        .table_index = ECU_CONFIG_TABLE_1D_16_30,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_INITIAL_IGNITION_ADVANCE
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_54,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_TPS_IGNITION_CTRL
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_33,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_P
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_34,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_I
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_35,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_RPM_D
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_36,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_P
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_37,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_I
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_38,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_STEPPER_PID_MAF_D
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_39,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_P
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_40,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_I
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_41,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_ETC_PID_RPM_D
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_42,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_P
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_43,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_I
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_44,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_REG_ETC_PID_MAF_D
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_45,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_P
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_46,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_I
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_47,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IDLE_IGN_ADV_PID_RPM_D
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_48,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_ETC_STOPPED_POS
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_49,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_ETC_STARTUP_POS
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_50,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_ETC_IDLE_TARGET_POS
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 2,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_51,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .table_index = ECU_CONFIG_TABLE_2D_16X16_3,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_ETC_RUNNING_POS
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_52,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_ETC_RUNUP_MOVE_TIME
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_53,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_ETC_APS_IGNITION_CTRL
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_56,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STEPPER_IDLEVALVE_STARTUP_POS
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_57,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STEPPER_IDLEVALVE_IDLE_TARGET_POS
        {
            .data_failsafe = { .value = 0.0f },
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .interpolation_limit = true,
                        .table_index = ECU_CONFIG_TABLE_1D_16_58,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_STEPPER_IDLEVALVE_RUNUP_MOVE_TIME
    }
};

error_t ecu_calibration_calcdata_output_data_get_default_config(ecu_calibration_instance_t instance, ecu_config_calcdata_output_data_varianted_items_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &cfg_output_data, sizeof(cfg_output_data));

  } while(0);

  return err;
}
