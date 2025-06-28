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
    .ect_model = {
        .calc_model = CALCDATA_ECT_CALC_MODEL_MAXIMAL,
        .rms_min = -20.0f,
    },
    .iat_model = {
        .sensors = {
            {
                .alpha_blending = {
                    .calc_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1,
                    .filters = {
                        {
                            .volume = 1.0f,
                            .low_pass = 0.10f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_1
                        {
                            .volume = 0.0f,
                            .low_pass = 1.00f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_2
                    },
                },
            }, //ECU_CONFIG_IO_IAT_MANIFOLD
            {
                .alpha_blending = {
                    .calc_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1,
                    .filters = {
                        {
                            .volume = 0.7f,
                            .low_pass = 0.10f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_1
                        {
                            .volume = 0.3f,
                            .low_pass = 0.05f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_2
                    },
                },
            }, //ECU_CONFIG_IO_IAT_THROTTLE
            {
                .alpha_blending = {
                    .calc_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1,
                    .filters = {
                        {
                            .volume = 0.3f,
                            .low_pass = 0.10f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_1
                        {
                            .volume = 0.7f,
                            .low_pass = 0.02f,
                        }, //CALCDATA_IAT_ALPHA_BLENDING_FILTER_2
                    },
                },
            }, //ECU_CONFIG_IO_IAT_INLET
        },
    },
    .air_calc_model = {
        .normal_conditions = {
            .air_temperature = 20.0f,
            .absolute_pressure = 1.01325f,
        },
        .sources = {
            {
                .model = CALCDATA_AIR_CALC_MODEL_SOURCE_MAP,
                .ve_out_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1,
                .iat_influence = {
                    .direct_type = CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_TABLE,
                    .direct_table_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1,
                },
            }, //CALCDATA_AIR_CALC_MODEL_INDEX_1
            {
                .model = CALCDATA_AIR_CALC_MODEL_SOURCE_TPS,
                .ve_out_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V2,
                .iat_influence = {
                    .direct_type = CALCDATA_IAT_DIRECT_INFLUENCE_TYPE_TABLE,
                    .direct_table_variant = CALCDATA_OUTPUT_VARIANTED_ITEM_V1,
                },
            }, //CALCDATA_AIR_CALC_MODEL_INDEX_2
            {
                .model = CALCDATA_AIR_CALC_MODEL_SOURCE_NONE,
            }, //CALCDATA_AIR_CALC_MODEL_INDEX_3
            {
                .model = CALCDATA_AIR_CALC_MODEL_SOURCE_NONE,
            }, //CALCDATA_AIR_CALC_MODEL_INDEX_4
        },
    },
    .idle = {
        .pos_flag_mode = CALCDATA_IDLE_POS_FLAG_MODE_APS,
        .pos_flag_thr_l = 0.1f,
        .pos_flag_thr_h = 0.1f,
    },
    .runup_rpm_threshold_low = 400.0f,
    .runup_rpm_threshold_high = 600.0f,
    .startup_large_revs = 4,
    .startup_large_to_small_trans_revs = 8,
    .startup_transition_reset = 32,
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
