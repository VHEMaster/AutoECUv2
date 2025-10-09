/*
 * isotp_fsm.c
 *
 *  Created on: Oct 9, 2025
 *      Author: VHEMaster
 */

#include "isotp_fsm.h"
#include "isotp_private.h"

#define ISOTP_MESSAGE_TYPE_MASK 0xF0
#define ISOTP_MESSAGE_TYPE_SF   0x00
#define ISOTP_MESSAGE_TYPE_FF   0x10
#define ISOTP_MESSAGE_TYPE_FC   0x30
#define ISOTP_MESSAGE_TYPE_CF   0x20

#define ISOTP_FLOW_STATUS_MASK  0x0F
#define ISOTP_FLOW_STATUS_CTS   0x00
#define ISOTP_FLOW_STATUS_WT    0x01
#define ISOTP_FLOW_STATUS_OVF   0x02

static error_t isotp_fsm_idle(isotp_ctx_t *ctx);
static error_t isotp_fsm_rx_sf_ff(isotp_ctx_t *ctx);
static error_t isotp_fsm_tx_sf_ff(isotp_ctx_t *ctx);
static error_t isotp_fsm_tx_wait_fc(isotp_ctx_t *ctx);
static error_t isotp_fsm_tx_cf(isotp_ctx_t *ctx);

void isotp_fsm(isotp_ctx_t *ctx)
{
  static isotp_state_t state_prev = ISOTP_STATE_IDLE;

  error_t err;

  while(true) {
    err = E_AGAIN;
    switch(ctx->state) {
      case ISOTP_STATE_IDLE:
        err = isotp_fsm_idle(ctx);
        break;
      case ISOTP_STATE_RX_SF_FF:
        err = isotp_fsm_rx_sf_ff(ctx);
        break;
      case ISOTP_STATE_TX_SF_FF:
        err = isotp_fsm_tx_sf_ff(ctx);
        break;
      case ISOTP_STATE_TX_WAIT_FC:
        err = isotp_fsm_tx_wait_fc(ctx);
        break;
      case ISOTP_STATE_TX_CF:
        err = isotp_fsm_tx_cf(ctx);
        break;
      default:
        break;
    }

    if(state_prev != ctx->state) {
      state_prev = ctx->state;
      ctx->state_time = time_now_us();
    }

    if(err == E_AGAIN) {
      continue;
    }
    break;
  }
}


static error_t isotp_fsm_idle(isotp_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    if(!ctx->data_upstream.ready && isotp_frame_has_data(&ctx->frame_fifo_upstream)) {
      ctx->local_error_code = ISOTP_MAX;
      ctx->state = ISOTP_STATE_RX_SF_FF;
      err = E_AGAIN;
      break;
    }

    if(ctx->data_downstream.ready && isotp_frame_get_free_space(&ctx->frame_fifo_downstream)) {
      ctx->local_error_code = ISOTP_MAX;
      ctx->state = ISOTP_STATE_TX_SF_FF;
      err = E_AGAIN;
      break;
    }
  } while(0);

  return err;
}

static error_t isotp_fsm_rx_sf_ff(isotp_ctx_t *ctx)
{
  error_t err = E_OK;

  do {

  } while(0);

  return err;
}

static error_t isotp_fsm_tx_sf_ff(isotp_ctx_t *ctx)
{
  error_t err = E_OK;
  isotp_fc_t *fc;
  isotp_data_t *data;
  isotp_frame_t *frame;
  isotp_frame_fifo_t *frame_fifo;

  uint16_t datalen;
  uint16_t msglen;
  uint32_t write;

  do {
    data = &ctx->data_upstream;
    datalen = data->length;
    msglen = data->length;

    frame_fifo = &ctx->frame_fifo_downstream;
    write = frame_fifo->write;
    frame = &frame_fifo->buffer[write];

    fc = &ctx->flowcontrol;

    if(datalen <= ISOTP_FRAME_LEN - 1) {
      msglen = datalen;
      frame->payload[0] = ISOTP_MESSAGE_TYPE_SF;
      frame->payload[0] |= datalen & ~ISOTP_MESSAGE_TYPE_MASK;
      memcpy(&frame->payload[1], data->payload, msglen);
      memset(&frame->payload[msglen + 1], 0x00, ISOTP_FRAME_LEN - 1 - msglen);

      if(++write >= ISOTP_FRAME_FIFO_LEN) {
        write = 0;
      }
      frame_fifo->write = write;
      ctx->state = ISOTP_STATE_IDLE;
      err = E_AGAIN;
    } else {
      msglen = ISOTP_FRAME_LEN - 2;
      frame->payload[0] = ISOTP_MESSAGE_TYPE_FF;
      frame->payload[0] |= (datalen >> 8) & ~ISOTP_MESSAGE_TYPE_MASK;
      frame->payload[1] = datalen & 0xFF;
      memcpy(&frame->payload[2], data->payload, msglen);
      if(++write >= ISOTP_FRAME_FIFO_LEN) {
        write = 0;
      }
      frame_fifo->write = write;

      fc->left = datalen - msglen;
      fc->pos = msglen;
      fc->sn = 1;

      ctx->state = ISOTP_STATE_TX_WAIT_FC;
    }
  } while(0);

  return err;
}

