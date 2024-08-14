/*
 * config_ignition.c
 *
 *  Created on: Jul 7, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_ignition_t ecu_calibration_ignition_config_default = {
    .signal_prepare_advance = 16.0f,
    .uspd_source = ECU_CONFIG_IGNITION_USPD_SOURCE_PER_REVOLUTION,

    .power_voltage_pin = ECU_IN_PORT2_VIGN,
    .groups = {
        {
            .enabled = true,
            .mode = ECU_CONFIG_IGNITION_GROUP_MODE_SEQUENTIAL_ONLY,
            .cylinders = {
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN1,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_1
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN2,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_2
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN3,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_3
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN4,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_4
            },
            .advance_add = 0.0f,
            .voltage_to_saturation_time = {
                .items = 8,
                .input = {
                    0.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 25.0f
                },
                .output = {
                    5700, 4500, 3200, 2500, 2100, 1800, 1600, 1000
                },
            },
            .rpm_to_saturation_mult = {
                .items = 16,
                .input = {
                    600, 740, 870, 1050, 1250, 1490, 1800, 2150,
                    2560, 3040, 3590, 4310, 5100, 6060, 7190, 8500
                },
                .output = {
                    4.0f, 1.7f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
                },
            },
            .distributor_dutycycle = 0.5f,
        }, //ECU_CONFIG_IGNITION_GROUP_PRIMARY
        {
            .enabled = true,
            .mode = ECU_CONFIG_IGNITION_GROUP_MODE_SEMISEQUENTIAL_ONLY,
            .cylinders = {
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN5,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_1
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN6,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_2
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN6,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_3
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN5,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_4
            },
            .advance_add = 0.0f,
            .voltage_to_saturation_time = {
                .items = 8,
                .input = {
                    0.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 25.0f
                },
                .output = {
                    5700, 4500, 3200, 2500, 2100, 1800, 1600, 1000
                },
            },
            .rpm_to_saturation_mult = {
                .items = 16,
                .input = {
                    600, 740, 870, 1050, 1250, 1490, 1800, 2150,
                    2560, 3040, 3590, 4310, 5100, 6060, 7190, 8500
                },
                .output = {
                    4.0f, 1.7f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
                },
            },
            .distributor_dutycycle = 0.5f,
        }, //ECU_CONFIG_IGNITION_GROUP_SECONDARY
        {
            .enabled = true,
            .mode = ECU_CONFIG_IGNITION_GROUP_MODE_SEMISEQUENTIAL_ONLY,
            .cylinders = {
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN7,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_1
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN8,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_2
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN8,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_3
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_IGN7,
                    .advance_add = 0.0f,
                }, //ECU_CYLINDER_4
            },
            .advance_add = 0.0f,
            .voltage_to_saturation_time = {
                .items = 8,
                .input = {
                    0.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 25.0f
                },
                .output = {
                    5700, 4500, 3200, 2500, 2100, 1800, 1600, 1000
                },
            },
            .rpm_to_saturation_mult = {
                .items = 16,
                .input = {
                    600, 740, 870, 1050, 1250, 1490, 1800, 2150,
                    2560, 3040, 3590, 4310, 5100, 6060, 7190, 8500
                },
                .output = {
                    4.0f, 1.7f, 1.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
                },
            },
            .distributor_dutycycle = 0.5f,
        }, //ECU_CONFIG_IGNITION_GROUP_TERTINARY
    },
};

error_t ecu_calibration_ignition_get_default_config(ecu_calibration_instance_t instance, ecu_config_ignition_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_calibration_ignition_config_default, sizeof(ecu_config_ignition_t));

  } while(0);

  return err;
}
