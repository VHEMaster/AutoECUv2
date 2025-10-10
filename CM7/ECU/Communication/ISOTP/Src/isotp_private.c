/*
 * isotp_private.c
 *
 *  Created on: Oct 9, 2025
 *      Author: VHEMaster
 */

#include "isotp_private.h"

INLINE error_t isotp_frame_fifo_push(isotp_frame_fifo_t *fifo, const isotp_frame_t *frame)
{
  error_t err = E_OK;
  uint32_t read, write, cur;

  do {
    BREAK_IF_ACTION(fifo == NULL, err = E_PARAM);
    BREAK_IF_ACTION(frame == NULL, err = E_PARAM);

    read = fifo->read;
    write = fifo->write;
    cur = write;

    if(++write >= ISOTP_FRAME_FIFO_LEN) {
      write = 0;
    }

    if(write == read) {
      err = E_OVERFLOW;
    } else {
      memcpy(&fifo->buffer[cur], frame, sizeof(isotp_frame_t));
      fifo->write = write;
    }

  } while(0);

  return err;
}

INLINE error_t isotp_frame_fifo_pull(isotp_frame_fifo_t *fifo, isotp_frame_t *frame)
{
  error_t err = E_OK;
  uint32_t read, write;

  do {
    BREAK_IF_ACTION(fifo == NULL, err = E_PARAM);
    BREAK_IF_ACTION(frame == NULL, err = E_PARAM);

    read = fifo->read;
    write = fifo->write;

    if(read == write) {
      err = E_AGAIN;
    } else {
      memcpy(frame, &fifo->buffer[read], sizeof(isotp_frame_t));
      if(++read >= ISOTP_FRAME_FIFO_LEN) {
        read = 0;
      }
      fifo->read = read;
    }

  } while(0);

  return err;
}

INLINE bool isotp_frame_is_empty(isotp_frame_fifo_t *fifo)
{
  bool ret = false;
  uint32_t read, write;

  do {
    BREAK_IF(fifo == NULL);

    read = fifo->read;
    write = fifo->write;

    ret = read == write;

  } while(0);

  return ret;
}

INLINE bool isotp_frame_has_data(isotp_frame_fifo_t *fifo)
{
  bool ret = false;
  uint32_t read, write;

  do {
    BREAK_IF(fifo == NULL);

    read = fifo->read;
    write = fifo->write;

    ret = read != write;

  } while(0);

  return ret;
}

INLINE uint32_t isotp_frame_get_items_count(isotp_frame_fifo_t *fifo)
{
  uint32_t ret = 0;
  uint32_t read, write;

  do {
    BREAK_IF(fifo == NULL);

    read = fifo->read;
    write = fifo->write;

    if(write >= read) {
      ret = write - read;
    } else {
      ret = ISOTP_FRAME_FIFO_LEN - read + write;
    }


  } while(0);

  return ret;
}

INLINE uint32_t isotp_frame_get_free_space(isotp_frame_fifo_t *fifo)
{
  uint32_t ret = 0;
  uint32_t read, write;

  do {
    BREAK_IF(fifo == NULL);

    read = fifo->read;
    write = fifo->write;

    if(write >= read) {
      ret = ISOTP_FRAME_FIFO_LEN - write + read;
    } else {
      ret = read - write;
    }


  } while(0);

  return ret;
}

void isotp_poll_for_reset(isotp_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    if(ctx->reset_trigger) {
      ctx->data_upstream.ready = false;
      ctx->data_downstream.ready = false;
      ctx->frame_fifo_upstream.read = 0;
      ctx->frame_fifo_upstream.write = 0;
      ctx->frame_fifo_downstream.read = 0;
      ctx->frame_fifo_downstream.write = 0;
      ctx->local_error_code = ISOTP_MAX;
      ctx->error_code = ISOTP_OK;
      ctx->state = ISOTP_STATE_IDLE;
      ctx->reset_trigger = false;
    }

  } while(0);
}
