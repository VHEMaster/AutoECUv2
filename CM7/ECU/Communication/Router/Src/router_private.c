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

static void router_handle_diag_pair_can_isotp(router_ctx_t *ctx, router_config_can_isotp_pairs_t pair);
static void router_handle_diag_pairs(router_ctx_t *ctx);

static void router_handle_diag_protocol_isotp(router_ctx_t *ctx, ecu_comm_isotp_t instance);
static void router_handle_diag_protocol_kwp(router_ctx_t *ctx, ecu_comm_kwp_t instance);
static void router_handle_diag_protocols(router_ctx_t *ctx);

static error_t router_configure_diag_pair_can_isotp(router_ctx_t *ctx, router_config_can_isotp_pairs_t pair);
static error_t router_configure_diag_pair_isotp_uds(router_ctx_t *ctx, router_config_isotp_uds_pairs_t pair);
static error_t router_configure_diag_pair_kwp_uds(router_ctx_t *ctx, router_config_kwp_uds_pairs_t pair);
static error_t router_configure_diag_pair_isotp_obd2(router_ctx_t *ctx, router_config_isotp_obd2_pairs_t pair);
static error_t router_configure_diag_pair_kwp_obd2(router_ctx_t *ctx, router_config_kwp_obd2_pairs_t pair);
static error_t router_configure_diag_pairs(router_ctx_t *ctx);

static void  router_reset_isotp(router_diag_can_isotp_pair_ctx_t *pair_ctx);

static void router_can_isotp_rx_callback(can_ctx_t *ctx, const can_message_t *message, void *usrdata)
{
  error_t err = E_OK;
  const router_config_can_isotp_pair_t *pair_cfg;
  router_diag_can_isotp_pair_ctx_t *pair_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(message == NULL);
    BREAK_IF(usrdata == NULL);

    pair_ctx = (router_diag_can_isotp_pair_ctx_t *)usrdata;
    pair_cfg = pair_ctx->pair_config;
    BREAK_IF(pair_cfg == NULL);
    BREAK_IF(pair_cfg->upstream_msg_id != message->id);

    err = isotp_frame_write_upstream(pair_ctx->isotp_ctx, (const isotp_frame_t *)message->payload);
    BREAK_IF(err != E_OK);

  } while(0);
}

static void router_can_isotp_err_callback(can_ctx_t *ctx, void *usrdata)
{
  const router_config_can_isotp_pair_t *pair_cfg;
  router_diag_can_isotp_pair_ctx_t *pair_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(usrdata == NULL);

    pair_ctx = (router_diag_can_isotp_pair_ctx_t *)usrdata;
    pair_cfg = pair_ctx->pair_config;
    BREAK_IF(pair_cfg == NULL);

    (void)isotp_reset(pair_ctx->isotp_ctx);
    if(pair_ctx->isotp_link_uds_ctx != NULL) {
      (void)uds_reset(pair_ctx->isotp_link_uds_ctx);
    }
    if(pair_ctx->isotp_link_obd2_ctx != NULL) {
      (void)obd2_reset(pair_ctx->isotp_link_obd2_ctx);
    }

  } while(0);
}

