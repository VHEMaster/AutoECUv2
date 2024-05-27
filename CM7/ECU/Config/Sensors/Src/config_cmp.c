/*
 * config_cmp.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_cmp.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    cmp_config_t config_default;
    cmp_init_ctx_t init;
    cmp_ctx_t *ctx;
}ecu_sensors_cmp_ctx_t;

static const cmp_config_t ecu_sensors_cmp_config_default = {
    .desync_on_error = true,
    .signal_ref_type = CMP_CONFIG_SIGNAL_REF_TYPE_SINGLEPULSE,
    .boot_time = 100 * TIME_US_IN_MS,
    .signal_ref_types_config = {
        .singlepulse = {

        },
    },
};

static const bool ecu_sensors_cmp_enabled_default[ECU_SENSOR_CMP_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_sensors_cmp_input_pin_default[ECU_SENSOR_CMP_MAX] = {
    ECU_IN_PORT2_SENT2,
};

static ecu_sensors_cmp_ctx_t ecu_sensors_cmp_ctx[ECU_SENSOR_CMP_MAX] = {
    {
      .init = {
          .instance_index = CMP_INSTANCE_1,
      },
      .config_default = ecu_sensors_cmp_config_default,
    },
};

error_t ecu_sensors_cmp_init(ecu_sensor_cmp_t instance, cmp_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX || ctx == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];
    cmp_ctx->ctx = ctx;

    cmp_ctx->config_default.enabled = ecu_sensors_cmp_enabled_default[instance];
    cmp_ctx->config_default.input_pin = ecu_sensors_cmp_input_pin_default[instance];

    err = cmp_init(cmp_ctx->ctx, &cmp_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&cmp_ctx->ctx->config, &cmp_ctx->config_default, sizeof(cmp_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_get_default_config(ecu_sensor_cmp_t instance, cmp_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX || config == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    memcpy(config, &cmp_ctx->config_default, sizeof(cmp_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_configure(ecu_sensor_cmp_t instance, const cmp_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX || config == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = cmp_configure(cmp_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_reset(ecu_sensor_cmp_t instance)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = cmp_reset(cmp_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_get_value(ecu_sensor_cmp_t instance, cmp_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = cmp_get_value(cmp_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_cmp_get_diag(ecu_sensor_cmp_t instance, cmp_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_cmp_ctx_t *cmp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_CMP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    cmp_ctx = &ecu_sensors_cmp_ctx[instance];

    err = cmp_get_diag(cmp_ctx->ctx, diag);

  } while(0);

  return err;
}
