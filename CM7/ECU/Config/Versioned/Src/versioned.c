/*
 * versioned.c
 *
 *  Created on: May 1, 2024
 *      Author: VHEMaster
 */

#include "versioned.h"

static const ecu_config_components_versions_t ecu_config_components_versions = {
    .components = {
        {
            .versions_count = FLEXIO_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = FLEXIO_CONFIG_VERSION_V1,
                    .size = sizeof(l9966_config_data_v1_t),
                    .translate_func = NULL,
                }
            },
        }, //ECU_CONFIG_COMPONENT_TYPE_FLEXIO
        {
            .versions_count = MOTOR_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = MOTOR_CONFIG_VERSION_V1,
                    .size = sizeof(l9960_config_v1_t),
                    .translate_func = NULL,
                }
            },
        }, //ECU_CONFIG_COMPONENT_TYPE_MOTOR
        {
            .versions_count = WBLS_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = WBLS_CONFIG_VERSION_V1,
                    .size = sizeof(cj125_config_v1_t),
                    .translate_func = NULL,
                }
            },
        }, //ECU_CONFIG_COMPONENT_TYPE_WBLS
    },
};
