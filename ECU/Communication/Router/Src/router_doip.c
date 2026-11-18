/*
 * router_doip.c
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#include "router.h"
#include "router_private.h"
#include "config_comm.h"

static void router_doip_reset(router_diag_doip_ctx_t *diag_ctx)
{
  if(diag_ctx->uds_ctx != NULL) {
    (void)uds_reset(diag_ctx->uds_ctx);
  }
  if(diag_ctx->obd2_ctx != NULL) {
    (void)obd2_reset(diag_ctx->obd2_ctx);
  }
  diag_ctx->state = ROUTER_DOIP_STATE_RESET;
  diag_ctx->response_pending = false;
}

error_t router_configure_doip(router_ctx_t *ctx)
{
  error_t err = E_OK;
  const router_config_doip_t *config;
  router_diag_doip_ctx_t *diag_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    for(uint32_t i = 0; i < ECU_COMM_DOIP_MAX; i++) {
      config = &ctx->config.diagnostics.doip[i];
      CONTINUE_IF(!config->enabled);
      BREAK_IF_ACTION(config->uds_instance >= ECU_COMM_UDS_MAX && config->obd2_instance >= ECU_COMM_OBD2_MAX, err = E_PARAM);

      /* Protocol mailboxes have one owner; sharing them loses the reply route. */
      for(uint32_t n = 0; n < ECU_COMM_ISOTP_MAX; n++) {
        const router_config_can_isotp_t *other = &ctx->config.diagnostics.can_isotp[n];
        CONTINUE_IF(!other->enabled);
        BREAK_IF_ACTION((config->uds_instance < ECU_COMM_UDS_MAX && config->uds_instance == other->uds_instance) ||
            (config->obd2_instance < ECU_COMM_OBD2_MAX && config->obd2_instance == other->obd2_instance), err = E_PARAM);
      }
      BREAK_IF(err != E_OK);
      for(uint32_t n = 0; n < ECU_COMM_KWP_MAX; n++) {
        const router_config_kwp_t *other = &ctx->config.diagnostics.kwp[n];
        CONTINUE_IF(!other->enabled);
        BREAK_IF_ACTION((config->uds_instance < ECU_COMM_UDS_MAX && config->uds_instance == other->uds_instance) ||
            (config->obd2_instance < ECU_COMM_OBD2_MAX && config->obd2_instance == other->obd2_instance), err = E_PARAM);
      }
      BREAK_IF(err != E_OK);
      for(uint32_t n = 0; n < i; n++) {
        const router_config_doip_t *other = &ctx->config.diagnostics.doip[n];
        CONTINUE_IF(!other->enabled);
        BREAK_IF_ACTION((config->uds_instance < ECU_COMM_UDS_MAX && config->uds_instance == other->uds_instance) ||
            (config->obd2_instance < ECU_COMM_OBD2_MAX && config->obd2_instance == other->obd2_instance), err = E_PARAM);
      }
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(uint32_t i = 0; i < ECU_COMM_DOIP_MAX; i++) {
      config = &ctx->config.diagnostics.doip[i];
      diag_ctx = &ctx->diag.doip_ctx[i];
      if(diag_ctx->active) {
        router_doip_reset(diag_ctx);
      }
      memset(diag_ctx, 0u, sizeof(*diag_ctx));
      diag_ctx->config = config;
      CONTINUE_IF(!config->enabled);

      err = ecu_comm_get_doip_ctx(i, &diag_ctx->doip_ctx);
      BREAK_IF(err != E_OK);
      if(config->uds_instance < ECU_COMM_UDS_MAX) {
        err = ecu_comm_get_uds_ctx(config->uds_instance, &diag_ctx->uds_ctx);
        BREAK_IF(err != E_OK);
      }
      if(config->obd2_instance < ECU_COMM_OBD2_MAX) {
        err = ecu_comm_get_obd2_ctx(config->obd2_instance, &diag_ctx->obd2_ctx);
        BREAK_IF(err != E_OK);
      }
      diag_ctx->active = true;
      diag_ctx->state = ROUTER_DOIP_STATE_RESET;
    }

  } while(0);

  return err;
}

