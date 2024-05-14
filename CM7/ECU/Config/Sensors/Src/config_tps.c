/*
 * config_tps.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_tps.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    tps_config_t config_default;
    tps_init_ctx_t init;
    tps_ctx_t *ctx;
}ecu_sensors_tps_ctx_t;

static const tps_config_t ecu_sensors_tps_config_default = {

};

static ecu_sensors_tps_ctx_t ecu_sensors_tps_ctx[ECU_SENSOR_TPS_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_tps_config_default,
    },
};

error_t ecu_sensors_tps_init(ecu_sensor_tps_t instance, tps_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_TPS_MAX || ctx == NULL, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];
    tps_ctx->ctx = ctx;

    err = tps_init(tps_ctx->ctx, &tps_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&tps_ctx->ctx->config, &tps_ctx->config_default, sizeof(tps_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_tps_get_default_config(ecu_sensor_tps_t instance, tps_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_TPS_MAX || config == NULL, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];

    memcpy(config, &tps_ctx->config_default, sizeof(tps_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_tps_configure(ecu_sensor_tps_t instance, const tps_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_TPS_MAX || config == NULL, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];

    err = tps_configure(tps_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_tps_reset(ecu_sensor_tps_t instance)
{
  error_t err = E_OK;
  ecu_sensors_tps_ctx_t *tps_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_TPS_MAX, err = E_PARAM);

    tps_ctx = &ecu_sensors_tps_ctx[instance];

    err = tps_reset(tps_ctx->ctx);

  } while(0);

  return err;
}
