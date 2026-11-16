/*
 * rs422_private.c
 *
 *  Created on: 16 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "rs422_private.h"

#define RS422_TX_COPY_DMA_TIMEOUT_MS (10u)

error_t rs422_private_tx_copy(rs422_ctx_t *ctx, void *dst, const void *src, uint32_t size)
{
  error_t err = E_OK;
  DMA_HandleTypeDef *hdma = ctx->init.hdma;
  HAL_StatusTypeDef status;

  do {
    BREAK_IF(size == 0u)

    if(hdma == NULL || size < ctx->init.dma_copy_threshold) {
      memcpy(dst, src, size);
      err = E_OK;
      break;
    }

    status = HAL_DMA_Start(hdma, (uint32_t)src, (uint32_t)dst, size);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_DMA_PollForTransfer(hdma, HAL_DMA_FULL_TRANSFER, RS422_TX_COPY_DMA_TIMEOUT_MS);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);


  } while(0);

  if(err == E_HAL) {
    (void)HAL_DMA_Abort(hdma);
  }

  return err;
}

void rs422_private_tx_process(rs422_ctx_t *ctx)
{
  HAL_StatusTypeDef status;
  uint32_t length;
  uint32_t send;
  uint32_t read;
  uint32_t write;
  uint8_t *fifo;


  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(ctx->configured == false);

    BREAK_IF(ATOMIC_TRY_LOCK(ctx->tx_process_lock) == false);

    while(true) {
      if(ctx->tx_busy == false) {
        read = ATOMIC_LOAD(ctx->tx_read_pointer);
        write = ATOMIC_LOAD(ctx->tx_write_pointer);
        length = GetUsedToRead(ctx->init.buffer_tx_size, read, write);

        if(length > 0u) {
          send = ctx->init.buffer_tx_size - read;
          if(send > length) {
            send = length;
          }

          ctx->tx_dma_length = send;
          ctx->tx_busy = true;
          ATOMIC_STORE(ctx->tx_errcode, E_AGAIN);

          fifo = ctx->init.buffer_tx;

          if(send >= ctx->init.dma_tx_threshold) {
            status = HAL_UART_Transmit_DMA(ctx->init.handle, fifo + read, send);
          } else {
            status = HAL_UART_Transmit_IT(ctx->init.handle, fifo + read, send);
          }
          if(status != HAL_OK) {
            ctx->tx_busy = false;
            ctx->tx_errcode = E_HAL;
            ctx->tx_errors++;
            break;
          }
        }
      } else if(ctx->tx_errcode != E_AGAIN) {
        if(ctx->tx_errcode != E_OK) {
          ctx->tx_errors++;
        }

        send = ctx->tx_dma_length;
        read = ATOMIC_LOAD(ctx->tx_read_pointer);

        read += send;
        if(read >= ctx->init.buffer_tx_size) {
          read -= ctx->init.buffer_tx_size;
        }
        ctx->tx_busy = false;
        ATOMIC_STORE(ctx->tx_read_pointer, read);

        continue;
      }
      break;
    }
    ATOMIC_RELEASE(ctx->tx_process_lock);
  } while(0);
}

error_t rs422_private_tx_write(rs422_ctx_t *ctx, const void *buffer, uint32_t length)
{
  error_t err = E_OK;
  uint32_t first;
  uint32_t free;
  uint32_t read;
  uint32_t write;
  uint8_t *fifo;

  do {
    BREAK_IF_ACTION(length == 0, err = E_OK);
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(buffer == NULL && length > 0, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);
    BREAK_IF_ACTION(length > ctx->init.buffer_tx_size, err = E_OVERFLOW);

    rs422_private_tx_process(ctx);

    read = ATOMIC_LOAD(ctx->tx_read_pointer);
    write = ATOMIC_LOAD(ctx->tx_write_pointer);

    free = GetFreeToWrite(ctx->init.buffer_tx_size, read, write);
    BREAK_IF_ACTION(length > free, err = E_AGAIN);

    fifo = (uint8_t *)ctx->init.buffer_tx;
    first = ctx->init.buffer_tx_size - write;
    if(first > length) {
      first = length;
    }

    err = rs422_private_tx_copy(ctx, fifo + write, buffer, first);
    BREAK_IF(err != E_OK);

    if(length > first) {
      err = rs422_private_tx_copy(ctx, fifo, (const uint8_t *)buffer + first, length - first);
      BREAK_IF(err != E_OK);
    }

    write += length;
    if(write >= ctx->init.buffer_tx_size) {
      write -= ctx->init.buffer_tx_size;
    }
    ATOMIC_STORE(ctx->tx_write_pointer, write);

    rs422_private_tx_process(ctx);

  } while(0);

  return err;
}
