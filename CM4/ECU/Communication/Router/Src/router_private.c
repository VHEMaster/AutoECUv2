/*
 * router_private.c
 *
 *  Created on: Oct 29, 2025
 *      Author: VHEMaster
 */

#include "config_comm.h"
#include "router_private.h"
#include "config_isotp.h"

static void router_can_signal_upstream_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata);
static void router_can_isotp_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata);
static void router_can_isotp_err_callback(can_ctx_t *ctx, void *usrdata);

static void router_handle_diag_io_can_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance);
static void router_handle_diag_io(router_ctx_t *ctx);

static void router_handle_signal_upstream(router_ctx_t *ctx);
static void router_handle_signal_downstream(router_ctx_t *ctx);

static void router_handle_diag_protocol_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance);
static void router_handle_diag_protocol_kwp(router_ctx_t *ctx, ecu_comm_kwp_t instance);
static void router_handle_diag_protocols(router_ctx_t *ctx);

static error_t router_configure_diag_io_can_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance);
static error_t router_configure_diag_io_kwp(router_ctx_t *ctx, ecu_comm_kwp_t instance);
static error_t router_configure_diag_io(router_ctx_t *ctx);
static error_t router_validate_can_signals_routes(const router_config_can_signals_list_item_t *items, uint32_t count, bool upstream);

static void router_reset_isotp(router_diag_isotp_ctx_t *diag_ctx);
static void router_reset_can(router_diag_isotp_ctx_t *diag_ctx);

static void router_can_signal_upstream_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata)
{
  error_t err = E_OK;
  router_signals_upstream_rx_callback_ctx_t *rx_cb_ctx = (router_signals_upstream_rx_callback_ctx_t *)usrdata;
  const router_config_can_signals_list_item_t *config_signal_item;
  ecu_comm_signals_t signals_instance;
  ecu_comm_can_t can_instance;
  signals_ctx_t *signals_ctx;
  router_ctx_t *router_ctx;
  can_ctx_t *can_ctx;

  do {
    BREAK_IF(message == NULL);
    BREAK_IF(rx_cb_ctx == NULL);
    BREAK_IF(ctx == NULL);

    config_signal_item = rx_cb_ctx->config_signal_item;
    BREAK_IF(config_signal_item == NULL);

    router_ctx = rx_cb_ctx->router_ctx;
    BREAK_IF(router_ctx == NULL);
    BREAK_IF(router_ctx->configured == false);

    signals_instance = config_signal_item->signals_instance;
    BREAK_IF(signals_instance >= ECU_COMM_SIGNALS_MAX);
    signals_ctx = router_ctx->signals.signals_ctxs[signals_instance];

    can_instance = config_signal_item->can_instance;
    BREAK_IF(can_instance >= ECU_COMM_CAN_MAX);
    can_ctx = router_ctx->signals.can_ctxs[can_instance];

    BREAK_IF(signals_ctx == NULL);
    BREAK_IF(can_ctx == NULL);
    BREAK_IF(can_ctx != ctx);
    BREAK_IF(rx_cb_ctx->can_ctx != ctx);

    BREAK_IF(message->id < config_signal_item->msg_id_1_start ||
        message->id > config_signal_item->msg_id_2_end);

    memcpy(&rx_cb_ctx->message.message, message, sizeof(can_message_t));
    rx_cb_ctx->message.timestamp = time_now_us();

    err = signals_data_write_upstream(signals_ctx, &rx_cb_ctx->message);
    if(err != E_OK) {
      // TODO: error handling
    }

  } while(0);
}

