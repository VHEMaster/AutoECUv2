/*
 * config_router.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "config_router.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#include "config_extern.h"
#include "common.h"

typedef struct ecu_comm_router_ctx_tag ecu_comm_router_ctx_t;

typedef struct ecu_comm_router_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
  const router_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  router_init_ctx_t init;
  router_ctx_t *ctx;
#endif /* CORE_CM4 */
}ecu_comm_router_ctx_t;

#ifdef CORE_CM7
static const router_config_t ecu_comm_router_default_config[ECU_COMM_ROUTER_MAX] = {
  {
      .can = {
          .signals = {
              .upstream_list = {

              },
              .downstream_list = {
                  {
                      .enabled = false,
                      .format = CAN_FRAME_FORMAT_CLASSIC,
                      .msg_id_1_start = 0x100,
                      .msg_id_2_end = 0x1FF,
                      .can_instance = ECU_COMM_CAN_1,
                      .signals_instance = ECU_COMM_SIGNALS_1,
                  },
              },
          },
      },
      .diagnostics = {
          .can_isotp = {
              {
                  .enabled = true,
                  .format = CAN_FRAME_FORMAT_CLASSIC,
                  .can_instance = ECU_COMM_CAN_1,
                  .uds_instance = ECU_COMM_UDS_1,
                  .obd2_instance = ECU_COMM_OBD2_1,
                  .upstream_phy_msg_id = 0x7E0,
                  .upstream_func_msg_id = 0x7DF,
                  .downstream_msg_id = 0x7E8,
              }, //ECU_COMM_ISOTP_1
              {

              }, //ECU_COMM_ISOTP_2
          },
          .kwp = {
              {
                  .enabled = true,
                  .uds_instance = ECU_COMM_UDS_2,
                  .obd2_instance = ECU_COMM_OBD2_2,
              }, //ECU_COMM_KWP_1
          },
      },
  }, //ECU_COMM_ROUTER_1
};

static const bool ecu_comm_router_enabled_default[ECU_COMM_ROUTER_MAX] = {
    true,
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_router_ctx_t ecu_comm_router_ctx[ECU_COMM_ROUTER_MAX] = {
    {
#ifdef CORE_CM4
      .init = {
          .signal_rx_callback = NULL,
          .error_callback = NULL,
          .callback_userdata = NULL,
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_router_default_config[ECU_COMM_ROUTER_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
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
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_router_get_default_config(ecu_comm_router_t instance, router_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_router_ctx_t *router_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ROUTER_MAX || config == NULL, err = E_PARAM);

    router_ctx = &ecu_comm_router_ctx[instance];

    memcpy(config, router_ctx->config_default, sizeof(router_config_t));
    config->enabled = ecu_comm_router_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_router_configure(ecu_comm_router_t instance, const router_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_router_ctx_t *router_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ROUTER_MAX || config == NULL, err = E_PARAM);

    router_ctx = &ecu_comm_router_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&router_ctx->rpc_configure, ECU_IPC_RPC_FUNC_ROUTER_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(router_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = router_configure(router_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_ROUTER, instance, router_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

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

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&router_ctx->rpc_reset, ECU_IPC_RPC_FUNC_ROUTER_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = router_reset(router_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}
