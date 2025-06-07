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
            .data_failsafe = { .value = 0.0f },
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
            .data_failsafe = { .value = 10.0f },
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
            .data_failsafe = { .value = 12.0f },
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
            .data_failsafe = { .value = 360.0f },
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
        {
            .data_failsafe = { .value = 1.0f },
            .variations = 2,
            .variants = {
                {
                    .type = CALCDATA_OUTPUT_TYPE_2D,
                    .data_2d = {
                        .table_index = ECU_CONFIG_TABLE_2D_16X16_2,
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
                    .type = CALCDATA_OUTPUT_TYPE_1D,
                    .data_1d = {
                        .table_index = ECU_CONFIG_TABLE_1D_16_13,
                        .source_x = {
                            .source = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAF,
                            .variant = CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1,
                        },
                    },
                }, //CALCDATA_OUTPUT_VARIANTED_ITEM_V2
            },
        }, //CALCDATA_OUTPUT_IAT_ALPHA_BLENDING
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
