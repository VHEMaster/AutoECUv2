/*
 * config_router.c
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#include "config_router.h"
#include "config_extern.h"
#include "common.h"
#include "config_common.h"
#include "config_ckp.h"
#include "config_map.h"
#include "config_tps.h"

typedef struct ecu_comm_router_ctx_tag ecu_comm_router_ctx_t;

typedef struct ecu_comm_router_ctx_tag {
  router_config_t config_default;
  router_init_ctx_t init;
  router_ctx_t *ctx;
}ecu_comm_router_ctx_t;

static const router_signal_tx_message_config_t ecu_router_tx_messages[] = {
    {
        .enabled = true,
        .message_id = 0x600,
        .period = 100u * TIME_US_IN_MS,
        .signals_count = 3,
        .signals = {
            {
                .parameter_id = { .bitfield = {
                    .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                    .type = ECU_SENSOR_TYPE_CKP, .instance = ECU_SENSOR_CKP_1,
                    .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_CKP_READ_PARAM_DATA,
                }},
                .multiplier = 1.0f, .byte_offset = 0,
            },
            {
                .parameter_id = { .bitfield = {
                    .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                    .type = ECU_SENSOR_TYPE_MAP, .instance = ECU_SENSOR_MAP_1,
                    .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_MAP_READ_PARAM_DATA,
                }},
                .multiplier = 1000.0f, .byte_offset = 2,
            },
            {
                .parameter_id = { .bitfield = {
                    .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                    .type = ECU_SENSOR_TYPE_TPS, .instance = ECU_SENSOR_TPS_1,
                    .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_TPS_READ_PARAM_DATA,
                }},
                .multiplier = 100.0f, .byte_offset = 4,
            },
        },
    },
};

static const router_signal_tx_config_t ecu_router_tx_config = {
    .messages_count = ITEMSOF(ecu_router_tx_messages),
    .messages = ecu_router_tx_messages,
};

static const router_config_t ecu_comm_router_default_config[ECU_COMM_ROUTER_MAX] = {
  {
      .can = {
          .signals = {
              .upstream_list = {


              },
              .downstream_list = {
                  { .enabled = true, .msg_id_1_start = 0x600, .msg_id_2_end = 0x600, .can_instance = ECU_COMM_CAN_1 },
              },
          },
      },
      .diagnostics = {
          .can_isotp = {
              {
                  .enabled = true,
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
  }, //ECU_ROUTER_IF_1
};

static const bool ecu_comm_router_enabled_default[ECU_COMM_ROUTER_MAX] = {
    true,
};

static RAM_SECTION ecu_comm_router_ctx_t ecu_comm_router_ctx[ECU_COMM_ROUTER_MAX] = {
    {
      .init = {
          .signals_tx = &ecu_router_tx_config,
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
    router_ctx->config_default.enabled = ecu_comm_router_enabled_default[instance];

    err = router_init(router_ctx->ctx, &router_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_ROUTER, instance, false);
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