static void router_can_isotp_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata)
{
  error_t err = E_OK;
  router_diag_isotp_ctx_t *diag_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(message == NULL);
    BREAK_IF(usrdata == NULL);

    diag_ctx = (router_diag_isotp_ctx_t *)usrdata;
    BREAK_IF(diag_ctx->isotp_ctx == NULL);
    BREAK_IF(diag_ctx->router_ctx == NULL);
    BREAK_IF(diag_ctx->router_ctx->configured == false);
    BREAK_IF(diag_ctx->config->upstream_func_msg_id != message->id &&
        diag_ctx->config->upstream_phy_msg_id != message->id);

    err = isotp_frame_write_upstream(diag_ctx->isotp_ctx, (const isotp_frame_t *)message->payload);
    BREAK_IF(err != E_OK);

  } while(0);
}

static void router_can_isotp_err_callback(can_ctx_t *ctx, void *usrdata)
{
  router_diag_isotp_ctx_t *diag_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(usrdata == NULL);

    diag_ctx = (router_diag_isotp_ctx_t *)usrdata;
    BREAK_IF(diag_ctx->isotp_ctx == NULL);
    BREAK_IF(diag_ctx->router_ctx == NULL);
    BREAK_IF(diag_ctx->isotp_ctx->configured == false);
    BREAK_IF(diag_ctx->router_ctx->configured == false);

    router_reset_can(diag_ctx);

  } while(0);
}

static void router_handle_diag_io_can_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance)
{
  error_t err = E_OK;
  const router_config_can_isotp_t *diag_cfg;
  router_diag_isotp_ctx_t *diag_ctx;
  isotp_error_code_t isotp_err;

  do {
    BREAK_IF(ctx == NULL);
    diag_ctx = &ctx->diag.isotp_ctx[instance];
    diag_cfg = &ctx->config.diagnostics.can_isotp[instance];

    while(true) {
      err = ecu_comm_isotp_get_error_flag(instance, &isotp_err);
      if(err == E_OK) {
        if(isotp_err != ISOTP_OK) {
          router_reset_isotp(diag_ctx);
          break;
        }
      }

      if(diag_ctx->runtime.message_downstream_pending == false) {
        err = isotp_frame_read_downstream(diag_ctx->isotp_ctx, (isotp_frame_t *)diag_ctx->runtime.message_downstream.payload);
        if(err != E_OK) {
          BREAK_IF(err == E_AGAIN);
          router_reset_can(diag_ctx);
        }
        diag_ctx->runtime.message_downstream_pending = true;

        diag_ctx->runtime.message_downstream.id = diag_cfg->downstream_msg_id;
        diag_ctx->runtime.message_downstream.len = ISOTP_FRAME_LEN;
      }

      if(diag_ctx->runtime.message_downstream_pending == true) {
        err = can_tx(diag_ctx->can_ctx, &diag_ctx->runtime.message_downstream);
        if(err != E_OK) {
          BREAK_IF(err == E_AGAIN);
          router_reset_can(diag_ctx);
        }
        diag_ctx->runtime.message_downstream_pending = false;
      }
    }

  } while(0);
}

static void router_handle_diag_io(router_ctx_t *ctx)
{
  for(ecu_comm_isotp_t i = 0; i < ECU_COMM_ISOTP_MAX; i++) {
    if(ctx->diag.isotp_ctx[i].active == true) {
      router_handle_diag_io_can_isotp(ctx, i);
    }
  }
}

static void router_handle_diag_protocols(router_ctx_t *ctx)
{
  for(ecu_comm_isotp_t i = 0; i < ECU_COMM_ISOTP_MAX; i++) {
    if(ctx->diag.isotp_ctx[i].active) {
      router_handle_diag_protocol_isotp(ctx, i);
    }
  }
  for(ecu_comm_kwp_t i = 0; i < ECU_COMM_KWP_MAX; i++) {
    if(ctx->diag.kwp_ctx[i].active) {
      router_handle_diag_protocol_kwp(ctx, i);
    }
  }
}

