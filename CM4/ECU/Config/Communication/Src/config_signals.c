/*
 * config_signals.c
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#include "config_signals.h"
#include "config_extern.h"
#include "config_global.h"
#include "common.h"

typedef struct ecu_comm_signals_ctx_tag ecu_comm_signals_ctx_t;

typedef struct ecu_comm_signals_ctx_tag {
  signals_init_ctx_t init;
  signals_ctx_t *ctx;
}ecu_comm_signals_ctx_t;

static RAM_SECTION signals_runtime_messages_fifo_item_t ecu_comm_signals_messages_upstream_buffer[ECU_COMM_SIGNALS_MAX][SIGNALS_CONFIG_MESSAGES_UPSTREAM_LIST_SIZE_MAX];
static RAM_SECTION signals_runtime_messages_fifo_item_t ecu_comm_signals_messages_downstream_buffer[ECU_COMM_SIGNALS_MAX][SIGNALS_CONFIG_MESSAGES_DOWNSTREAM_LIST_SIZE_MAX];





static RAM_SECTION ecu_comm_signals_ctx_t ecu_comm_signals_ctx[ECU_COMM_SIGNALS_MAX] = {
    {
      .init = {
          .messages_upstream_buffer = ecu_comm_signals_messages_upstream_buffer[ECU_COMM_SIGNALS_1],
          .messages_upstream_buffer_len = ITEMSOF(ecu_comm_signals_messages_upstream_buffer[ECU_COMM_SIGNALS_1]),
          .messages_downstream_buffer = ecu_comm_signals_messages_downstream_buffer[ECU_COMM_SIGNALS_1],
          .messages_downstream_buffer_len = ITEMSOF(ecu_comm_signals_messages_downstream_buffer[ECU_COMM_SIGNALS_1]),
      },
    },
};

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



error_t ecu_comm_signals_configure(ecu_comm_signals_t instance, const signals_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_signals_ctx_t *signals_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_SIGNALS_MAX || config == NULL, err = E_PARAM);

    signals_ctx = &ecu_comm_signals_ctx[instance];

    err = signals_configure(signals_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_SIGNALS, instance, signals_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

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

    err = signals_reset(signals_ctx->ctx);

  } while(0);

  return err;
}
