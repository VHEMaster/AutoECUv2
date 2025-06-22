/*
 * config_vss.c
 *
 *  Created on: Jun 2, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_vss.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    vss_config_t config_default;
    vss_init_ctx_t init;
    vss_ctx_t *ctx;
}ecu_sensors_vss_ctx_t;

static const vss_config_t ecu_sensors_vss_config_default = {
    .pulses_per_meter = 6.0f,
    .input_correction = 1.00f,
    .speed_max = 300.0f,
};

static const bool ecu_sensors_vss_enabled_default[ECU_SENSOR_VSS_MAX] = {
    true,
};

static const ecu_gpio_input_pin_t ecu_sensors_vss_input_pin_default[ECU_SENSOR_VSS_MAX] = {
    ECU_IN_PORT2_SENT1,
};

static RAM_SECTION ecu_sensors_vss_ctx_t ecu_sensors_vss_ctx[ECU_SENSOR_VSS_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_vss_config_default,
    },
};

error_t ecu_sensors_vss_init(ecu_sensor_vss_t instance, vss_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_vss_ctx_t *vss_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_VSS_MAX || ctx == NULL, err = E_PARAM);

    vss_ctx = &ecu_sensors_vss_ctx[instance];
    vss_ctx->ctx = ctx;

    vss_ctx->config_default.enabled = ecu_sensors_vss_enabled_default[instance];
    vss_ctx->config_default.input_pin = ecu_sensors_vss_input_pin_default[instance];

    err = vss_init(vss_ctx->ctx, &vss_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&vss_ctx->ctx->config, &vss_ctx->config_default, sizeof(vss_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_vss_get_default_config(ecu_sensor_vss_t instance, vss_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_vss_ctx_t *vss_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_VSS_MAX || config == NULL, err = E_PARAM);

    vss_ctx = &ecu_sensors_vss_ctx[instance];

    memcpy(config, &vss_ctx->config_default, sizeof(vss_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_vss_configure(ecu_sensor_vss_t instance, const vss_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_vss_ctx_t *vss_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_VSS_MAX || config == NULL, err = E_PARAM);

    vss_ctx = &ecu_sensors_vss_ctx[instance];

    err = vss_configure(vss_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_vss_reset(ecu_sensor_vss_t instance)
{
  error_t err = E_OK;
  ecu_sensors_vss_ctx_t *vss_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_VSS_MAX, err = E_PARAM);

    vss_ctx = &ecu_sensors_vss_ctx[instance];

    err = vss_reset(vss_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_sensors_vss_get_value(ecu_sensor_vss_t instance, vss_data_t *data)
{
  error_t err = E_OK;
  ecu_sensors_vss_ctx_t *vss_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_VSS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    vss_ctx = &ecu_sensors_vss_ctx[instance];

    err = vss_get_value(vss_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_sensors_vss_get_diag(ecu_sensor_vss_t instance, vss_diag_t *diag)
{
  error_t err = E_OK;
  ecu_sensors_vss_ctx_t *vss_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_VSS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    vss_ctx = &ecu_sensors_vss_ctx[instance];

    err = vss_get_diag(vss_ctx->ctx, diag);

  } while(0);

  return err;
}
