/*
 * config_maf.c
 *
 *  Created on: Apr 17, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_maf.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    maf_config_t config_default;
    maf_init_ctx_t init;
    maf_ctx_t *ctx;
}ecu_sensors_maf_ctx_t;

static const maf_config_t ecu_sensors_maf_config_default = {

};

static ecu_sensors_maf_ctx_t ecu_sensors_maf_ctx[ECU_SENSOR_MAF_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_sensors_maf_config_default,
    },
};

error_t ecu_sensors_maf_init(ecu_sensor_maf_t instance, maf_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX || ctx == NULL, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];
    maf_ctx->ctx = ctx;

    err = maf_init(maf_ctx->ctx, &maf_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&maf_ctx->ctx->config, &maf_ctx->config_default, sizeof(maf_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_maf_get_default_config(ecu_sensor_maf_t instance, maf_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX || config == NULL, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];

    memcpy(config, &maf_ctx->config_default, sizeof(maf_config_t));

  } while(0);

  return err;
}

error_t ecu_sensors_maf_configure(ecu_sensor_maf_t instance, const maf_config_t *config)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX || config == NULL, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];

    err = maf_configure(maf_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_sensors_maf_reset(ecu_sensor_maf_t instance)
{
  error_t err = E_OK;
  ecu_sensors_maf_ctx_t *maf_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_SENSOR_MAF_MAX, err = E_PARAM);

    maf_ctx = &ecu_sensors_maf_ctx[instance];

    err = maf_reset(maf_ctx->ctx);

  } while(0);

  return err;
}
