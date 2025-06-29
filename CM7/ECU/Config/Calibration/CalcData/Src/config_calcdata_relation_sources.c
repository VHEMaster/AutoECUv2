/*
 * config_calcdata_relation_sources.c
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_calcdata_relation_input_varianted_items_t cfg_relation_sources = {
    .items = {
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_0,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_0,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS
        {
            .variations = 3,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_1,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_1,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_31,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
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
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
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
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
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
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
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
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
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
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
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
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
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
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_EGT
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_4,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_IAT
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_5,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_2,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAF
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_6,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_3,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAP
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_9,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_4,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_TPS
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_4,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_IAT_MANIFOLD
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_11,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_5,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_CHARGE
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_5,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_2,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_32,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V2
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //CALCDATA_RELATION_INPUT_VARIANTED_ITEM_V3
            },
        }, //CALCDATA_RELATION_INPUT_SOURCE_CALC_IDLE_TARGET_RPM_DEVIATION
    }
};

error_t ecu_calibration_calcdata_relation_sources_get_default_config(ecu_calibration_instance_t instance, ecu_config_calcdata_relation_input_varianted_items_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &cfg_relation_sources, sizeof(cfg_relation_sources));

  } while(0);

  return err;
}
