/*
 * config_injection.c
 *
 *  Created on: Jul 7, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_injection_t ecu_calibration_injection_config_default = {
    .power_voltage_pin = ECU_IN_PORT2_VIGN,
    .groups = {
        {
            .enabled = true,
            .mode = ECU_CONFIG_INJECTION_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL,
            .cylinders = {
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_PORT1_PIN9,

                    .performance_static_mul = 1.0f,
                    .performance_static_add = 0.0f,

                    .performance_dynamic_mul = 1.0f,
                    .performance_dynamic_add = 0.0f,

                    .phase_add = 0.0f,
                }, //ECU_CYLINDER_1
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_PORT1_PIN10,

                    .performance_static_mul = 1.0f,
                    .performance_static_add = 0.0f,

                    .performance_dynamic_mul = 1.0f,
                    .performance_dynamic_add = 0.0f,

                    .phase_add = 0.0f,
                }, //ECU_CYLINDER_2
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_PORT1_PIN11,

                    .performance_static_mul = 1.0f,
                    .performance_static_add = 0.0f,

                    .performance_dynamic_mul = 1.0f,
                    .performance_dynamic_add = 0.0f,

                    .phase_add = 0.0f,
                }, //ECU_CYLINDER_3
                {
                    .disabled = false,
                    .output_pin = ECU_OUT_PORT1_PIN12,

                    .performance_static_mul = 1.0f,
                    .performance_static_add = 0.0f,

                    .performance_dynamic_mul = 1.0f,
                    .performance_dynamic_add = 0.0f,

                    .phase_add = 0.0f,
                }, //ECU_CYLINDER_4
            },
            .voltage_to_performance_dynamic = {
                .items = 8,
                .input = {
                    0.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 25.0f
                },
                .output = {
                    5000, 3000, 2180, 1670, 1180, 910, 630, 30
                },
            },

            .performance_fuel_pressure = 3.0f,
            .performance_static = 523.0f,

            .phase_add = 0.0f,
            .phase_slew_rate = 6.0f,

        }, //ECU_CONFIG_INJECTION_GROUP_PRIMARY
    },
};

error_t ecu_calibration_injection_get_default_config(ecu_calibration_instance_t instance, ecu_config_injection_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_calibration_injection_config_default, sizeof(ecu_config_injection_t));

  } while(0);

  return err;
}
