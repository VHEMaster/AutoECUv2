/*
 * config_router.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "config_router.h"
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_router_ctx_tag ecu_comm_router_ctx_t;

typedef struct ecu_comm_router_ctx_tag {
  router_init_ctx_t init;
  router_ctx_t *ctx;
}ecu_comm_router_ctx_t;





static RAM_SECTION ecu_comm_router_ctx_t ecu_comm_router_ctx[ECU_COMM_ROUTER_MAX] = {
    {
      .init = {
          .signal_rx_callback = NULL,
          .error_callback = NULL,
          .callback_userdata = NULL,
      },
    },
};

error_t ecu_comm_router_init(ecu_comm_router_t instance, router_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_router_ctx_t *router_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ROUTER_MAX || ctx == NULL, err = E_PARAM);

    router_ctx = &ecu_comm_router_ctx[instance];
    router_ctx->ctx = ctx;

    err = router_init(router_ctx->ctx, &router_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_ROUTER, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}



error_t ecu_comm_router_configure(ecu_comm_router_t instance, const router_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_router_ctx_t *router_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ROUTER_MAX || config == NULL, err = E_PARAM);

    router_ctx = &ecu_comm_router_ctx[instance];

    err = router_configure(router_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_ROUTER, instance, router_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_comm_router_reset(ecu_comm_router_t instance)
{
  error_t err = E_OK;
  ecu_comm_router_ctx_t *router_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ROUTER_MAX, err = E_PARAM);

    router_ctx = &ecu_comm_router_ctx[instance];

    err = router_reset(router_ctx->ctx);

  } while(0);

  return err;
}