static void router_handle_diag_pair_can_isotp(router_ctx_t *ctx, router_config_can_isotp_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_can_isotp_pair_t *pair_cfg;
  router_diag_can_isotp_pair_ctx_t *pair_ctx;
  isotp_error_code_t isotp_err;

  do {
    BREAK_IF(ctx == NULL);
    pair_ctx = &ctx->diag.can_isotp_pairs[pair];
    pair_cfg = &ctx->config.diagnostics.can_isotp_pairs[pair];

    while(true) {
      err = ecu_comm_isotp_get_error_flag(pair_cfg->isotp_instance, &isotp_err);
      if(err == E_OK) {
        if(isotp_err != ISOTP_OK) {
          router_reset_isotp(pair_ctx);
          pair_ctx->message_downstream_pending = false;
          break;
        }
      }

      if(pair_ctx->message_downstream_pending == false) {
        err = isotp_frame_read_downstream(pair_ctx->isotp_ctx, (isotp_frame_t *)pair_ctx->message_downstream.payload);
        BREAK_IF(err != E_OK);
        pair_ctx->message_downstream_pending = true;

        pair_ctx->message_downstream.id = pair_cfg->downstream_msg_id;
        pair_ctx->message_downstream.len = ISOTP_FRAME_LEN;
      }

      if(pair_ctx->message_downstream_pending == true) {
        err = can_tx(pair_ctx->can_ctx, &pair_ctx->message_downstream);
        if(err != E_OK) {
          BREAK_IF(err == E_AGAIN);
          (void)can_reset(pair_ctx->can_ctx);
          (void)isotp_reset(pair_ctx->isotp_ctx);
        }
        pair_ctx->message_downstream_pending = false;
      }
    }

  } while(0);
}

static void router_handle_diag_pairs(router_ctx_t *ctx)
{
  for(router_config_can_isotp_pairs_t p = 0; p < ROUTER_CONFIG_CAN_ISOTP_PAIR_MAX; p++) {
    if(ctx->diag.can_isotp_pairs[p].active == true) {
      router_handle_diag_pair_can_isotp(ctx, p);
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

    diag_ctx->data_len = ISOTP_PAYLOAD_LEN_MAX;
    err = isotp_data_read_upstream(diag_ctx->isotp_ctx, diag_ctx->data, &diag_ctx->data_len);
    if(err == E_OK) {
      // TODO: IMPLEMENT
    } else if(err != E_AGAIN) {
      router_reset_isotp(&ctx->diag.can_isotp_pairs[diag_ctx->pair_can]);

      // TODO: IMPLEMENT

      break;
    }

  } while(0);
}

static void router_handle_diag_protocol_kwp(router_ctx_t *ctx, ecu_comm_kwp_t instance)
{
  // TODO: IMPLEMENT
}

static error_t router_configure_diag_pair_can_isotp(router_ctx_t *ctx, router_config_can_isotp_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_can_isotp_pair_t *pair_cfg;
  router_diag_can_isotp_pair_ctx_t *pair_ctx;
  router_diag_isotp_ctx_t *diag_isotp_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    pair_cfg = &ctx->config.diagnostics.can_isotp_pairs[pair];
    pair_ctx = &ctx->diag.can_isotp_pairs[pair];
    memset(pair_ctx, 0, sizeof(*pair_ctx));
    pair_ctx->pair_config = pair_cfg;
    diag_isotp_ctx = &ctx->diag.isotp_ctx[pair_cfg->isotp_instance];

    if(pair_cfg->enabled == true) {
      err = ecu_comm_get_can_ctx(pair_cfg->can_instance, &pair_ctx->can_ctx);
      BREAK_IF(err != E_OK);
      err = ecu_comm_get_isotp_ctx(pair_cfg->isotp_instance, &pair_ctx->isotp_ctx);
      BREAK_IF(err != E_OK);

      diag_isotp_ctx->isotp_ctx = pair_ctx->isotp_ctx;
      diag_isotp_ctx->pair_can = pair;

      for(router_config_isotp_uds_pairs_t p = 0; p < ROUTER_CONFIG_ISOTP_UDS_PAIR_MAX; p++) {
        const router_config_isotp_uds_pair_t *uds_pair_cfg = &ctx->config.diagnostics.isotp_uds_pairs[p];
        if(uds_pair_cfg->enabled == true && uds_pair_cfg->isotp_instance == pair_cfg->isotp_instance) {
          diag_isotp_ctx->pair_uds = p;
          if(pair_ctx->isotp_link_uds_ctx == NULL) {
            err = ecu_comm_get_uds_ctx(uds_pair_cfg->uds_instance, &pair_ctx->isotp_link_uds_ctx);
            BREAK_IF(err != E_OK);
            diag_isotp_ctx->uds_ctx = pair_ctx->isotp_link_uds_ctx;
          } else {
            err = E_PARAM;
            break;
          }
        }
      }
      BREAK_IF(err != E_OK);

      for(router_config_isotp_obd2_pairs_t p = 0; p < ROUTER_CONFIG_ISOTP_OBD2_PAIR_MAX; p++) {
        const router_config_isotp_obd2_pair_t *obd2_pair_cfg = &ctx->config.diagnostics.isotp_obd2_pairs[p];
        if(obd2_pair_cfg->enabled == true && obd2_pair_cfg->isotp_instance == pair_cfg->isotp_instance) {
          diag_isotp_ctx->pair_obd2 = p;
          if(pair_ctx->isotp_link_obd2_ctx == NULL) {
            err = ecu_comm_get_obd2_ctx(obd2_pair_cfg->obd2_instance, &pair_ctx->isotp_link_obd2_ctx);
            BREAK_IF(err != E_OK);
            diag_isotp_ctx->obd2_ctx = pair_ctx->isotp_link_obd2_ctx;
          } else {
            err = E_PARAM;
            break;
          }
        }
      }
      BREAK_IF(err != E_OK);

      err = can_register_rx_callback(pair_ctx->can_ctx, pair_cfg->upstream_msg_id, router_can_isotp_rx_callback, pair_ctx);
      BREAK_IF(err != E_OK);
      err = can_register_err_callback(pair_ctx->can_ctx, router_can_isotp_err_callback, pair_ctx);
      BREAK_IF(err != E_OK);

      diag_isotp_ctx->active = true;
      pair_ctx->active = true;
    }

  } while(0);

  return err;
}

