/*
 * spi.c
 *
 *  Created on: Mar 20, 2024
 *      Author: VHEMaster
 */

#include "spi.h"
#include "spi_private.h"

error_t spi_init(spi_t *spi, const spi_cfg_t *cfg)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status = HAL_OK;

  do {
    if(spi == NULL || cfg == NULL || cfg->hspi == NULL) {
      err = E_PARAM;
      break;
    }

    spi->state = SPI_STATE_RESET;
    spi->cfg = *cfg;

    if(spi->cfg.timeout == 0) {
      spi->cfg.timeout = SPI_TIMEOUT_DEFAULT_US;
    }

    if(spi->cfg.use_dma && spi->cfg.dma_usage_threshold < SPI_DMA_THR_MIN) {
      spi->cfg.dma_usage_threshold = SPI_DMA_THR_MIN;
    }

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
    if(spi->cfg.use_dma || spi->cfg.use_interrupt) {
      if(spi->cfg.tx_cplt_cb != NULL) {
        status |= HAL_SPI_RegisterCallback(spi->cfg.hspi, HAL_SPI_TX_COMPLETE_CB_ID, spi->cfg.tx_cplt_cb);
      } else {
        err = E_PARAM;
        break;
      }
      if(spi->cfg.rx_cplt_cb != NULL) {
        status |= HAL_SPI_RegisterCallback(spi->cfg.hspi, HAL_SPI_RX_COMPLETE_CB_ID, spi->cfg.rx_cplt_cb);
      } else {
        err = E_PARAM;
        break;
      }
      if(spi->cfg.txrx_cplt_cb != NULL) {
        status |= HAL_SPI_RegisterCallback(spi->cfg.hspi, HAL_SPI_TX_RX_COMPLETE_CB_ID, spi->cfg.txrx_cplt_cb);
      } else {
        err = E_PARAM;
        break;
      }
      if(spi->cfg.err_cplt_cb != NULL) {
        status |= HAL_SPI_RegisterCallback(spi->cfg.hspi, HAL_SPI_ERROR_CB_ID, spi->cfg.err_cplt_cb);
      } else {
        err = E_PARAM;
        break;
      }

      if(status != HAL_OK) {
        err = E_HAL;
        break;
      }
    }
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

    spi->busy = false;
    spi->state = SPI_STATE_IDLE;

  } while(0);

  return err;
}

void spi_loop_main(spi_t *spi)
{

}

void spi_loop_slow(spi_t *spi)
{
}

void spi_loop_fast(spi_t *spi)
{
  spi_private_poll_loop(spi);
}


error_t spi_slave_init(spi_t *spi, spi_slave_t *slave, const gpio_t *nss_pin)
{
  error_t err = E_OK;

  do {
    if(spi == NULL || slave == NULL || nss_pin == NULL) {
      err = E_PARAM;
      break;
    }

    if(nss_pin->port == NULL || nss_pin->pin == 0) {
      err = E_PARAM;
      break;
    }

    slave->spi = spi;
    slave->nss_pin = *nss_pin;
  } while(0);

  return err;
}


error_t spi_configure_prescaler(spi_t *spi, uint16_t prescaler)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  spi->cfg.hspi->Init.BaudRatePrescaler = prescaler;
  status = HAL_SPI_Init(spi->cfg.hspi);

  if(status != HAL_OK) {
    err = E_HAL;
  }

  return err;
}

error_t spi_configure_mode(spi_t *spi, spi_mode_t mode)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  switch(mode) {
    case SPI_MODE_0:
      spi->cfg.hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
      spi->cfg.hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
      break;
    case SPI_MODE_1:
      spi->cfg.hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
      spi->cfg.hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
      break;
    case SPI_MODE_2:
      spi->cfg.hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
      spi->cfg.hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
      break;
    case SPI_MODE_3:
      spi->cfg.hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
      spi->cfg.hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
      break;
    default:
      err = E_PARAM;
      break;
  }

  if(err == E_OK) {
    status = HAL_SPI_Init(spi->cfg.hspi);

    if(status != HAL_OK) {
      err = E_HAL;
    }
  }

  return err;
}

