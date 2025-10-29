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
static void router_handle_diag_pair_isotp_uds(router_ctx_t *ctx, router_config_isotp_uds_pairs_t pair);
static void router_handle_diag_pair_kwp_uds(router_ctx_t *ctx, router_config_kwp_uds_pairs_t pair);
static void router_handle_diag_pair_isotp_obd2(router_ctx_t *ctx, router_config_isotp_obd2_pairs_t pair);
static void router_handle_diag_pair_kwp_obd2(router_ctx_t *ctx, router_config_kwp_obd2_pairs_t pair);
static void router_handle_diag_pairs(router_ctx_t *ctx);

static error_t router_configure_diag_pair_can_isotp(router_ctx_t *ctx, router_config_can_isotp_pairs_t pair);
static error_t router_configure_diag_pair_isotp_uds(router_ctx_t *ctx, router_config_isotp_uds_pairs_t pair);
static error_t router_configure_diag_pair_kwp_uds(router_ctx_t *ctx, router_config_kwp_uds_pairs_t pair);
static error_t router_configure_diag_pair_isotp_obd2(router_ctx_t *ctx, router_config_isotp_obd2_pairs_t pair);
static error_t router_configure_diag_pair_kwp_obd2(router_ctx_t *ctx, router_config_kwp_obd2_pairs_t pair);
static error_t router_configure_diag_pairs(router_ctx_t *ctx);

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
  error_t err = E_OK;
  const router_config_can_isotp_pair_t *pair_cfg;
  router_diag_can_isotp_pair_ctx_t *pair_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(usrdata == NULL);

    pair_ctx = (router_diag_can_isotp_pair_ctx_t *)usrdata;
    pair_cfg = pair_ctx->pair_config;
    BREAK_IF(pair_cfg == NULL);

    err = isotp_reset(pair_ctx->isotp_ctx);
    BREAK_IF(err != E_OK);

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
          (void)can_reset(pair_ctx->can_ctx);
          (void)isotp_reset(pair_ctx->isotp_ctx);
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

static void router_handle_diag_pair_isotp_uds(router_ctx_t *ctx, router_config_isotp_uds_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_isotp_uds_pair_t *pair_cfg;
  router_diag_isotp_uds_pair_ctx_t *pair_ctx;

  (void)err;
  (void)pair_cfg;
  (void)pair_ctx;

  do {
    BREAK_IF(ctx == NULL);
    pair_ctx = &ctx->diag.isotp_uds_pairs[pair];
    pair_cfg = &ctx->config.diagnostics.isotp_uds_pairs[pair];

    // TODO: IMPLEMENT

  } while(0);
}

static void router_handle_diag_pair_kwp_uds(router_ctx_t *ctx, router_config_kwp_uds_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_kwp_uds_pair_t *pair_cfg;
  router_diag_kwp_uds_pair_ctx_t *pair_ctx;

  (void)err;
  (void)pair_cfg;
  (void)pair_ctx;

  do {
    BREAK_IF(ctx == NULL);
    pair_ctx = &ctx->diag.kwp_uds_pairs[pair];
    pair_cfg = &ctx->config.diagnostics.kwp_uds_pairs[pair];

    // TODO: IMPLEMENT

  } while(0);
}

static void router_handle_diag_pair_isotp_obd2(router_ctx_t *ctx, router_config_isotp_obd2_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_isotp_obd2_pair_t *pair_cfg;
  router_diag_isotp_obd2_pair_ctx_t *pair_ctx;

  (void)err;
  (void)pair_cfg;
  (void)pair_ctx;

  do {
    BREAK_IF(ctx == NULL);
    pair_ctx = &ctx->diag.isotp_obd2_pairs[pair];
    pair_cfg = &ctx->config.diagnostics.isotp_obd2_pairs[pair];

    // TODO: IMPLEMENT

  } while(0);
}

static void router_handle_diag_pair_kwp_obd2(router_ctx_t *ctx, router_config_kwp_obd2_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_kwp_obd2_pair_t *pair_cfg;
  router_diag_kwp_obd2_pair_ctx_t *pair_ctx;

  (void)err;
  (void)pair_cfg;
  (void)pair_ctx;

  do {
    BREAK_IF(ctx == NULL);
    pair_ctx = &ctx->diag.kwp_obd2_pairs[pair];
    pair_cfg = &ctx->config.diagnostics.kwp_obd2_pairs[pair];

    // TODO: IMPLEMENT

  } while(0);
}


static void router_handle_diag_pairs(router_ctx_t *ctx)
{
  for(router_config_can_isotp_pairs_t p = 0; p < ROUTER_CONFIG_CAN_ISOTP_PAIR_MAX; p++) {
    if(ctx->diag.can_isotp_pairs[p].active == true) {
      router_handle_diag_pair_can_isotp(ctx, p);
    }
  }
  for(router_config_isotp_uds_pairs_t p = 0; p < ROUTER_CONFIG_ISOTP_UDS_PAIR_MAX; p++) {
    if(ctx->diag.isotp_uds_pairs[p].active == true) {
      router_handle_diag_pair_isotp_uds(ctx, p);
    }
  }
  for(router_config_kwp_uds_pairs_t p = 0; p < ROUTER_CONFIG_KWP_UDS_PAIR_MAX; p++) {
    if(ctx->diag.kwp_uds_pairs[p].active == true) {
      router_handle_diag_pair_kwp_uds(ctx, p);
    }
  }
  for(router_config_isotp_obd2_pairs_t p = 0; p < ROUTER_CONFIG_ISOTP_OBD2_PAIR_MAX; p++) {
    if(ctx->diag.isotp_obd2_pairs[p].active == true) {
      router_handle_diag_pair_isotp_obd2(ctx, p);
    }
  }
  for(router_config_kwp_obd2_pairs_t p = 0; p < ROUTER_CONFIG_KWP_OBD2_PAIR_MAX; p++) {
    if(ctx->diag.kwp_obd2_pairs[p].active == true) {
      router_handle_diag_pair_kwp_obd2(ctx, p);
    }
  }
}

static error_t router_configure_diag_pair_can_isotp(router_ctx_t *ctx, router_config_can_isotp_pairs_t pair)
{
  error_t err = E_OK;
  const router_config_can_isotp_pair_t *pair_cfg;
  router_diag_can_isotp_pair_ctx_t *pair_ctx;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    pair_cfg = &ctx->config.diagnostics.can_isotp_pairs[pair];
    pair_ctx = &ctx->diag.can_isotp_pairs[pair];
    memset(pair_ctx, 0, sizeof(*pair_ctx));
    pair_ctx->pair_config = pair_cfg;

    if(pair_cfg->enabled == true) {
      err = ecu_comm_get_can_ctx(pair_cfg->can_instance, &pair_ctx->can_ctx);
      BREAK_IF(err != E_OK);
      err = ecu_comm_get_isotp_ctx(pair_cfg->isotp_instance, &pair_ctx->isotp_ctx);
      BREAK_IF(err != E_OK);

      err = can_register_rx_callback(pair_ctx->can_ctx, pair_cfg->upstream_msg_id, router_can_isotp_rx_callback, pair_ctx);
      BREAK_IF(err != E_OK);
      err = can_register_err_callback(pair_ctx->can_ctx, router_can_isotp_err_callback, pair_ctx);
      BREAK_IF(err != E_OK);

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

  } while(0);
}

void router_handle_signals(router_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);


  } while(0);
}
