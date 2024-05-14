/*
 * config_ect.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_ect.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    ect_config_t config_default;
    ect_init_ctx_t init;
    ect_ctx_t *ctx;
}ecu_sensors_ect_ctx_t;

static const ect_config_t ecu_sensors_ect_config_default = {

};

static ecu_sensors_ect_ctx_t ecu_sensors_ect_ctx[ECU_SENSOR_ECT_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_ect_config_default,
    },
};

error_t ecu_sensors_ect_init(ecu_sensor_ect_t instance, ect_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_ect_ctx_t *ect_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_ECT_MAX || ctx == NULL, err = E_PARAM);

    ect_ctx = &ecu_sensors_ect_ctx[instance];
    ect_ctx->ctx = ctx;

    err = ect_init(ect_ctx->ctx, &ect_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&ect_ctx->ctx->config, &ect_ctx->config_default, sizeof(ect_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_ect_get_default_config(ecu_sensor_ect_t instance, ect_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_ect_ctx_t *ect_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_ECT_MAX || config == NULL, err = E_PARAM);

    ect_ctx = &ecu_sensors_ect_ctx[instance];

    memcpy(config, &ect_ctx->config_default, sizeof(ect_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_ect_configure(ecu_sensor_ect_t instance, const ect_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_ect_ctx_t *ect_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_ECT_MAX || config == NULL, err = E_PARAM);

    ect_ctx = &ecu_sensors_ect_ctx[instance];

    err = ect_configure(ect_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_ect_reset(ecu_sensor_ect_t instance)
{
  error_t err = E_OK;
  ecu_sensors_ect_ctx_t *ect_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_ECT_MAX, err = E_PARAM);

    ect_ctx = &ecu_sensors_ect_ctx[instance];

    err = ect_reset(ect_ctx->ctx);

  } while(0);

  return err;
}