static void router_handle_diag_protocol_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance)
{
  error_t err = E_OK;
  router_diag_isotp_ctx_t *diag_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(instance >= ECU_COMM_ISOTP_MAX);

    diag_ctx = &ctx->diag.isotp_ctx[instance];

    do {
      if(diag_ctx->runtime.isotp_upstream_pending == false) {
        diag_ctx->upstream_data_len = ISOTP_PAYLOAD_LEN_MAX;
        err = isotp_data_read_upstream(diag_ctx->isotp_ctx, diag_ctx->upstream_data, &diag_ctx->upstream_data_len);
        if(err == E_OK) {
          diag_ctx->runtime.isotp_upstream_pending = true;
        } else if(err != E_AGAIN) {
          router_reset_isotp(diag_ctx);
          break;
        }
      }

      if(diag_ctx->runtime.isotp_upstream_pending &&
          !diag_ctx->runtime.obd2_upstream_pending &&
          !diag_ctx->runtime.uds_upstream_pending) {
        diag_ctx->runtime.isotp_upstream_pending = false;

        if(diag_ctx->upstream_data_len > 0) {
          if(diag_ctx->obd2_ctx != NULL &&
              diag_ctx->upstream_data[0] >= OBD2_SID_CURRENT_DATA &&
              diag_ctx->upstream_data[0] <= OBD2_SID_PERMANENT_DIAGNOSTIC_TROUBLE_CODES) {
            diag_ctx->runtime.obd2_upstream_pending = true;
          } else if(diag_ctx->uds_ctx != NULL) {
            diag_ctx->runtime.uds_upstream_pending = true;
          }
        }
      }

      if(diag_ctx->runtime.obd2_upstream_pending && diag_ctx->runtime.uds_upstream_pending) {
        router_reset_isotp(diag_ctx);
      } else if(diag_ctx->runtime.obd2_upstream_pending && diag_ctx->obd2_ctx) {
        err = obd2_message_write_upstream(diag_ctx->obd2_ctx, diag_ctx->upstream_data, diag_ctx->upstream_data_len);
        if(err != E_OK) {
          BREAK_IF(err == E_AGAIN);
          router_reset_isotp(diag_ctx);
          break;
        }
        diag_ctx->runtime.obd2_upstream_pending = false;
      } else if(diag_ctx->runtime.uds_upstream_pending && diag_ctx->uds_ctx) {
        err = uds_message_write_upstream(diag_ctx->uds_ctx, diag_ctx->upstream_data, diag_ctx->upstream_data_len);
        if(err != E_OK) {
          BREAK_IF(err == E_AGAIN);
          router_reset_isotp(diag_ctx);
          break;
        }
        diag_ctx->runtime.uds_upstream_pending = false;
      }
    } while(0);

    do {
      if(!diag_ctx->runtime.isotp_downstream_pending) {
        if(diag_ctx->obd2_ctx != NULL) {
          diag_ctx->downstream_data_len = ISOTP_PAYLOAD_LEN_MAX;
          err = obd2_message_read_downstream(diag_ctx->obd2_ctx, diag_ctx->downstream_data, &diag_ctx->downstream_data_len);
          if(err == E_OK) {
            diag_ctx->runtime.isotp_downstream_pending = true;
          } else if(err != E_AGAIN) {
            router_reset_isotp(diag_ctx);
            break;
          }
        }
      }
      if(!diag_ctx->runtime.isotp_downstream_pending) {
        if(diag_ctx->uds_ctx != NULL) {
          diag_ctx->downstream_data_len = ISOTP_PAYLOAD_LEN_MAX;
          err = uds_message_read_downstream(diag_ctx->uds_ctx, diag_ctx->downstream_data, &diag_ctx->downstream_data_len);
          if(err == E_OK) {
            diag_ctx->runtime.isotp_downstream_pending = true;
          } else if(err != E_AGAIN) {
            router_reset_isotp(diag_ctx);
            break;
          }
        }
      }

      if(diag_ctx->runtime.isotp_downstream_pending) {
        err = isotp_data_write_downstream(diag_ctx->isotp_ctx, diag_ctx->downstream_data, diag_ctx->downstream_data_len);
        if(err == E_OK) {
          diag_ctx->runtime.isotp_downstream_pending = false;
        } else if(err != E_AGAIN) {
          router_reset_isotp(diag_ctx);
          break;
        }
      }
    } while(0);

  } while(0);
}

