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
  router_config_t config_default;
  router_init_ctx_t init;
  router_ctx_t *ctx;
}ecu_comm_router_ctx_t;

static const router_config_t ecu_comm_router_default_config[ECU_COMM_ROUTER_MAX] = {
  {
      .can = {
          .signals = {
              .upstream_list = {


              },
              .downstream_list = {

              },
          },
      },
      .diagnostics = {
          .can_isotp_pairs = {
              {
                  .enabled = true,
                  .can_instance = ECU_COMM_CAN_1,
                  .isotp_instance = ECU_COMM_ISOTP_1,
                  .upstream_msg_id = 0x7E0,
                  .downstream_msg_id = 0x7E8,
              }, //ROUTER_CONFIG_CAN_ISOTP_PAIR_1
              {
                  .enabled = true,
                  .can_instance = ECU_COMM_CAN_1,
                  .isotp_instance = ECU_COMM_ISOTP_2,
                  .upstream_msg_id = 0x7DF,
                  .downstream_msg_id = 0x7E8,
              }, //ROUTER_CONFIG_CAN_ISOTP_PAIR_2
          },
          .isotp_uds_pairs = {
              {
                  .enabled = true,
                  .isotp_instance = ECU_COMM_ISOTP_1,
                  .uds_instance = ECU_COMM_UDS_1,
              }, //ROUTER_CONFIG_ISOTP_UDS_PAIR_1
              {
                  .enabled = true,
                  .isotp_instance = ECU_COMM_ISOTP_2,
                  .uds_instance = ECU_COMM_UDS_2,
              }, //ROUTER_CONFIG_ISOTP_UDS_PAIR_2
          },
          .kwp_uds_pairs = {
              {
                  .enabled = true,
                  .kwp_instance = ECU_COMM_KWP_1,
                  .uds_instance = ECU_COMM_UDS_3,
              }, //ROUTER_CONFIG_KWP_UDS_PAIR_1
              {

              }, //ROUTER_CONFIG_KWP_UDS_PAIR_2
          },
          .isotp_obd2_pairs = {
              {
                  .enabled = true,
                  .isotp_instance = ECU_COMM_ISOTP_1,
                  .obd2_instance = ECU_COMM_OBD2_1,
              }, //ROUTER_CONFIG_ISOTP_OBD2_PAIR_1
              {
                  .enabled = true,
                  .isotp_instance = ECU_COMM_ISOTP_2,
                  .obd2_instance = ECU_COMM_OBD2_2,
              }, //ROUTER_CONFIG_ISOTP_OBD2_PAIR_2
          },
          .kwp_obd2_pairs = {
              {
                  .enabled = true,
                  .kwp_instance = ECU_COMM_KWP_1,
                  .obd2_instance = ECU_COMM_OBD2_2,
              }, //ROUTER_CONFIG_KWP_OBD2_PAIR_1
              {

              }, //ROUTER_CONFIG_KWP_OBD2_PAIR_2
          },
      },
  }, //ECU_ROUTER_IF_1
};

static ecu_comm_router_ctx_t ecu_comm_router_ctx[ECU_COMM_ROUTER_MAX] = {
    {
      .init = {
          .signal_rx_callback = NULL,
          .error_callback = NULL,
          .callback_userdata = NULL,
      },
      .config_default = ecu_comm_router_default_config[ECU_COMM_ROUTER_1],
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

  } while(0);

  return err;
}

error_t ecu_comm_router_get_default_config(ecu_comm_router_t instance, router_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_router_ctx_t *router_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ROUTER_MAX || config == NULL, err = E_PARAM);

    router_ctx = &ecu_comm_router_ctx[instance];

    memcpy(config, &router_ctx->config_default, sizeof(router_config_t));

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
