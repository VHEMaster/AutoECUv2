/*
 * config_iat.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_iat.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    iat_config_t config_default;
    iat_init_ctx_t init;
    iat_ctx_t *ctx;
}ecu_sensors_iat_ctx_t;

static const iat_config_t ecu_sensors_iat_config_default = {

};

static ecu_sensors_iat_ctx_t ecu_sensors_iat_ctx[ECU_SENSOR_IAT_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_iat_config_default,
    },
};

error_t ecu_sensors_iat_init(ecu_sensor_iat_t instance, iat_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX || ctx == NULL, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];
    iat_ctx->ctx = ctx;

    err = iat_init(iat_ctx->ctx, &iat_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&iat_ctx->ctx->config, &iat_ctx->config_default, sizeof(iat_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_iat_get_default_config(ecu_sensor_iat_t instance, iat_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX || config == NULL, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];

    memcpy(config, &iat_ctx->config_default, sizeof(iat_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_iat_configure(ecu_sensor_iat_t instance, const iat_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX || config == NULL, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];

    err = iat_configure(iat_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_iat_reset(ecu_sensor_iat_t instance)
{
  error_t err = E_OK;
  ecu_sensors_iat_ctx_t *iat_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_IAT_MAX, err = E_PARAM);

    iat_ctx = &ecu_sensors_iat_ctx[instance];

    err = iat_reset(iat_ctx->ctx);

  } while(0);

  return err;
}
