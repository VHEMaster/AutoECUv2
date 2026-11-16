/*
 * rs422.c
 *
 *  Created on: 16 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "rs422.h"

error_t rs422_init(rs422_ctx_t *ctx, const rs422_init_ctx_t *init_ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(init_ctx == NULL, err = E_PARAM);

    memset(ctx, 0u, sizeof(rs422_ctx_t));
    memcpy(&ctx->init, init_ctx, sizeof(rs422_init_ctx_t));

    BREAK_IF_ACTION(ctx->init.handle == NULL, err = E_PARAM);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1UL)
    status = HAL_OK;
    if(ctx->init.tx_cplt_cb != NULL) {
      status |= HAL_UART_RegisterCallback(ctx->init.handle, HAL_UART_TX_COMPLETE_CB_ID, ctx->init.tx_cplt_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->init.rx_halfcplt_cb != NULL) {
      status |= HAL_UART_RegisterCallback(ctx->init.handle, HAL_UART_RX_HALFCOMPLETE_CB_ID, ctx->init.rx_halfcplt_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->init.rx_cplt_cb != NULL) {
      status |= HAL_UART_RegisterCallback(ctx->init.handle, HAL_UART_RX_COMPLETE_CB_ID, ctx->init.rx_cplt_cb);
    } else {
      err = E_PARAM;
      break;
    }
    if(ctx->init.error_cb != NULL) {
      status |= HAL_UART_RegisterCallback(ctx->init.handle, HAL_UART_ERROR_CB_ID, ctx->init.error_cb);
    } else {
      err = E_PARAM;
      break;
    }

    if(status != HAL_OK) {
      err = E_HAL;
      break;
    }
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

    ctx->initialized = true;


  } while(0);

  return err;
}

error_t rs422_configure(rs422_ctx_t *ctx, const rs422_config_t *config)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    if(&ctx->config != config) {
      memcpy(&ctx->config, config, sizeof(rs422_config_t));
    }
    ctx->configured = false;

    (void)HAL_UART_Abort(ctx->init.handle);

    if(ctx->config.enabled) {
      ctx->init.handle->Init.BaudRate = ctx->config.baudrate;
      status = HAL_UART_Init(ctx->init.handle);
      BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

      ctx->rx_half_size = ctx->init.buffer_rx_size >> 1;

      ATOMIC_STORE_RELAXED(ctx->rx_write_pointer, 0u);
      ATOMIC_STORE_RELAXED(ctx->rx_read_pointer, 0u);
      ATOMIC_STORE_RELAXED(ctx->rx_errors, 0u);
      ATOMIC_STORE(ctx->tx_errcode, E_OK);
      ctx->tx_busy = false;

      status = HAL_UART_Receive_DMA(ctx->init.handle, ctx->init.buffer_rx, ctx->init.buffer_rx_size);
      BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

      ctx->configured = true;
    }

  } while(0);

  return err;
}

error_t rs422_reset(rs422_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->configured == false, err = E_INVALACT);

    (void)HAL_UART_Abort(ctx->init.handle);

    ATOMIC_STORE_RELAXED(ctx->tx_errcode, E_OK);
    ATOMIC_STORE_RELAXED(ctx->rx_write_pointer, 0u);
    ATOMIC_STORE(ctx->rx_read_pointer, 0u);
    ctx->rx_errors = 0;
    ctx->tx_busy = false;

    status = HAL_UART_Receive_DMA(ctx->init.handle, ctx->init.buffer_rx, ctx->init.buffer_rx_size);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

  } while(0);

  return err;
}

void rs422_loop_main(rs422_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    // TODO: is it anything to do here?

  } while(0);
}

void rs422_loop_slow(rs422_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    // TODO: is it anything to do here?

  } while(0);
}

void rs422_loop_comm(rs422_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    // TODO: is it anything to do here?

  } while(0);
}

void rs422_rx_cplt_irq(rs422_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    // TODO: implement RX overflow handling

  } while(0);
}

void rs422_rx_halfcplt_irq(rs422_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    // TODO: implement RX overflow handling

  } while(0);
}

void rs422_tx_cplt_irq(rs422_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    ATOMIC_STORE(ctx->tx_errcode, E_OK);

  } while(0);
}

void rs422_error_irq(rs422_ctx_t *ctx)
{
  do {
    BREAK_IF(ctx == NULL);

    ctx->rx_errors++;
    ATOMIC_STORE(ctx->tx_errcode, E_IO);
    ATOMIC_STORE(ctx->rx_write_pointer, 0u);
    ATOMIC_STORE(ctx->rx_read_pointer, 0u);
    (void)HAL_UART_Receive_DMA(ctx->init.handle, ctx->init.buffer_rx, ctx->init.buffer_rx_size);

  } while(0);
}

error_t rs422_tx(rs422_ctx_t *ctx, const void *buffer, uint32_t length)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(length == 0u, err = E_OK);
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);

    if(ctx->tx_busy == false) {
      ctx->tx_errcode = E_AGAIN;
      ctx->tx_buffer = buffer;
      ctx->tx_length = length;
      if(length < ctx->init.dma_tx_threshold) {
        status = HAL_UART_Transmit_IT(ctx->init.handle, buffer, length);
        BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
      } else {
        status = HAL_UART_Transmit_DMA(ctx->init.handle, buffer, length);
        BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
      }
      ctx->tx_busy = true;
      err = E_AGAIN;
    } else {
      BREAK_IF_ACTION(ctx->tx_buffer != buffer || ctx->tx_length != length, err = E_AGAIN);

      err = ctx->tx_errcode;
      if(err != E_AGAIN) {
        ctx->tx_busy = false;
      }
    }

  } while(0);

  return err;
}

error_t rs422_rx(rs422_ctx_t *ctx, void *buffer, uint32_t *p_length)
{
  error_t err = E_OK;
  uint32_t buffer_size;
  uint32_t write_checkup;
  uint32_t write_pointer;
  uint32_t write_original;
  uint32_t read_pointer;
  uint32_t read_checkup;
  uint32_t read_original;
  uint32_t capacity;
  uint32_t read;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(buffer == NULL, err = E_PARAM);
    BREAK_IF_ACTION(p_length == NULL, err = E_PARAM);

    capacity = *p_length;

    buffer_size = ctx->init.buffer_rx_size;
    write_pointer = ATOMIC_LOAD(ctx->rx_write_pointer);
    read_pointer = ATOMIC_LOAD(ctx->rx_read_pointer);
    write_original = write_pointer;
    read_original = read_pointer;

    write_pointer = buffer_size - __HAL_DMA_GET_COUNTER(ctx->init.handle->hdmarx);

    read = GetUsedToRead(ctx->init.buffer_rx_size, read_pointer, write_pointer);
    if(read > capacity) {
      read = capacity;
    }

    for(uint32_t i = 0; i < read; i++) {
      ((uint8_t *)buffer)[i] = ((uint8_t *)ctx->init.buffer_rx)[read_pointer];
      if(++read_pointer >= buffer_size) {
        read_pointer = 0u;
      }
    }

    write_checkup = ATOMIC_EXCHANGE_RELEASE(ctx->rx_write_pointer, write_pointer);
    read_checkup = ATOMIC_EXCHANGE_RELEASE(ctx->rx_read_pointer, read_pointer);
    if(write_original != write_checkup || read_original != read_checkup) {
      ATOMIC_STORE_RELAXED(ctx->rx_write_pointer, 0u);
      ATOMIC_STORE(ctx->rx_read_pointer, 0u);
      err = E_IO;
      read = 0;
    }

    *p_length = read;

  } while(0);

  return err;
}
