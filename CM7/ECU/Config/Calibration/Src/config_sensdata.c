/*
 * config_sensdata.c
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_sensdata_t ecu_sensdata_config_default = {
    .sources = {
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_0,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_0,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_APS
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_1,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_1,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_CKP
        {
            .variations = 0,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_CMP
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_2,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_ECT
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_3,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_EGT
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_4,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_IAT
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_5,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_2,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_MAF
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_6,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_3,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_MAP
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_7,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_OPS
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_8,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_OTS
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_9,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_4,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_TPS
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_10,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_SENSOR_VSS
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_11,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_5,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_CALC_CYCLE_FILLING
        {
            .variations = 2,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_5,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_1D_32_2,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_CALC_MASS_AIR_FLOW
        {
            .variations = 1,
            .variants = {
                {
                    .table_index = ECU_CONFIG_TABLE_1D_16_12,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V1
                {
                    .table_index = ECU_CONFIG_TABLE_NONE,
                }, //SENSDATA_RELATION_INPUT_VARIANTED_ITEM_V2
            },
        }, //SENSDATA_RELATION_INPUT_SOURCE_CALC_ENGINE_LOAD
    },
};

error_t ecu_calibration_sensdata_get_default_config(ecu_calibration_instance_t instance, ecu_config_sensdata_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_sensdata_config_default, sizeof(ecu_config_sensdata_t));

  } while(0);

  return err;
}
