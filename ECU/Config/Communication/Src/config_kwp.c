/*
 * config_kwp.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "config_kwp.h"
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_kwp_ctx_tag ecu_comm_kwp_ctx_t;

typedef struct ecu_comm_kwp_ctx_tag {
  kwp_config_t config_default;
  kwp_init_ctx_t init;
  kwp_ctx_t *ctx;
}ecu_comm_kwp_ctx_t;

static const kwp_config_t ecu_comm_kwp_default_config[ECU_COMM_KWP_MAX] = {
  {

  }, //ECU_KWP_IF_1
};

static const bool ecu_comm_kwp_enabled_default[ECU_COMM_KWP_MAX] = {
    true
};

static RAM_SECTION ecu_comm_kwp_ctx_t ecu_comm_kwp_ctx[ECU_COMM_KWP_MAX] = {
    {
      .init = {

      },
      .config_default = ecu_comm_kwp_default_config[ECU_COMM_KWP_1],
    },
};

error_t ecu_comm_kwp_init(ecu_comm_kwp_t instance, kwp_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_KWP_MAX || ctx == NULL, err = E_PARAM);

    kwp_ctx = &ecu_comm_kwp_ctx[instance];
    kwp_ctx->ctx = ctx;
    kwp_ctx->config_default.enabled = ecu_comm_kwp_enabled_default[instance];

    err = kwp_init(kwp_ctx->ctx, &kwp_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_KWP, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_comm_kwp_get_default_config(ecu_comm_kwp_t instance, kwp_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_KWP_MAX || config == NULL, err = E_PARAM);

    kwp_ctx = &ecu_comm_kwp_ctx[instance];

    memcpy(config, &kwp_ctx->config_default, sizeof(kwp_config_t));

  } while(0);

  return err;
}

error_t ecu_comm_kwp_configure(ecu_comm_kwp_t instance, const kwp_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_KWP_MAX || config == NULL, err = E_PARAM);

    kwp_ctx = &ecu_comm_kwp_ctx[instance];

    err = kwp_configure(kwp_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_KWP, instance, kwp_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_comm_kwp_reset(ecu_comm_kwp_t instance)
{
  error_t err = E_OK;
  ecu_comm_kwp_ctx_t *kwp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_KWP_MAX, err = E_PARAM);

    kwp_ctx = &ecu_comm_kwp_ctx[instance];

    err = kwp_reset(kwp_ctx->ctx);

  } while(0);

  return err;
}
