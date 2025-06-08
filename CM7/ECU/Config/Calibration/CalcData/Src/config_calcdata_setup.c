/*
 * config_calcdata_output_data.c
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_calcdata_setup_t cfg_setup = {
    .iat_model = {
        .sensors = {
            {
                .alpha_blending = {
                    .calc_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V2,
                    .filters = {
                        {
                            .volume = 1.0f,
                            .low_pass = 0.9f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_1
                        {
                            .volume = 0.0f,
                            .low_pass = 1.0f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_2
                    },
                },
            }, //ECU_CONFIG_IO_IAT_MANIFOLD
            {
                .alpha_blending = {
                    .calc_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V2,
                    .filters = {
                        {
                            .volume = 0.9f,
                            .low_pass = 0.9f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_1
                        {
                            .volume = 0.1f,
                            .low_pass = 0.5f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_2
                    },
                },
            }, //ECU_CONFIG_IO_IAT_THROTTLE
            {
                .alpha_blending = {
                    .calc_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V2,
                    .filters = {
                        {
                            .volume = 0.2f,
                            .low_pass = 0.9f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_1
                        {
                            .volume = 0.9f,
                            .low_pass = 0.1f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_2
                    },
                },
            }, //ECU_CONFIG_IO_IAT_INLET
        },
    },
};

error_t ecu_calibration_calcdata_setup_get_default_config(ecu_calibration_instance_t instance, ecu_config_calcdata_setup_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &cfg_setup, sizeof(cfg_setup));

  } while(0);

  return err;
}
