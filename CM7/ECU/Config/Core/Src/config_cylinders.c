/*
 * config_cylinders.c
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_cylinders.h"
#include "config_extern.h"
#include "config_ckp.h"
#include "config_cmp.h"
#include "compiler.h"

typedef struct ecu_core_cylinders_ctx_tag {
    ecu_cylinders_config_t config_default;
    cylinders_init_ctx_t init;
    cylinders_ctx_t *ctx;
}ecu_core_cylinders_ctx_t;

static const ecu_cylinders_config_t ecu_core_cylinders_config_default = {
    .cylinders_count = 4,
    .cylinders = {
        {
            .pos_offset = 360.0f,
        }, //CYLINDERS_CY_1
        {
            .pos_offset = 180.0f,
        }, //CYLINDERS_CY_2
        {
            .pos_offset = 540.0f,
        }, //CYLINDERS_CY_3
        {
            .pos_offset = 0.0f,
        }, //CYLINDERS_CY_4
    },
};

static ecu_core_cylinders_ctx_t ecu_core_cylinders_ctx[ECU_CORE_COMPONENT_CYLINDERS_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_core_cylinders_config_default,
    },
};

error_t ecu_core_component_cylinders_init(ecu_core_component_cylinders_t instance, cylinders_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_core_cylinders_ctx_t *cylinders_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_CORE_COMPONENT_CYLINDERS_MAX || ctx == NULL, err = E_PARAM);

    cylinders_ctx = &ecu_core_cylinders_ctx[instance];
    cylinders_ctx->ctx = ctx;

    err = cylinders_init(cylinders_ctx->ctx, &cylinders_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&cylinders_ctx->ctx->config, &cylinders_ctx->config_default, sizeof(ecu_cylinders_config_t));

  } while(0);

  return err;
}

error_t ecu_core_component_cylinders_get_default_config(ecu_core_component_cylinders_t instance, ecu_cylinders_config_t *config)
{
  error_t err = E_OK;
  ecu_core_cylinders_ctx_t *cylinders_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_CORE_COMPONENT_CYLINDERS_MAX || config == NULL, err = E_PARAM);

    cylinders_ctx = &ecu_core_cylinders_ctx[instance];

    memcpy(config, &cylinders_ctx->config_default, sizeof(ecu_cylinders_config_t));

  } while(0);

  return err;
}

error_t ecu_core_component_cylinders_configure(ecu_core_component_cylinders_t instance, const ecu_cylinders_config_t *config)
{
  error_t err = E_OK;
  ecu_core_cylinders_ctx_t *cylinders_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_CORE_COMPONENT_CYLINDERS_MAX || config == NULL, err = E_PARAM);

    cylinders_ctx = &ecu_core_cylinders_ctx[instance];

    err = cylinders_configure(cylinders_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_core_component_cylinders_reset(ecu_core_component_cylinders_t instance)
{
  error_t err = E_OK;
  ecu_core_cylinders_ctx_t *cylinders_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_CORE_COMPONENT_CYLINDERS_MAX, err = E_PARAM);

    cylinders_ctx = &ecu_core_cylinders_ctx[instance];

    err = cylinders_reset(cylinders_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_core_component_cylinders_get_data(ecu_core_component_cylinders_t instance, cylinders_data_t *data)
{
  error_t err = E_OK;
  ecu_core_cylinders_ctx_t *cylinders_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_CORE_COMPONENT_CYLINDERS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(data == NULL, err = E_PARAM);

    cylinders_ctx = &ecu_core_cylinders_ctx[instance];

    err = cylinders_get_data(cylinders_ctx->ctx, data);

  } while(0);

  return err;
}

error_t ecu_core_component_cylinders_get_diag(ecu_core_component_cylinders_t instance, cylinders_diag_t *diag)
{
  error_t err = E_OK;
  ecu_core_cylinders_ctx_t *cylinders_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_CORE_COMPONENT_CYLINDERS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(diag == NULL, err = E_PARAM);

    cylinders_ctx = &ecu_core_cylinders_ctx[instance];

    err = cylinders_get_diag(cylinders_ctx->ctx, diag);

  } while(0);

  return err;
}