static error_t router_configure_diag_pair_isotp_uds(router_ctx_t *ctx, router_config_isotp_uds_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_isotp_uds_pair_t *pair_cfg;
  router_diag_isotp_uds_pair_ctx_t *pair_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    pair_cfg = &ctx->config.diagnostics.isotp_uds_pairs[pair];
    pair_ctx = &ctx->diag.isotp_uds_pairs[pair];
    memset(pair_ctx, 0, sizeof(*pair_ctx));
    pair_ctx->pair_config = pair_cfg;

    if(pair_cfg->enabled == true) {
      err = ecu_comm_get_isotp_ctx(pair_cfg->isotp_instance, &pair_ctx->isotp_ctx);
      BREAK_IF(err != E_OK);
      err = ecu_comm_get_uds_ctx(pair_cfg->uds_instance, &pair_ctx->uds_ctx);
      BREAK_IF(err != E_OK);
      pair_ctx->active = true;
    }

  } while(0);

  return err;
}

static error_t router_configure_diag_pair_kwp_uds(router_ctx_t *ctx, router_config_kwp_uds_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_kwp_uds_pair_t *pair_cfg;
  router_diag_kwp_uds_pair_ctx_t *pair_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    pair_cfg = &ctx->config.diagnostics.kwp_uds_pairs[pair];
    pair_ctx = &ctx->diag.kwp_uds_pairs[pair];
    memset(pair_ctx, 0, sizeof(*pair_ctx));
    pair_ctx->pair_config = pair_cfg;

    if(pair_cfg->enabled == true) {
      err = ecu_comm_get_kwp_ctx(pair_cfg->kwp_instance, &pair_ctx->kwp_ctx);
      BREAK_IF(err != E_OK);
      err = ecu_comm_get_uds_ctx(pair_cfg->uds_instance, &pair_ctx->uds_ctx);
      BREAK_IF(err != E_OK);
      pair_ctx->active = true;
    }

  } while(0);

  return err;
}

