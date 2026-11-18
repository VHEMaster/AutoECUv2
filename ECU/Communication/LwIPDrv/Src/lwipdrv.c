/*
 * lwipdrv.c
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "lwipdrv.h"
#include "time.h"

static uint32_t lwipdrv_ppp_output_cb(ppp_pcb *pcb, const void *data, uint32_t len, void *arg)
{
  uint32_t ret = 0u;
  error_t err = E_OK;
  lwipdrv_ctx_t *ctx = arg;

  err = rs422_tx(ctx->init.rs422_ctx, data, len);
  if(err == E_OK) {
    ret = len;
  }

  return ret;

}

static void lwipdrv_ppp_link_cb(ppp_pcb *pcb, int status, void *arg)
{
  lwipdrv_ctx_t *ctx = arg;

  ctx->lwip.link_up = status == PPPERR_NONE;
  if(status != PPPERR_NONE && ctx->configured) {
    ctx->lwip.restart_pending = true;
  }

  (void)pcb;
}

uint32_t sys_now(void)
{
   return HAL_GetTick();
}

uint32_t sys_jiffies(void)
{
   return HAL_GetTick();
}

int rand(void)
{
  static int state = 0xA341316Cu;

  state ^= state << 13u;
  state ^= state >> 17u;
  state ^= state << 5u;
  state ^= time_now_tick();

  return state & RAND_MAX;
}

error_t lwipdrv_init(lwipdrv_ctx_t *ctx, const lwipdrv_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init->rs422_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(lwipdrv_ctx_t));
    memcpy(&ctx->init, init, sizeof(lwipdrv_init_ctx_t));

    lwip_init();

    ctx->lwip.ppp = pppos_create(&ctx->lwip.ppp_netif,
        lwipdrv_ppp_output_cb, lwipdrv_ppp_link_cb, ctx);
    BREAK_IF_ACTION(ctx->lwip.ppp == NULL, err = E_FAULT);

    doip_lwip_init(&ctx->doip, ctx->init.doip_ctx, &ctx->lwip.ppp_netif);

    ctx->initialized = true;

  } while(0);

  return err;
}

error_t lwipdrv_configure(lwipdrv_ctx_t *ctx, const lwipdrv_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    memcpy(&ctx->config, config, sizeof(lwipdrv_config_t));
    ctx->configured = ctx->config.enabled;
    ctx->reset_trigger = true;

  } while(0);

  return err;
}

static void lwipdrv_ppp_restart(lwipdrv_ctx_t *ctx)
{
  err_t err;

  do {
    if(ctx->reset_trigger) {
      ctx->reset_trigger = false;
      ctx->lwip.restart_pending = true;
      ctx->lwip.close_pending = false;
      ctx->lwip.link_up = false;
      doip_lwip_stop(&ctx->doip);
    }
    BREAK_IF(!ctx->lwip.restart_pending);

    if(ctx->lwip.ppp->phase != PPP_PHASE_DEAD) {
      if(!ctx->lwip.close_pending) {
        (void)ppp_close(ctx->lwip.ppp, 0u);
        ctx->lwip.close_pending = true;
      }
      break;
    }
    ctx->lwip.close_pending = false;
    if(!ctx->configured) {
      ctx->lwip.restart_pending = false;
      break;
    }

    IP4_ADDR(&ctx->lwip.addr_our, ctx->config.ip_addr_our[0], ctx->config.ip_addr_our[1], ctx->config.ip_addr_our[2], ctx->config.ip_addr_our[3]);
    IP4_ADDR(&ctx->lwip.addr_his, ctx->config.ip_addr_his[0], ctx->config.ip_addr_his[1], ctx->config.ip_addr_his[2], ctx->config.ip_addr_his[3]);
    ppp_set_ipcp_ouraddr(ctx->lwip.ppp, &ctx->lwip.addr_our);
    ppp_set_ipcp_hisaddr(ctx->lwip.ppp, &ctx->lwip.addr_his);
    ppp_set_silent(ctx->lwip.ppp, true);

    err = ppp_listen(ctx->lwip.ppp);
    BREAK_IF(err != ERR_OK);

    ppp_set_default(ctx->lwip.ppp);
    ctx->lwip.restart_pending = false;

  } while(0);
}

error_t lwipdrv_reset(lwipdrv_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    ctx->reset_trigger = true;

  } while(0);

  return err;
}

void lwipdrv_loop_slow(lwipdrv_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void lwipdrv_loop_comm(lwipdrv_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

  } while(0);
}

void lwipdrv_loop_main(lwipdrv_ctx_t *ctx)
{
  error_t err = E_OK;
  uint32_t bytes;
  const void *buffer;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->initialized == false);

    lwipdrv_ppp_restart(ctx);

    sys_check_timeouts();

    bytes = 512u;
    err = rs422_rx_nocopy(ctx->init.rs422_ctx, &buffer, &bytes);
    if(err == E_OK && bytes > 0u) {
      pppos_input(ctx->lwip.ppp, buffer, bytes);
    }

    doip_lwip_loop(&ctx->doip, ctx->configured && ctx->lwip.link_up && !ctx->lwip.restart_pending);

  } while(0);
}