error_t spi_configure_datasize(spi_t *spi, uint8_t datasize)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;

  spi->cfg.hspi->Init.DataSize = datasize - 1;
  status = HAL_SPI_Init(spi->cfg.hspi);

  if(status != HAL_OK) {
    err = E_HAL;
  }

  return err;
}
error_t spi_configure_timeout(spi_t *spi, time_delta_us_t timeout)
{
  error_t err = E_OK;

  spi->cfg.timeout = timeout;

  return err;
}

error_t spi_slave_configure_callback(spi_slave_t *spi_slave, spi_op_cplt_cb_t callback)
{
  error_t err = E_OK;

  spi_slave->cplt_callback = callback;

  return err;
}

error_t spi_transmit(spi_slave_t *spi_slave, const void *data, uint16_t bytes)
{
  error_t err = E_OK;
  spi_t *spi = spi_slave->spi;

  if(spi->state == SPI_STATE_IDLE && spi->busy == false) {
    gpio_reset(&spi_slave->nss_pin);
    spi->busy = true;
    spi->slave_own = spi_slave;
    spi->cplt_callback = spi_private_txrx_full_cplt_cb;
    spi->err_callback = spi_private_error_cb;

    err = spi_private_transmit(spi_slave, data, bytes);

  } else {
    err = spi_sync(spi_slave);
  }

  return err;
}

error_t spi_transmit_byte(spi_slave_t *spi_slave, uint8_t data)
{
  return spi_transmit(spi_slave, &data, sizeof(data));
}

error_t spi_transmit_halfword(spi_slave_t *spi_slave, uint16_t data)
{
  return spi_transmit(spi_slave, &data, sizeof(data));
}
error_t spi_transmit_word(spi_slave_t *spi_slave, uint32_t data)
{
  return spi_transmit(spi_slave, &data, sizeof(data));
}

error_t spi_receive(spi_slave_t *spi_slave, void *data, uint16_t bytes)
{
  error_t err = E_OK;
  spi_t *spi = spi_slave->spi;

  if(spi->state == SPI_STATE_IDLE && spi->busy == false) {
    gpio_reset(&spi_slave->nss_pin);
    spi->busy = true;
    spi->slave_own = spi_slave;
    spi->cplt_callback = spi_private_txrx_full_cplt_cb;
    spi->err_callback = spi_private_error_cb;

    err = spi_private_receive(spi_slave, data, bytes);

  } else {
    err = spi_sync(spi_slave);
  }

  return err;
}

error_t spi_receive_byte(spi_slave_t *spi_slave, uint8_t *data)
{
  return spi_receive(spi_slave, data, sizeof(data));
}

error_t spi_receive_halfword(spi_slave_t *spi_slave, uint16_t *data)
{
  return spi_receive(spi_slave, data, sizeof(data));
}

error_t spi_receive_word(spi_slave_t *spi_slave, uint32_t *data)
{
  return spi_receive(spi_slave, data, sizeof(data));
}

error_t spi_transmit_and_receive(spi_slave_t *spi_slave, const void *transmit, void *receive, uint16_t bytes)
{
  error_t err = E_OK;
  spi_t *spi = spi_slave->spi;

  if(spi->state == SPI_STATE_IDLE && spi->busy == false) {
    gpio_reset(&spi_slave->nss_pin);
    spi->busy = true;
    spi->slave_own = spi_slave;
    spi->cplt_callback = spi_private_txrx_full_cplt_cb;
    spi->err_callback = spi_private_error_cb;

    err = spi_private_transmit_receive(spi_slave, transmit, receive, bytes);

  } else {
    err = spi_sync(spi_slave);
  }

  return err;
}