static error_t router_configure_diag_pair_isotp_obd2(router_ctx_t *ctx, router_config_isotp_obd2_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_isotp_obd2_pair_t *pair_cfg;
  router_diag_isotp_obd2_pair_ctx_t *pair_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    pair_cfg = &ctx->config.diagnostics.isotp_obd2_pairs[pair];
    pair_ctx = &ctx->diag.isotp_obd2_pairs[pair];
    memset(pair_ctx, 0, sizeof(*pair_ctx));
    pair_ctx->pair_config = pair_cfg;

    if(pair_cfg->enabled == true) {
      err = ecu_comm_get_isotp_ctx(pair_cfg->isotp_instance, &pair_ctx->isotp_ctx);
      BREAK_IF(err != E_OK);
      err = ecu_comm_get_obd2_ctx(pair_cfg->obd2_instance, &pair_ctx->obd2_ctx);
      BREAK_IF(err != E_OK);
      pair_ctx->active = true;
    }

  } while(0);

  return err;
}

static error_t router_configure_diag_pair_kwp_obd2(router_ctx_t *ctx, router_config_kwp_obd2_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_kwp_obd2_pair_t *pair_cfg;
  router_diag_kwp_obd2_pair_ctx_t *pair_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    pair_cfg = &ctx->config.diagnostics.kwp_obd2_pairs[pair];
    pair_ctx = &ctx->diag.kwp_obd2_pairs[pair];
    memset(pair_ctx, 0, sizeof(*pair_ctx));
    pair_ctx->pair_config = pair_cfg;

    if(pair_cfg->enabled == true) {
      err = ecu_comm_get_kwp_ctx(pair_cfg->kwp_instance, &pair_ctx->kwp_ctx);
      BREAK_IF(err != E_OK);
      err = ecu_comm_get_obd2_ctx(pair_cfg->obd2_instance, &pair_ctx->obd2_ctx);
      BREAK_IF(err != E_OK);
      pair_ctx->active = true;
    }

  } while(0);

  return err;
}

static error_t router_configure_diag_pairs(router_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    memset(&ctx->diag, 0, sizeof(ctx->diag));

    for(router_config_can_isotp_pairs_t p = 0; p < ROUTER_CONFIG_CAN_ISOTP_PAIR_MAX; p++) {
      err = router_configure_diag_pair_can_isotp(ctx, p);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(router_config_isotp_uds_pairs_t p = 0; p < ROUTER_CONFIG_ISOTP_UDS_PAIR_MAX; p++) {
      err = router_configure_diag_pair_isotp_uds(ctx, p);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(router_config_kwp_uds_pairs_t p = 0; p < ROUTER_CONFIG_KWP_UDS_PAIR_MAX; p++) {
      err = router_configure_diag_pair_kwp_uds(ctx, p);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(router_config_isotp_obd2_pairs_t p = 0; p < ROUTER_CONFIG_ISOTP_OBD2_PAIR_MAX; p++) {
      err = router_configure_diag_pair_isotp_obd2(ctx, p);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(router_config_kwp_obd2_pairs_t p = 0; p < ROUTER_CONFIG_KWP_OBD2_PAIR_MAX; p++) {
      err = router_configure_diag_pair_kwp_obd2(ctx, p);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

static void  router_reset_isotp(router_diag_can_isotp_pair_ctx_t *pair_ctx)
{
  (void)can_reset(pair_ctx->can_ctx);
  (void)isotp_reset(pair_ctx->isotp_ctx);

  if(pair_ctx->isotp_link_uds_ctx != NULL) {
    (void)uds_reset(pair_ctx->isotp_link_uds_ctx);
  }
  if(pair_ctx->isotp_link_obd2_ctx != NULL) {
    (void)obd2_reset(pair_ctx->isotp_link_obd2_ctx);
  }
}

error_t router_configure_diag(router_ctx_t *ctx)
{
  error_t err = E_OK;
  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    memset(&ctx->diag, 0, sizeof(ctx->diag));

    err = router_configure_diag_pairs(ctx);
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

    router_handle_diag_pairs(ctx);
    router_handle_diag_protocols(ctx);

  } while(0);
}

void router_handle_signals(router_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);


  } while(0);
}