static void router_handle_diag_protocol_kwp(router_ctx_t *ctx, ecu_comm_kwp_t instance)
{
  // TODO: IMPLEMENT
}

static error_t router_configure_diag_io_can_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance)
{
  error_t err = E_OK;
  const router_config_can_isotp_t *diag_cfg;
  router_diag_isotp_ctx_t *diag_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    diag_cfg = &ctx->config.diagnostics.can_isotp[instance];
    diag_ctx = &ctx->diag.isotp_ctx[instance];

    if(diag_ctx->can_ctx != NULL) {
      err = can_unregister_rx_callback(diag_ctx->can_ctx, router_can_isotp_rx_callback, diag_ctx);
      BREAK_IF(err != E_OK);

      err = can_unregister_err_callback(diag_ctx->can_ctx, router_can_isotp_err_callback, diag_ctx);
      BREAK_IF(err != E_OK);
    }

    memset(diag_ctx, 0, sizeof(*diag_ctx));
    diag_ctx->config = diag_cfg;


    if(diag_cfg->enabled == true) {
      diag_ctx->router_ctx = ctx;
      err = ecu_comm_get_can_ctx(diag_cfg->can_instance, &diag_ctx->can_ctx);
      BREAK_IF(err != E_OK);
      err = ecu_comm_get_isotp_ctx(instance, &diag_ctx->isotp_ctx);
      BREAK_IF(err != E_OK);

      if(diag_cfg->uds_instance < ECU_COMM_UDS_MAX) {
        err = ecu_comm_get_uds_ctx(diag_cfg->uds_instance, &diag_ctx->uds_ctx);
        BREAK_IF(err != E_OK);
      } else {
        diag_ctx->uds_ctx = NULL;
      }

      if(diag_cfg->obd2_instance < ECU_COMM_OBD2_MAX) {
        err = ecu_comm_get_obd2_ctx(diag_cfg->obd2_instance, &diag_ctx->obd2_ctx);
        BREAK_IF(err != E_OK);
      } else {
        diag_ctx->obd2_ctx = NULL;
      }

      err = can_register_rx_callback(diag_ctx->can_ctx, diag_cfg->upstream_phy_msg_id,
          diag_cfg->upstream_phy_msg_id, router_can_isotp_rx_callback, diag_ctx);
      BREAK_IF(err != E_OK);
      if(diag_cfg->upstream_func_msg_id != diag_cfg->upstream_phy_msg_id) {
        err = can_register_rx_callback(diag_ctx->can_ctx, diag_cfg->upstream_func_msg_id,
            diag_cfg->upstream_func_msg_id, router_can_isotp_rx_callback, diag_ctx);
        BREAK_IF(err != E_OK);
      }
      err = can_register_err_callback(diag_ctx->can_ctx, router_can_isotp_err_callback, diag_ctx);
      BREAK_IF(err != E_OK);

      diag_ctx->active = true;
    }

  } while(0);

  return err;
}