error_t spi_transmit_then_receive(spi_slave_t *spi_slave, const void *transmit, uint8_t tx_bytes, void *receive, uint8_t rx_bytes)
{
  error_t err = E_OK;
  spi_t *spi = spi_slave->spi;

  if(spi->state == SPI_STATE_IDLE && spi->busy == false) {
    gpio_reset(&spi_slave->nss_pin);
    spi->busy = true;
    spi->slave_own = spi_slave;
    spi->rx_buffer = receive;
    spi->rx_bytes = rx_bytes;
    spi->cplt_callback = spi_private_tx_then_rx_tx_cplt_cb;
    spi->err_callback = spi_private_error_cb;

    err = spi_private_transmit(spi_slave, transmit, tx_bytes);

  } else {
    err = spi_sync(spi_slave);
  }

  return err;
}

error_t spi_transmit_and_poll(spi_slave_t *spi_slave, const void *transmit, uint16_t tx_bytes, void *receive, const void *rx_mask, const void *rx_value, uint16_t rx_bytes, time_delta_us_t poll_period, time_delta_us_t timeout)
{
  error_t err = E_OK;
  spi_t *spi = spi_slave->spi;

  if(spi->state == SPI_STATE_IDLE && spi->busy == false) {
    gpio_reset(&spi_slave->nss_pin);
    spi->busy = true;
    spi->slave_own = spi_slave;
    spi->rx_mask = rx_mask;
    spi->rx_value = rx_value;
    spi->rx_buffer = receive;
    spi->rx_bytes = rx_bytes;
    spi->poll_period = poll_period;
    spi->cplt_callback = spi_private_tx_and_poll_tx_cplt_cb;
    spi->err_callback = spi_private_error_cb;
    spi->time_poll_begin = time_get_current_us();
    spi->time_poll_timeout = timeout;

    err = spi_private_transmit(spi_slave, transmit, tx_bytes);

  } else {
    err = spi_sync(spi_slave);
  }

  return err;
}

error_t spi_sync(spi_slave_t *spi_slave)
{
  error_t err = E_AGAIN;
  spi_t *spi = spi_slave->spi;
  uint32_t time_transaction, now;

  if(spi->slave_own == spi_slave && spi->busy == true) {
    if(spi->state == SPI_STATE_DONE) {
      spi_private_slave_reset(spi_slave);
      err = E_OK;
    } else if(spi->state == SPI_STATE_ERROR) {
      err = spi->errorcode;
      spi_private_slave_reset(spi_slave);
    } else {
      time_transaction = spi->time_transaction;
      now = time_get_current_us();
      if(spi->poll_scheduled == false) {
        if(time_diff(now, time_transaction) >= spi->cfg.timeout) {
          err = E_TIMEOUT;
          spi_private_error_cb(spi_slave, err);
          spi_private_slave_reset(spi_slave);
        }
      }
    }
  } else {
    err = E_BUSY;
  }

  return err;
}

error_t spi_transmit_byte_and_poll_byte(spi_slave_t *spi_slave, uint8_t transmit, uint8_t *receive, uint8_t rx_mask, uint8_t rx_value, time_delta_us_t poll_period, time_delta_us_t timeout)
{
  return spi_transmit_and_poll(spi_slave, &transmit, sizeof(transmit), &receive, &rx_mask, &rx_value, sizeof(rx_value), poll_period, timeout);
}

void spi_tx_irq(spi_t *spi)
{
  spi->state = SPI_STATE_TX_CPLT;
  spi_private_irq_handler(spi, E_OK);
}

void spi_rx_irq(spi_t *spi)
{
  spi->state = SPI_STATE_RX_CPLT;
  spi_private_irq_handler(spi, E_OK);
}

void spi_tx_rx_irq(spi_t *spi)
{
  spi->state = SPI_STATE_TXRX_CPLT;
  spi_private_irq_handler(spi, E_OK);
}

void spi_error_irq(spi_t *spi)
{
  spi->state = SPI_STATE_IRQ_ERROR;
  spi_private_irq_handler(spi, E_IO);
}