static void router_handle_diag_protocol_doip(router_diag_doip_ctx_t *diag_ctx)
{
  error_t err = E_OK;
  bool upstream;
  bool downstream;

  do {
    if(diag_ctx->state == ROUTER_DOIP_STATE_RESET) {
      BREAK_IF(diag_ctx->uds_ctx != NULL && diag_ctx->uds_ctx->reset_trigger);
      BREAK_IF(diag_ctx->obd2_ctx != NULL && diag_ctx->obd2_ctx->reset_trigger);
      diag_ctx->state = ROUTER_DOIP_STATE_IDLE;
    }
    if(diag_ctx->state != ROUTER_DOIP_STATE_IDLE) {
      err = doip_message_is_active(diag_ctx->doip_ctx, &diag_ctx->message);
      BREAK_IF(err == E_AGAIN);
      if(err != E_OK || time_diff(time_now_us(), diag_ctx->request_time) >= 5u * TIME_US_IN_S) {
        router_doip_reset(diag_ctx);
        break;
      }
    }
    if(diag_ctx->state == ROUTER_DOIP_STATE_IDLE) {
      diag_ctx->upstream_data_len = sizeof(diag_ctx->upstream_data);
      err = doip_data_read_upstream(diag_ctx->doip_ctx, &diag_ctx->message,
          diag_ctx->upstream_data, &diag_ctx->upstream_data_len);
      BREAK_IF(err != E_OK);

      diag_ctx->obd2 = diag_ctx->upstream_data[0] >= OBD2_SID_CURRENT_DATA &&
          diag_ctx->upstream_data[0] <= OBD2_SID_PERMANENT_DIAGNOSTIC_TROUBLE_CODES;
      diag_ctx->state = ROUTER_DOIP_STATE_UPSTREAM;
      diag_ctx->response_pending = false;
      diag_ctx->request_time = time_now_us();
    }
    if(diag_ctx->state == ROUTER_DOIP_STATE_UPSTREAM) {
      if(diag_ctx->obd2 && diag_ctx->obd2_ctx != NULL) {
        err = obd2_message_write_upstream(diag_ctx->obd2_ctx, diag_ctx->upstream_data, diag_ctx->upstream_data_len);
      } else if(!diag_ctx->obd2 && diag_ctx->uds_ctx != NULL) {
        err = uds_message_write_upstream(diag_ctx->uds_ctx, diag_ctx->upstream_data, diag_ctx->upstream_data_len);
      } else {
        err = E_NOTSUPPORT;
      }
      BREAK_IF(err == E_AGAIN);
      if(err == E_OK) {
        diag_ctx->state = ROUTER_DOIP_STATE_RESPONSE;
      } else {
        diag_ctx->downstream_data[0] = 0x7Fu;
        diag_ctx->downstream_data[1] = diag_ctx->upstream_data[0];
        diag_ctx->downstream_data[2] = err == E_OVERFLOW ? 0x13u : 0x11u;
        diag_ctx->downstream_data_len = 3u;
        diag_ctx->state = ROUTER_DOIP_STATE_DOWNSTREAM;
      }
      /* Let the protocol main loop consume the request before polling a reply. */
      break;
    }
    if(diag_ctx->state == ROUTER_DOIP_STATE_RESPONSE) {
      diag_ctx->downstream_data_len = sizeof(diag_ctx->downstream_data);
      if(diag_ctx->obd2) {
        upstream = diag_ctx->obd2_ctx->upstream_available;
        downstream = diag_ctx->obd2_ctx->downstream_available;
        err = obd2_message_read_downstream(diag_ctx->obd2_ctx, diag_ctx->downstream_data, &diag_ctx->downstream_data_len);
      } else {
        upstream = diag_ctx->uds_ctx->upstream_available;
        downstream = diag_ctx->uds_ctx->downstream_available;
        err = uds_message_read_downstream(diag_ctx->uds_ctx, diag_ctx->downstream_data, &diag_ctx->downstream_data_len);
      }
      if(err == E_AGAIN) {
        /* Existing protocol handlers also support requests with no response. */
        if(!upstream && !downstream && !diag_ctx->response_pending) {
          diag_ctx->state = ROUTER_DOIP_STATE_IDLE;
        }
        break;
      }
      if(err != E_OK) {
        router_doip_reset(diag_ctx);
        break;
      }
      diag_ctx->state = ROUTER_DOIP_STATE_DOWNSTREAM;
    }
    if(diag_ctx->state == ROUTER_DOIP_STATE_DOWNSTREAM) {
      err = doip_data_write_downstream(diag_ctx->doip_ctx, &diag_ctx->message,
          diag_ctx->downstream_data, diag_ctx->downstream_data_len);
      BREAK_IF(err == E_AGAIN);
      if(err != E_OK) {
        router_doip_reset(diag_ctx);
        break;
      }
      diag_ctx->response_pending = !diag_ctx->obd2 && diag_ctx->downstream_data_len == 3u &&
          diag_ctx->downstream_data[0] == 0x7Fu && diag_ctx->downstream_data[2] == 0x78u;
      diag_ctx->state = diag_ctx->response_pending ? ROUTER_DOIP_STATE_RESPONSE : ROUTER_DOIP_STATE_IDLE;
      diag_ctx->request_time = time_now_us();
    }

  } while(0);
}

void router_handle_doip(router_ctx_t *ctx)
{
  for(uint32_t i = 0; i < ECU_COMM_DOIP_MAX; i++) {
    if(ctx->diag.doip_ctx[i].active) {
      router_handle_diag_protocol_doip(&ctx->diag.doip_ctx[i]);
    }
  }
}
