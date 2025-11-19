/*
 * config_injection.c
 *
 *  Created on: Jul 7, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_timings.h"
#include "versioned_timings.h"

typedef struct ecu_timings_injection_ctx_tag ecu_timings_injection_ctx_t;

typedef struct ecu_timings_injection_ctx_tag {
    injection_config_t config_default;
    injection_init_ctx_t init;
    injection_ctx_t *ctx;
}ecu_timings_injection_ctx_t;

static const injection_config_t ecu_timings_injection_config_default = {
    .signal_prepare_advance = 16.0f,
    .process_update_trigger = INJECTION_CONFIG_PROCESS_UPDATE_TRIGGER_ALWAYS,
    .uspd_source = INJECTION_CONFIG_USPD_SOURCE_PER_REVOLUTION,
    .power_voltage_pin = ECU_IN_PORT2_VIGN,
    .groups = {
        {
            .enabled = true,
            .process_update_trigger = INJECTION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS,
            .mode = INJECTION_CONFIG_GROUP_MODE_SEQUENTIAL_AND_SEMISEQUENTIAL,
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
            .performance_static_units = INJECTION_CONFIG_GROUP_PERF_UNITS_CC,
            .performance_static_fuel_pressure = 3.0f,

            .performance_static_semiseq_mul = 1.0f,
            .performance_static_semiseq_add = 0.0f,

            .performance_static_seq_mul = 1.0f,
            .performance_static_seq_add = 0.0f,

            .performance_fuel_mass_per_cc = 750.0f,

            .performance_fuelramp_nominal_pressure = 3.0f,
            .performance_fuel_pressure_manifold_source = INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_NONE,
            .performance_fuel_pressure_manifold_io_type = ECU_CONFIG_IO_MAP_MANIFOLD,
            .performance_fuel_pressure_fuelramp_source = INJECTION_CONFIG_GROUP_PERF_PRESSURE_SOURCE_NONE,
            .performance_fuel_pressure_fuelramp_io_type = ECU_CONFIG_IO_MAP_MANIFOLD,

            .performance_fuel_pressure_manifold_clamp_min = 0.0f,
            .performance_fuel_pressure_manifold_clamp_max = 5.0f,
            .performance_fuel_pressure_fuelramp_clamp_min = 0.0f,
            .performance_fuel_pressure_fuelramp_clamp_max = 5.0f,

            .phase_add = 0.0f,
            .phase_slew_rate = 24.0f,
            .phase_mode = INJECTION_CONFIG_GROUP_PHASE_MODE_ENDING,

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

            .enrichment_late_phase_mode = INJECTION_CONFIG_GROUP_LATE_PHASE_MODE_ENDING,
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
            .dutycycle_limit_mode = INJECTION_CONFIG_GROUP_DUTYCYCLE_LIMIT_MODE_CUTOFF,

        }, //INJECTION_CONFIG_GROUP_PRIMARY
    },
};

static const bool ecu_timings_injection_enabled_default[ECU_TIMING_INJECTION_MAX] = {
    true
};

static RAM_SECTION ecu_timings_injection_ctx_t ecu_timings_injection_ctx[ECU_TIMING_INJECTION_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_timings_injection_config_default,
    },
};

error_t ecu_timings_injection_init(ecu_timing_injection_t instance, injection_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_timings_injection_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_INJECTION_MAX || ctx == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_injection_ctx[instance];
    timing_ctx->ctx = ctx;

    timing_ctx->config_default.enabled = ecu_timings_injection_enabled_default[instance];

    err = ecu_config_global_get_engine_calibration_config(&timing_ctx->init.calibration_config);
    BREAK_IF_ACTION(err != E_OK, err = E_FAULT);
    BREAK_IF_ACTION(timing_ctx->init.calibration_config == NULL, err = E_FAULT);

    err = injection_init(timing_ctx->ctx, &timing_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&timing_ctx->ctx->config, &timing_ctx->config_default, sizeof(injection_config_t));

    err = ecu_timings_set_timing_enabled(ECU_TIMING_TYPE_INJECTION, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_timings_injection_get_default_config(ecu_timing_injection_t instance, injection_config_t *config)
{
  error_t err = E_OK;
  ecu_timings_injection_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_INJECTION_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_injection_ctx[instance];

    memcpy(config, &timing_ctx->config_default, sizeof(injection_config_t));

  } while(0);

  return err;
}

error_t ecu_timings_injection_configure(ecu_timing_injection_t instance, const injection_config_t *config)
{
  error_t err = E_OK;
  ecu_timings_injection_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_INJECTION_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_injection_ctx[instance];

    err = injection_configure(timing_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_timings_set_timing_enabled(ECU_TIMING_TYPE_INJECTION, instance, timing_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_timings_injection_reset(ecu_timing_injection_t instance)
{
  error_t err = E_OK;
  ecu_timings_injection_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_INJECTION_MAX, err = E_PARAM);

    timing_ctx = &ecu_timings_injection_ctx[instance];

    err = injection_reset(timing_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_timings_injection_get_runtime_data_ptr(ecu_timing_injection_t instance, injection_runtime_ctx_t **data)
{
  error_t err = E_OK;
  ecu_timings_injection_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_INJECTION_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_injection_ctx[instance];

    err = injection_get_runtime_data_ptr(timing_ctx->ctx, data);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