static error_t router_configure_diag_io_kwp(router_ctx_t *ctx, ecu_comm_kwp_t instance)
{
  error_t err = E_OK;
  const router_config_kwp_t *diag_cfg;
  router_diag_kwp_ctx_t *diag_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    diag_cfg = &ctx->config.diagnostics.kwp[instance];
    diag_ctx = &ctx->diag.kwp_ctx[instance];

    //if(diag_ctx->kwp_ctx != NULL) {
    //  err = kwp_unregister_rx_callback(diag_ctx->kwp_ctx, router_can_kwp_rx_callback, diag_ctx);
    //  BREAK_IF(err != E_OK);
    //
    //  err = kwp_unregister_err_callback(diag_ctx->kwp_ctx, router_can_kwp_err_callback, diag_ctx);
    //  BREAK_IF(err != E_OK);
    //}
    // TODO: IMPLEMENT

    memset(diag_ctx, 0, sizeof(*diag_ctx));
    diag_ctx->config = diag_cfg;

    if(diag_cfg->enabled == true) {
      diag_ctx->router_ctx = ctx;
      err = ecu_comm_get_kwp_ctx(instance, &diag_ctx->kwp_ctx);
      BREAK_IF(err != E_OK);

      if(diag_cfg->uds_instance < ECU_COMM_UDS_MAX) {
        err = ecu_comm_get_uds_ctx(diag_cfg->uds_instance, &diag_ctx->uds_ctx);
        BREAK_IF(err != E_OK);
      } else {
        diag_ctx->uds_ctx = NULL;
      }

      if(diag_cfg->obd2_instance < ECU_COMM_OBD2_MAX) {
        err = ecu_comm_get_obd2_ctx(diag_cfg->obd2_instance, &diag_ctx->obd2_ctx);
        BREAK_IF(err != E_OK);
      } else {
        diag_ctx->obd2_ctx = NULL;
      }

      //err = kwp_register_rx_callback(diag_ctx->kwp_ctx, router_can_kwp_rx_callback, diag_ctx);
      //BREAK_IF(err != E_OK);
      //err = kwp_register_err_callback(diag_ctx->kwp_ctx, router_can_kwp_err_callback, diag_ctx);
      //BREAK_IF(err != E_OK);
      // TODO: IMPLEMENT

      diag_ctx->active = true;
    }

  } while(0);

  return err;
}

