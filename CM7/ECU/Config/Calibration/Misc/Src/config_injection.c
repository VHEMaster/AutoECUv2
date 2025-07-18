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
    .signal_prepare_advance = 16.0f,
    .process_update_trigger = ECU_CONFIG_INJECTION_PROCESS_UPDATE_TRIGGER_1OF2_1ST,
    .uspd_source = ECU_CONFIG_INJECTION_USPD_SOURCE_PER_REVOLUTION,
    .power_voltage_pin = ECU_IN_PORT2_VIGN,
    .groups = {
        {
            .enabled = true,
            .process_update_trigger = ECU_CONFIG_INJECTION_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS,
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
            /* Turbo 0280158235
            .voltage_to_performance_dynamic = {
                .items = 8,
                .input = {
                    0.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 25.0f
                },
                .output = {
                    8000, 3440, 1740, 1140, 800, 540, 300, 30
                },
            },
            .performance_static = 731.0f,
            */
            .voltage_to_performance_dynamic = {
                .items = 8,
                .input = {
                    0.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f, 25.0f
                },
                .output = {
                    5000, 3000, 2180, 1670, 1180, 910, 630, 30
                },
            },
            .performance_static = 523.0f,
            .performance_static_units = ECU_CONFIG_INJECTION_GROUP_PERF_UNITS_CC,
            .performance_static_fuel_pressure = 3.0f,

            .performance_static_semiseq_mul = 1.0f,
            .performance_static_semiseq_add = 0.0f,

            .performance_static_seq_mul = 1.0f,
            .performance_static_seq_add = 0.0f,

            .performance_fuel_mass_per_cc = 750.0f,

            .performance_fuelramp_nominal_pressure = 3.0f,
            .performance_fuel_pressure_manifold_source = ECU_CONFIG_INJECTION_GROUP_PERF_PRESSURE_SOURCE_NONE,
            .performance_fuel_pressure_manifold_io_type = ECU_CONFIG_IO_MAP_MANIFOLD,
            .performance_fuel_pressure_fuelramp_source = ECU_CONFIG_INJECTION_GROUP_PERF_PRESSURE_SOURCE_NONE,
            .performance_fuel_pressure_fuelramp_io_type = ECU_CONFIG_IO_MAP_MANIFOLD,

            .performance_fuel_pressure_manifold_clamp_min = 0.0f,
            .performance_fuel_pressure_manifold_clamp_max = 5.0f,
            .performance_fuel_pressure_fuelramp_clamp_min = 0.0f,
            .performance_fuel_pressure_fuelramp_clamp_max = 5.0f,

            .phase_add = 0.0f,
            .phase_slew_rate = 24.0f,
            .phase_mode = ECU_CONFIG_INJECTION_GROUP_PHASE_MODE_ENDING,

            .rpm_to_phase_add = {
                .items = 16,
                .input = {
                    600, 740, 870, 1050, 1250, 1490, 1800, 2150,
                    2560, 3040, 3590, 4310, 5100, 6060, 7190, 8500
                },
                .output = {
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                },
            },

            .enrichment_late_phase_mode = ECU_CONFIG_INJECTION_GROUP_LATE_PHASE_MODE_ENDING,
            .enrichment_late_phase = {
                .items = 16,
                .input = {
                    600, 740, 870, 1050, 1250, 1490, 1800, 2150,
                    2560, 3040, 3590, 4310, 5100, 6060, 7190, 8500
                },
                .output = {
                    540, 540, 540, 540, 540, 540, 540, 540,
                    540, 540, 540, 540, 540, 540, 540, 540
                },
            },

            .inject_mass_low_threshold = 0.0f,
            .inject_mass_reduction = 0.0f,
            .dutycycle_warning = 0.8f,
            .dutycycle_limit = 0.9f,
            .dutycycle_limit_mode = ECU_CONFIG_INJECTION_GROUP_DUTYCYCLE_LIMIT_MODE_CUTOFF,

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
