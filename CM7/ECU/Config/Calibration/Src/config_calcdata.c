/*
 * config_calcdata.c
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_calcdata_t ecu_calcdata_config_default = {
    .relation_sources = {
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_0,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_0,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_1,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_1,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_2,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_7,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_OPS
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_8,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_OTS
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_10,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_VSS
        {
            .variations = 0,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_CMP_IN
        {
            .variations = 0,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_CMP_EX
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_3,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_EGT
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_5,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_2,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAF
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_9,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_4,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_TPS
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_11,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_5,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_FILLING
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_5,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_2,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_12,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_ENGINE_LOAD
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_4,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_IAT_MANIFOLD
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_6,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_3,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_MAP_MANIFOLD
    },
    .output_data = {
        {
            .variations = 2,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .table_index = ECU_CONFIG_TABLE_2D_32X32_0,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_MAP_MANIFOLD,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
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
        }, //CALCDATA_OUTPUT_RELATIVE_FILLING
        {
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .table_index = ECU_CONFIG_TABLE_2D_32X32_2,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_FILLING,
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
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .table_index = ECU_CONFIG_TABLE_2D_16X16_0,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_FILLING,
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
            .variations = 1,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .table_index = ECU_CONFIG_TABLE_2D_16X16_1,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                        .source_y = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_FILLING,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V1
                {
                    .type = CALCDATA_OUTPUT_TYPE_NONE,
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_INJECTION_PHASE
    },
};

error_t ecu_calibration_calcdata_get_default_config(ecu_calibration_instance_t instance, ecu_config_calcdata_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_calcdata_config_default, sizeof(ecu_config_calcdata_t));

  } while(0);

  return err;
}
