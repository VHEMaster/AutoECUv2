/*
 * config_aps.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_aps.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    aps_config_t config_default;
    aps_init_ctx_t init;
    aps_ctx_t *ctx;
}ecu_sensors_aps_ctx_t;

static const aps_config_t ecu_sensors_aps_config_default = {

};

static ecu_sensors_aps_ctx_t ecu_sensors_aps_ctx[ECU_SENSOR_APS_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_aps_config_default,
    },
};

error_t ecu_sensors_aps_init(ecu_sensor_aps_t instance, aps_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX || ctx == NULL, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];
    aps_ctx->ctx = ctx;

    err = aps_init(aps_ctx->ctx, &aps_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&aps_ctx->ctx->config, &aps_ctx->config_default, sizeof(aps_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_aps_get_default_config(ecu_sensor_aps_t instance, aps_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX || config == NULL, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];

    memcpy(config, &aps_ctx->config_default, sizeof(aps_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_aps_configure(ecu_sensor_aps_t instance, const aps_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX || config == NULL, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];

    err = aps_configure(aps_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_aps_reset(ecu_sensor_aps_t instance)
{
  error_t err = E_OK;
  ecu_sensors_aps_ctx_t *aps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_APS_MAX, err = E_PARAM);

    aps_ctx = &ecu_sensors_aps_ctx[instance];

    err = aps_reset(aps_ctx->ctx);

  } while(0);

  return err;
}