static error_t router_configure_diag_io(router_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    for(ecu_comm_isotp_t i = 0; i < ECU_COMM_ISOTP_MAX; i++) {
      err = router_configure_diag_io_can_isotp(ctx, i);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(ecu_comm_kwp_t i = 0; i < ECU_COMM_KWP_MAX; i++) {
      err = router_configure_diag_io_kwp(ctx, i);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

static void router_reset_can(router_diag_isotp_ctx_t *diag_ctx)
{
  (void)can_reset(diag_ctx->can_ctx);
  router_reset_isotp(diag_ctx);
}

static void router_reset_isotp(router_diag_isotp_ctx_t *diag_ctx)
{
  (void)isotp_reset(diag_ctx->isotp_ctx);

  if(diag_ctx->uds_ctx != NULL) {
    (void)uds_reset(diag_ctx->uds_ctx);
  }
  if(diag_ctx->obd2_ctx != NULL) {
    (void)obd2_reset(diag_ctx->obd2_ctx);
  }
  memset(&diag_ctx->runtime, 0, sizeof(diag_ctx->runtime));
}

static void router_handle_signal_upstream(router_ctx_t *ctx)
{
  // TODO: nothing to do here
}

static void router_handle_signal_downstream(router_ctx_t *ctx)
{
  error_t err = E_OK;
  signals_ctx_t *signal_ctx;
  can_ctx_t *can_ctx;
  signals_runtime_messages_fifo_item_t *message;
  router_signals_downstream_signal_ctx_t *downstream_signal;
  router_signals_downstream_can_ctx_t *downstream_can;
  const router_config_can_signals_list_item_t *signal_item;
  uint32_t message_id;
  bool break_condition;
  bool route_found;
  uint32_t iterations = 0;

  do {
    BREAK_IF(iterations++ >= ROUTER_DOWNSTREAM_LOOP_ITERATIONS_MAX);
    break_condition = true;

    for(ecu_comm_signals_t i = 0; i < ECU_COMM_SIGNALS_MAX; i++) {
      downstream_signal = &ctx->signals.downstream.signals[i];
      signal_ctx = ctx->signals.signals_ctxs[i];
      if(signal_ctx != NULL) {
        message = &downstream_signal->message;
        if(downstream_signal->signal_message_pending == false) {
          err = signals_data_read_downstream(signal_ctx, message);
          if(err == E_OK) {
            downstream_signal->signal_message_pending = true;
            break_condition = false;
          } else if(err != E_AGAIN) {
            // TODO: error handling
          }
        }

        if(downstream_signal->signal_message_pending == true) {
          message_id = message->message.id;
          route_found = false;
          for(uint32_t s = 0; s < ROUTER_SIGNAL_DOWNSTREAM_LIST_LENGTH; s++) {
            signal_item = &ctx->config.can.signals.downstream_list[s];
            CONTINUE_IF(signal_item->enabled == false);

            if(signal_item->signals_instance == i &&
                message_id >= signal_item->msg_id_1_start &&
                message_id <= signal_item->msg_id_2_end) {
              downstream_can = &ctx->signals.downstream.cans[signal_item->can_instance];
              can_ctx = ctx->signals.can_ctxs[signal_item->can_instance];
              if(can_ctx != NULL) {
                if(downstream_can->can_message_pending == false) {
                  memcpy(&downstream_can->message, message, sizeof(signals_runtime_messages_fifo_item_t));
                  downstream_can->can_message_pending = true;
                  downstream_signal->signal_message_pending = false;
                  break_condition = false;
                }
              }
              route_found = true;
              break;
            }
          }
          if(route_found == false) {
            downstream_signal->signal_message_pending = false;
            break_condition = false;
          }
        }
      }
    }

    for(ecu_comm_can_t i = 0; i < ECU_COMM_CAN_MAX; i++) {
      downstream_can = &ctx->signals.downstream.cans[i];
      can_ctx = ctx->signals.can_ctxs[i];
      if(can_ctx != NULL) {
        if(downstream_can->can_message_pending == true) {
          err = can_tx(can_ctx, &downstream_can->message.message);
          if(err != E_AGAIN) {
            if(err == E_OK) {
              break_condition = false;
            } else {
              // TODO: error handling
            }
            downstream_can->can_message_pending = false;
          }
        }
      }

    }
  } while(break_condition == false);
}
static error_t router_validate_can_signals_routes(const router_config_can_signals_list_item_t *items, uint32_t count, bool upstream)
{
  error_t err = E_OK;
  const router_config_can_signals_list_item_t *route;
  const router_config_can_signals_list_item_t *previous;

  do {
    BREAK_IF_ACTION(items == NULL, err = E_PARAM);

    for(uint32_t i = 0; i < count; i++) {
      route = &items[i];
      CONTINUE_IF(route->enabled == false);

      BREAK_IF_ACTION(route->msg_id_1_start > route->msg_id_2_end, err = E_PARAM);

      for(uint32_t p = 0; p < i; p++) {
        previous = &items[p];
        CONTINUE_IF(previous->enabled == false);

        BREAK_IF_ACTION(route->signals_instance == previous->signals_instance &&
            route->msg_id_1_start <= previous->msg_id_2_end &&
            previous->msg_id_1_start <= route->msg_id_2_end &&
            (upstream || route->can_instance != previous->can_instance), err = E_PARAM);
      }
      BREAK_IF(err != E_OK);
    }

  } while(0);

  return err;
}

error_t router_configure_diag(router_ctx_t *ctx)
{
  error_t err = E_OK;
  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    err = router_configure_diag_io(ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t router_configure_signals(router_ctx_t *ctx)
{
  error_t err = E_OK;
  const router_config_can_signals_list_item_t *config_signal_item;
  router_signals_upstream_rx_callback_ctx_t *rx_cb_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    for(uint32_t s = 0; s < ROUTER_SIGNAL_UPSTREAM_LIST_LENGTH; s++) {
      rx_cb_ctx = &ctx->signals.upstream.rx_cb_ctx[s];
      CONTINUE_IF(rx_cb_ctx->can_ctx == NULL);
      err = can_unregister_rx_callback(rx_cb_ctx->can_ctx, router_can_signal_upstream_rx_callback, rx_cb_ctx);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    memset(&ctx->signals, 0, sizeof(ctx->signals));

    err = router_validate_can_signals_routes(ctx->config.can.signals.upstream_list,
        ITEMSOF(ctx->config.can.signals.upstream_list), true);
    BREAK_IF(err != E_OK);

    err = router_validate_can_signals_routes(ctx->config.can.signals.downstream_list,
        ITEMSOF(ctx->config.can.signals.downstream_list), false);
    BREAK_IF(err != E_OK);

    for(uint32_t s = 0; s < ROUTER_SIGNAL_UPSTREAM_LIST_LENGTH; s++) {
      config_signal_item = &ctx->config.can.signals.upstream_list[s];
      CONTINUE_IF(config_signal_item->enabled == false);

      BREAK_IF_ACTION(config_signal_item->can_instance >= ECU_COMM_CAN_MAX, err = E_PARAM);
      BREAK_IF_ACTION(config_signal_item->signals_instance >= ECU_COMM_SIGNALS_MAX, err = E_PARAM);

      if(ctx->signals.can_ctxs[config_signal_item->can_instance] == NULL) {
        err = ecu_comm_get_can_ctx(config_signal_item->can_instance,
            &ctx->signals.can_ctxs[config_signal_item->can_instance]);
        BREAK_IF(err != E_OK);
      }
      if(ctx->signals.signals_ctxs[config_signal_item->signals_instance] == NULL) {
        err = ecu_comm_get_signals_ctx(config_signal_item->signals_instance,
            &ctx->signals.signals_ctxs[config_signal_item->signals_instance]);
        BREAK_IF(err != E_OK);
      }

      rx_cb_ctx = &ctx->signals.upstream.rx_cb_ctx[s];
      rx_cb_ctx->config_signal_item = config_signal_item;
      rx_cb_ctx->router_ctx = ctx;
      rx_cb_ctx->can_ctx = ctx->signals.can_ctxs[config_signal_item->can_instance];
      err = can_register_rx_callback(rx_cb_ctx->can_ctx,
          config_signal_item->msg_id_1_start, config_signal_item->msg_id_2_end,
          router_can_signal_upstream_rx_callback, rx_cb_ctx);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(uint32_t s = 0; s < ROUTER_SIGNAL_DOWNSTREAM_LIST_LENGTH; s++) {
      config_signal_item = &ctx->config.can.signals.downstream_list[s];
      CONTINUE_IF(config_signal_item->enabled == false);

      BREAK_IF_ACTION(config_signal_item->can_instance >= ECU_COMM_CAN_MAX, err = E_PARAM);
      BREAK_IF_ACTION(config_signal_item->signals_instance >= ECU_COMM_SIGNALS_MAX, err = E_PARAM);

      if(ctx->signals.can_ctxs[config_signal_item->can_instance] == NULL) {
        err = ecu_comm_get_can_ctx(config_signal_item->can_instance,
            &ctx->signals.can_ctxs[config_signal_item->can_instance]);
        BREAK_IF(err != E_OK);
      }
      if(ctx->signals.signals_ctxs[config_signal_item->signals_instance] == NULL) {
        err = ecu_comm_get_signals_ctx(config_signal_item->signals_instance,
            &ctx->signals.signals_ctxs[config_signal_item->signals_instance]);
        BREAK_IF(err != E_OK);
      }
    }
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

void router_handle_diag(router_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    router_handle_diag_io(ctx);
    router_handle_diag_protocols(ctx);

  } while(0);
}

void router_handle_signals(router_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    router_handle_signal_upstream(ctx);
    router_handle_signal_downstream(ctx);

  } while(0);
}
