/*
 * config_signals.c
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#include "config_signals.h"
#ifdef CORE_CM7
#include "config_comm_rpc.h"
#endif /* CORE_CM7 */
#include "config_extern.h"
#ifdef CORE_CM7
#include "config_global.h"
#endif /* CORE_CM7 */
#include "common.h"

typedef struct ecu_comm_signals_ctx_tag ecu_comm_signals_ctx_t;

typedef struct ecu_comm_signals_ctx_tag {
#ifdef CORE_CM7
    ecu_comm_rpc_ctx_t rpc_configure;
    ecu_comm_rpc_ctx_t rpc_reset;
  const signals_config_t *config_default;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  signals_init_ctx_t init;
  signals_ctx_t *ctx;
#endif /* CORE_CM4 */
}ecu_comm_signals_ctx_t;

#ifdef CORE_CM4
static RAM_SECTION signals_runtime_messages_fifo_item_t ecu_comm_signals_messages_upstream_buffer[ECU_COMM_SIGNALS_MAX][SIGNALS_CONFIG_MESSAGES_UPSTREAM_LIST_SIZE_MAX];
static RAM_SECTION signals_runtime_messages_fifo_item_t ecu_comm_signals_messages_downstream_buffer[ECU_COMM_SIGNALS_MAX][SIGNALS_CONFIG_MESSAGES_DOWNSTREAM_LIST_SIZE_MAX];
#endif /* CORE_CM4 */

#ifdef CORE_CM7
static const signals_config_t ecu_comm_signals_default_config[ECU_COMM_SIGNALS_MAX] = {
  {
      .messages_lists = {
          .upstream_list = {
              {

              },
          },
          .downstream_list = {
              {
                  .enabled = true,
                  .format = CAN_FRAME_FORMAT_CLASSIC,
                  .message_id = 0x100,
                  .message_len = 8u,
                  .upstream_timeout = 500 * TIME_US_IN_MS,
                  .downstream_periodicity = 100 * TIME_US_IN_MS,
                  .has_counter = true,
                  .has_crc = true,
                  .counter_mask = 0x0F,
                  .crc_poly = 0x1D,
                  .counter_byte_pos = 6,
                  .crc_byte_pos = 7,
              },
          },
      },
      .signals_lists = {
          .upstream_list = {
              {

              },
          },
          .downstream_list = {
              {
                  .enabled = true,
                  .message_id = 0x100,
                  .description = {
                      .byteorder = CAN_SIGNAL_BYTEORDER_INTEL,
                      .type = CAN_SIGNAL_TYPE_UFLT,
                      .bitlength = 8,
                      .startbit = 0,
                      .offset = 0,
                      .gain = 40.0f,
                  },
                  .parameter_id = { .bitfield = {
                      .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                      .type = ECU_SENSOR_TYPE_CKP, .instance = ECU_SENSOR_CKP_1,
                      .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_CKP_READ_PARAM_RPM,
                  }},
              },
              {
                  .enabled = true,
                  .message_id = 0x100,
                  .description = {
                      .byteorder = CAN_SIGNAL_BYTEORDER_INTEL,
                      .type = CAN_SIGNAL_TYPE_UFLT,
                      .bitlength = 8,
                      .startbit = 8,
                      .offset = 0,
                      .gain = 0.012f,
                  },
                  .parameter_id = { .bitfield = {
                      .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                      .type = ECU_SENSOR_TYPE_MAP, .instance = ECU_SENSOR_MAP_1,
                      .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_MAP_READ_PARAM_PRESSURE,
                  }},
              },
              {
                  .enabled = true,
                  .message_id = 0x100,
                  .description = {
                      .byteorder = CAN_SIGNAL_BYTEORDER_INTEL,
                      .type = CAN_SIGNAL_TYPE_UFLT,
                      .bitlength = 8,
                      .startbit = 16,
                      .offset = 0,
                      .gain = 0.4f,
                  },
                  .parameter_id = { .bitfield = {
                      .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                      .type = ECU_SENSOR_TYPE_TPS, .instance = ECU_SENSOR_TPS_1,
                      .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_TPS_READ_PARAM_THROTTLE_POSITION,
                  }},
              },
          },
      },
  }, //ECU_COMM_SIGNALS_1
};

static const bool ecu_comm_signals_enabled_default[ECU_COMM_SIGNALS_MAX] = {
    true,
};
#endif /* CORE_CM7 */

static RAM_SECTION ecu_comm_signals_ctx_t ecu_comm_signals_ctx[ECU_COMM_SIGNALS_MAX] = {
    {
#ifdef CORE_CM4
      .init = {
          .messages_upstream_buffer = ecu_comm_signals_messages_upstream_buffer[ECU_COMM_SIGNALS_1],
          .messages_upstream_buffer_len = ITEMSOF(ecu_comm_signals_messages_upstream_buffer[ECU_COMM_SIGNALS_1]),
          .messages_downstream_buffer = ecu_comm_signals_messages_downstream_buffer[ECU_COMM_SIGNALS_1],
          .messages_downstream_buffer_len = ITEMSOF(ecu_comm_signals_messages_downstream_buffer[ECU_COMM_SIGNALS_1]),
      },
#endif /* CORE_CM4 */
#ifdef CORE_CM7
      .config_default = &ecu_comm_signals_default_config[ECU_COMM_SIGNALS_1],
#endif /* CORE_CM7 */
    },
};

#ifdef CORE_CM4
error_t ecu_comm_signals_init(ecu_comm_signals_t instance, signals_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_signals_ctx_t *signals_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_SIGNALS_MAX || ctx == NULL, err = E_PARAM);

    signals_ctx = &ecu_comm_signals_ctx[instance];
    signals_ctx->ctx = ctx;

    err = signals_init(signals_ctx->ctx, &signals_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_SIGNALS, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
#endif /* CORE_CM4 */

#ifdef CORE_CM7
error_t ecu_comm_signals_get_default_config(ecu_comm_signals_t instance, signals_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_signals_ctx_t *signals_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_SIGNALS_MAX || config == NULL, err = E_PARAM);

    signals_ctx = &ecu_comm_signals_ctx[instance];

    memcpy(config, signals_ctx->config_default, sizeof(signals_config_t));
    config->enabled = ecu_comm_signals_enabled_default[instance];

  } while(0);

  return err;
}
#endif /* CORE_CM7 */

error_t ecu_comm_signals_configure(ecu_comm_signals_t instance, const signals_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_signals_ctx_t *signals_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_SIGNALS_MAX || config == NULL, err = E_PARAM);

    signals_ctx = &ecu_comm_signals_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&signals_ctx->rpc_configure, ECU_IPC_RPC_FUNC_SIGNALS_CONFIGURE,
        instance, config, sizeof(*config), ALIGNOF(signals_config_t));
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = signals_configure(signals_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_SIGNALS, instance, signals_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_comm_signals_reset(ecu_comm_signals_t instance)
{
  error_t err = E_OK;
  ecu_comm_signals_ctx_t *signals_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_SIGNALS_MAX, err = E_PARAM);

    signals_ctx = &ecu_comm_signals_ctx[instance];

#ifdef CORE_CM7
    err = ecu_comm_rpc_call(&signals_ctx->rpc_reset, ECU_IPC_RPC_FUNC_SIGNALS_RESET,
        instance, NULL, 0u, 1u);
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    err = signals_reset(signals_ctx->ctx);
#endif /* CORE_CM4 */

  } while(0);

  return err;
}
