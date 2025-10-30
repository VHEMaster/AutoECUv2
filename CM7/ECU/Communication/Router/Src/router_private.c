/*
 * router_private.c
 *
 *  Created on: Oct 29, 2025
 *      Author: VHEMaster
 */

#include "config_comm.h"
#include "router_private.h"
#include "config_isotp.h"

static void router_can_isotp_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata);
static void router_can_isotp_err_callback(can_ctx_t *ctx, void *usrdata);

static void router_handle_diag_io_can_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance);
static void router_handle_diag_io(router_ctx_t *ctx);

static void router_handle_diag_protocol_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance);
static void router_handle_diag_protocol_kwp(router_ctx_t *ctx, ecu_comm_kwp_t instance);
static void router_handle_diag_protocols(router_ctx_t *ctx);

static error_t router_configure_diag_io_can_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance);
static error_t router_configure_diag_io_kwp(router_ctx_t *ctx, ecu_comm_kwp_t instance);
static error_t router_configure_diag_io(router_ctx_t *ctx);

static void router_reset_isotp(router_diag_isotp_ctx_t *diag_ctx);
static void router_reset_can(router_diag_isotp_ctx_t *diag_ctx);

static void router_can_isotp_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata)
{
  error_t err = E_OK;
  router_diag_isotp_ctx_t *diag_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(message == NULL);
    BREAK_IF(usrdata == NULL);

    diag_ctx = (router_diag_isotp_ctx_t *)usrdata;
    BREAK_IF(diag_ctx->config->upstream_msg_id != message->id);

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
        BREAK_IF(err != E_OK);
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
    memset(diag_ctx, 0, sizeof(*diag_ctx));
    diag_ctx->config = diag_cfg;

    if(diag_cfg->enabled == true) {
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

      err = can_register_rx_callback(diag_ctx->can_ctx, diag_cfg->upstream_msg_id, router_can_isotp_rx_callback, diag_ctx);
      BREAK_IF(err != E_OK);
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
    memset(diag_ctx, 0, sizeof(*diag_ctx));
    diag_ctx->config = diag_cfg;

    if(diag_cfg->enabled == true) {
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
    memset(&ctx->diag, 0, sizeof(ctx->diag));

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

error_t router_configure_diag(router_ctx_t *ctx)
{
  error_t err = E_OK;
  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    memset(&ctx->diag, 0, sizeof(ctx->diag));

    err = router_configure_diag_io(ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t router_configure_signals(router_ctx_t *ctx)
{
  error_t err = E_OK;
  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

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


  } while(0);
}
