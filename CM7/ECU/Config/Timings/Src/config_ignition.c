/*
 * config_ignition.c
 *
 *  Created on: Jul 7, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_timings.h"
#include "versioned_timings.h"

typedef struct ecu_timings_ignition_ctx_tag ecu_timings_ignition_ctx_t;

typedef struct ecu_timings_ignition_ctx_tag {
    ignition_config_t config_default;
    ignition_init_ctx_t init;
    ignition_ctx_t *ctx;
}ecu_timings_ignition_ctx_t;

static const ignition_config_t ecu_timings_ignition_config_default = {
    .signal_prepare_advance = 16.0f,
    .process_update_trigger = IGNITION_CONFIG_PROCESS_UPDATE_TRIGGER_1OF2_2ND,
    .uspd_source = IGNITION_CONFIG_USPD_SOURCE_PER_REVOLUTION,

    .power_voltage_pin = ECU_IN_PORT2_VIGN,
    .use_ignition_acceptance = true,
    .groups = {
        {
            .enabled = true,
            .process_update_trigger = IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS,
            .mode = IGNITION_CONFIG_GROUP_MODE_SEQUENTIAL_ONLY,
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
            .advance_slew_rate_earlier = 50.0f,
            .advance_slew_rate_later = 1000.0f,
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
        }, //IGNITION_CONFIG_GROUP_PRIMARY
        {
            .enabled = true,
            .process_update_trigger = IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS,
            .mode = IGNITION_CONFIG_GROUP_MODE_SEMISEQUENTIAL_ONLY,
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
            .advance_slew_rate_earlier = 50.0f,
            .advance_slew_rate_later = 1000.0f,
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
        }, //IGNITION_CONFIG_GROUP_SECONDARY
        {
            .enabled = true,
            .process_update_trigger = IGNITION_CONFIG_GROUP_PROCESS_UPDATE_TRIGGER_ALWAYS,
            .mode = IGNITION_CONFIG_GROUP_MODE_SEMISEQUENTIAL_ONLY,
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
            .advance_slew_rate_earlier = 50.0f,
            .advance_slew_rate_later = 1000.0f,
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
        }, //IGNITION_CONFIG_GROUP_TERTINARY
    },
};

static const bool ecu_timings_ignition_enabled_default[ECU_TIMING_IGNITION_MAX] = {
    true
};

static RAM_SECTION ecu_timings_ignition_ctx_t ecu_timings_ignition_ctx[ECU_TIMING_IGNITION_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_timings_ignition_config_default,
    },
};

error_t ecu_timings_ignition_init(ecu_timing_ignition_t instance, ignition_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_timings_ignition_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_IGNITION_MAX || ctx == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_ignition_ctx[instance];
    timing_ctx->ctx = ctx;

    timing_ctx->config_default.enabled = ecu_timings_ignition_enabled_default[instance];

    err = ignition_init(timing_ctx->ctx, &timing_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&timing_ctx->ctx->config, &timing_ctx->config_default, sizeof(ignition_config_t));

    err = ecu_timings_set_timing_enabled(ECU_TIMING_TYPE_IGNITION, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_timings_ignition_get_default_config(ecu_timing_ignition_t instance, ignition_config_t *config)
{
  error_t err = E_OK;
  ecu_timings_ignition_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_IGNITION_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_ignition_ctx[instance];

    memcpy(config, &timing_ctx->config_default, sizeof(ignition_config_t));

  } while(0);

  return err;
}

error_t ecu_timings_ignition_configure(ecu_timing_ignition_t instance, const ignition_config_t *config)
{
  error_t err = E_OK;
  ecu_timings_ignition_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_IGNITION_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_ignition_ctx[instance];

    err = ignition_configure(timing_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_timings_set_timing_enabled(ECU_TIMING_TYPE_IGNITION, instance, timing_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_timings_ignition_reset(ecu_timing_ignition_t instance)
{
  error_t err = E_OK;
  ecu_timings_ignition_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_IGNITION_MAX, err = E_PARAM);

    timing_ctx = &ecu_timings_ignition_ctx[instance];

    err = ignition_reset(timing_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_timings_ignition_get_runtime_data_ptr(ecu_timing_ignition_t instance, ignition_runtime_ctx_t **data)
{
  error_t err = E_OK;
  ecu_timings_ignition_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_TIMING_IGNITION_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    timing_ctx = &ecu_timings_ignition_ctx[instance];

    err = ignition_get_runtime_data_ptr(timing_ctx->ctx, data);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