static error_t isotp_fsm_tx_wait_fc(isotp_ctx_t *ctx)
{
  error_t err = E_OK;
  isotp_fc_t *fc;
  isotp_frame_t *frame_rx;
  isotp_frame_fifo_t *frame_fifo_rx;
  time_us_t now;

  uint8_t fcd;
  uint8_t fs;
  uint8_t bs;
  uint8_t st;
  uint32_t read;

  do {
    frame_fifo_rx = &ctx->frame_fifo_upstream;
    now = time_now_us();

    if(isotp_frame_has_data(frame_fifo_rx)) {
      read = frame_fifo_rx->read;
      frame_rx = &frame_fifo_rx->buffer[read];

      fcd = frame_rx->payload[0] & ISOTP_MESSAGE_TYPE_FC;

      if(++read >= ISOTP_FRAME_FIFO_LEN) {
        read = 0;
      }
      frame_fifo_rx->read = read;

      if(fcd == ISOTP_MESSAGE_TYPE_FC) {
        fs = frame_rx->payload[0] & ISOTP_FLOW_STATUS_MASK;
        if(fs == ISOTP_FLOW_STATUS_CTS) {
          fc = &ctx->flowcontrol;
          bs = frame_rx->payload[1];
          st = frame_rx->payload[2];

          fc->bs = bs;
          fc->safc = false;

          if(st <= 0x7F) {
            fc->separation_time = st * TIME_US_IN_MS;
          } else if(st >= 0xF1 && st <= 0xF9) {
            fc->separation_time = (st - 0xF0) * 100;
          } else {
            fc->separation_time = 0;
          }

          ctx->state = ISOTP_STATE_TX_CF;
          err = E_AGAIN;

        } else if(fs == ISOTP_FLOW_STATUS_WT) {
          ctx->state_time = now;
        } else if(fs == ISOTP_FLOW_STATUS_OVF) {
          ctx->local_error_code = ISOTP_OVERFLOW;
        } else {
          ctx->local_error_code = ISOTP_INVALID_FS;
        }
      } else {
        ctx->local_error_code = ISOTP_UNEXPECTED_PDU;
      }
    } else if(time_diff(now, ctx->state_time) > ctx->config.timeout) {
      ctx->local_error_code = ISOTP_TIMEOUT_BS;
    }

  } while(0);

  if(ctx->local_error_code != ISOTP_MAX) {
    ctx->data_downstream.ready = false;
    ctx->state = ISOTP_STATE_IDLE;
    ctx->error_code = ctx->local_error_code;
    err = E_AGAIN;
  }

  return err;

}
static error_t isotp_fsm_tx_cf(isotp_ctx_t *ctx)
{
  error_t err = E_OK;
  isotp_fc_t *fc;
  isotp_data_t *data;
  isotp_frame_t *frame;
  isotp_frame_fifo_t *frame_fifo;
  time_us_t now;

  uint16_t msglen;
  uint16_t left;
  uint16_t pos;
  uint16_t sn;
  uint32_t write;

  do {
    data = &ctx->data_upstream;

    frame_fifo = &ctx->frame_fifo_downstream;
    write = frame_fifo->write;
    fc = &ctx->flowcontrol;

    while(true) {
      now = time_now_us();

      if(fc->left == 0) {
        ctx->local_error_code = ISOTP_OK;
        break;
      } else if(isotp_frame_get_free_space(frame_fifo)) {
        if(fc->separation_time == 0 || fc->safc == false || time_diff(now, ctx->state_time) >= fc->separation_time) {
          frame = &frame_fifo->buffer[write];

          pos = fc->pos;
          left = fc->left;
          sn = fc->sn;

          msglen = ISOTP_FRAME_LEN - 1;
          if(left < msglen) {
            msglen = left;

            memset(&frame->payload[msglen + 1], 0x00, ISOTP_FRAME_LEN - 1 - msglen);
          }

          frame->payload[0] = ISOTP_MESSAGE_TYPE_CF;
          frame->payload[0] |= sn & ~ISOTP_MESSAGE_TYPE_MASK;
          memcpy(&frame->payload[1], &data->payload[pos], msglen);

          if(++write >= ISOTP_FRAME_FIFO_LEN) {
            write = 0;
          }
          frame_fifo->write = write;

          fc->left = left - msglen;
          fc->pos = pos + msglen;
          fc->sn = sn + 1;
          fc->bn++;
          fc->safc = true;
          ctx->state_time = now;

          if(fc->bs != 0 && fc->bn >= fc->bs) {
            ctx->state = ISOTP_STATE_TX_WAIT_FC;
            break;
          } else {
            continue;
          }
        } else {
          break;
        }
      } else if(time_diff(now, ctx->state_time) > ctx->config.timeout) {
        ctx->local_error_code = ISOTP_TIMEOUT_AS;
      }
    }
    ctx->state = ISOTP_STATE_TX_WAIT_FC;
  } while(0);

  if(ctx->local_error_code != ISOTP_MAX) {
    ctx->data_downstream.ready = false;
    ctx->state = ISOTP_STATE_IDLE;
    ctx->error_code = ctx->local_error_code;
    err = E_AGAIN;
  }

  return err;
}

static error_t isotp_fsm_(isotp_ctx_t *ctx)
{
  error_t err = E_OK;

  do {

  } while(0);

  return err;
}
