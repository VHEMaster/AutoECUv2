/*
 * config_timing.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_timing.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    timing_config_t config_default;
    timing_init_ctx_t init;
    timing_ctx_t *ctx;
}ecu_modules_timing_ctx_t;

static const timing_config_t ecu_modules_timing_config_default = {
    .ckp_instance = ECU_SENSOR_CKP_1,
    .cmp_config = {
        {
          .enabled = true,
          .cmp_instance = ECU_SENSOR_CMP_1,
          .pos_relative = -105.8f,
          .pos_min = -3.0f,
          .pos_max = 3.0f,
        },  //ECU_SENSOR_CMP_1
    },
};

static ecu_modules_timing_ctx_t ecu_modules_timing_ctx[ECU_MODULE_TIMING_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_modules_timing_config_default,
    },
};

error_t ecu_modules_timing_init(ecu_module_timing_t instance, timing_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX || ctx == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];
    timing_ctx->ctx = ctx;

    err = timing_init(timing_ctx->ctx, &timing_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&timing_ctx->ctx->config, &timing_ctx->config_default, sizeof(timing_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_timing_get_default_config(ecu_module_timing_t instance, timing_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    memcpy(config, &timing_ctx->config_default, sizeof(timing_config_t));

  } while(0);

  return err;
}

error_t ecu_modules_timing_configure(ecu_module_timing_t instance, const timing_config_t *config)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX || config == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    err = timing_configure(timing_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_modules_timing_reset(ecu_module_timing_t instance)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    err = timing_reset(timing_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_modules_timing_get_data(ecu_module_timing_t instance, timing_data_t *data)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    err = timing_get_data(timing_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_modules_timing_get_diag(ecu_module_timing_t instance, timing_diag_t *diag)
{
  error_t err = E_OK;
  ecu_modules_timing_ctx_t *timing_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_MODULE_TIMING_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    timing_ctx = &ecu_modules_timing_ctx[instance];

    err = timing_get_diag(timing_ctx->ctx, diag);

  } while(0);

  return err;
}
