/*
 * spi_private.c
 *
 *  Created on: Mar 20, 2024
 *      Author: VHEMaster
 */

#include "spi_private.h"
#include "compiler.h"

INLINE void spi_private_slave_reset(spi_slave_t *spi_slave)
{
  spi_slave->spi->state = SPI_STATE_IDLE;
  spi_slave->spi->slave_own = NULL;
  spi_slave->spi->poll_scheduled = false;
  spi_slave->spi->errorcode = E_OK;
  spi_slave->spi->busy = false;
}

INLINE void spi_private_irq_handler(spi_t *spi, error_t errorcode)
{
  if(spi != NULL) {
    if(errorcode == E_OK) {
      if(spi->cplt_callback) {
        spi->cplt_callback(spi->slave_own, errorcode);
      }
    } else {
      if(spi->err_callback) {
        spi->err_callback(spi->slave_own, errorcode);
      }
    }
  }
}

void spi_private_call_slave_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  if(spi_slave->cplt_callback) {
    spi_slave->cplt_callback(spi_slave, errorcode);
  }
}

void spi_private_error_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  gpio_set(&spi_slave->nss_pin);
  spi_slave->spi->errorcode = errorcode;
  spi_slave->spi->state = SPI_STATE_ERROR;

  spi_private_call_slave_cb(spi_slave, errorcode);
}

void spi_private_txrx_full_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  gpio_set(&spi_slave->nss_pin);
  spi_slave->spi->errorcode = errorcode;
  spi_slave->spi->state = SPI_STATE_DONE;

  spi_private_call_slave_cb(spi_slave, errorcode);
}

void spi_private_tx_then_rx_tx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  spi_slave->spi->errorcode = errorcode;
  if(errorcode == E_OK) {
    spi_private_receive(spi_slave, spi_slave->spi->rx_buffer, spi_slave->spi->rx_bytes);
  } else {
    spi_private_error_cb(spi_slave, errorcode);
  }
}

void spi_private_tx_and_poll_tx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  spi_slave->spi->errorcode = errorcode;
  if(errorcode == E_OK) {
    spi_slave->spi->cplt_callback = spi_private_tx_and_poll_rx_cplt_cb;
    spi_private_receive(spi_slave, spi_slave->spi->rx_buffer, spi_slave->spi->rx_bytes);
  } else {
    spi_private_error_cb(spi_slave, errorcode);
  }
}

void spi_private_tx_and_poll_rx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  const uint8_t *mask = spi_slave->spi->rx_mask;
  const uint8_t *value = spi_slave->spi->rx_value;
  const uint8_t *rxbyte = spi_slave->spi->rx_buffer;
  uint32_t poll_begin_time, now;
  bool match = true;

  if(errorcode == E_OK) {

    for(int i = 0; i < spi_slave->spi->rx_bytes; i++) {
      if((*rxbyte & *mask) != *value) {
        match = false;
        break;
      }
      mask++;
      value++;
      rxbyte++;
    }

    if(match == true) {
      spi_private_txrx_full_cplt_cb(spi_slave, errorcode);
    } else {
      poll_begin_time = spi_slave->spi->time_poll_begin;
      now = time_get_current_us();
      if(time_diff(now, poll_begin_time) <= spi_slave->spi->time_poll_timeout) {
        if(spi_slave->spi->poll_period == 0u) {
          spi_private_receive(spi_slave, spi_slave->spi->rx_buffer, spi_slave->spi->rx_bytes);
        } else {
          spi_slave->spi->poll_startpoint = time_get_current_us();
          spi_slave->spi->poll_scheduled = true;
        }
      } else {
        errorcode = E_TIMEOUT;
        spi_private_error_cb(spi_slave, errorcode);
      }
    }
  } else {
    spi_private_error_cb(spi_slave, errorcode);
  }
}

INLINE error_t spi_private_transmit_receive(spi_slave_t *spi_slave, const void *tx_data, const void *rx_data, uint16_t size)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  error_t err = E_HAL;

  spi_slave->spi->state = SPI_STATE_WAIT_TXRX;
  spi_slave->spi->time_transaction = time_get_current_us();

  if(spi_slave->spi->cfg.use_dma && size >= spi_slave->spi->cfg.dma_usage_threshold) {
    status = HAL_SPI_TransmitReceive_DMA(spi_slave->spi->cfg.hspi, (const uint8_t *)tx_data, (uint8_t *)rx_data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else if(spi_slave->spi->cfg.use_interrupt) {
    status = HAL_SPI_TransmitReceive_IT(spi_slave->spi->cfg.hspi, (const uint8_t *)tx_data, (uint8_t *)rx_data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else {
    status = HAL_SPI_TransmitReceive(spi_slave->spi->cfg.hspi, (const uint8_t *)tx_data, (uint8_t *)rx_data, size, spi_slave->spi->cfg.timeout / TIME_US_IN_MS);
    if(status == HAL_OK) {
      err = E_OK;
    }
  }

  if(status != HAL_OK || err == E_OK) {
    gpio_set(&spi_slave->nss_pin);
    spi_private_error_cb(spi_slave, err);
  }

  return err;
}

INLINE error_t spi_private_transmit(spi_slave_t *spi_slave, const void *data, uint16_t size)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  error_t err = E_HAL;

  spi_slave->spi->state = SPI_STATE_WAIT_TX;
  spi_slave->spi->time_transaction = time_get_current_us();

  if(spi_slave->spi->cfg.use_dma && size >= spi_slave->spi->cfg.dma_usage_threshold) {
    status = HAL_SPI_Transmit_DMA(spi_slave->spi->cfg.hspi, (const uint8_t *)data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else if(spi_slave->spi->cfg.use_interrupt) {
    status = HAL_SPI_Transmit_IT(spi_slave->spi->cfg.hspi, (const uint8_t *)data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else {
    status = HAL_SPI_Transmit(spi_slave->spi->cfg.hspi, (const uint8_t *)data, size, spi_slave->spi->cfg.timeout / TIME_US_IN_MS);
    if(status == HAL_OK) {
      err = E_OK;
    }
  }

  if(status != HAL_OK || err == E_OK) {
    gpio_set(&spi_slave->nss_pin);
    spi_private_error_cb(spi_slave, err);
  }

  return err;
}

INLINE error_t spi_private_receive(spi_slave_t *spi_slave, void *data, uint16_t size)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  error_t err = E_HAL;

  spi_slave->spi->state = SPI_STATE_WAIT_RX;
  spi_slave->spi->time_transaction = time_get_current_us();

  if(spi_slave->spi->cfg.use_dma && size >= spi_slave->spi->cfg.dma_usage_threshold) {
    status = HAL_SPI_Receive_DMA(spi_slave->spi->cfg.hspi, (uint8_t *)data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else if(spi_slave->spi->cfg.use_interrupt) {
    status = HAL_SPI_Receive_IT(spi_slave->spi->cfg.hspi, (uint8_t *)data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else {
    status = HAL_SPI_Receive(spi_slave->spi->cfg.hspi, (uint8_t *)data, size, spi_slave->spi->cfg.timeout / TIME_US_IN_MS);
    if(status == HAL_OK) {
      err = E_OK;
    }
  }

  if(status != HAL_OK || err == E_OK) {
    gpio_set(&spi_slave->nss_pin);
    spi_private_error_cb(spi_slave, err);
  }

  return err;
}

void spi_private_poll_loop(spi_t *spi)
{
  uint32_t poll_timestamp, now;

  if(spi->busy == true && spi->poll_scheduled == true) {
    poll_timestamp = spi->poll_startpoint;
    now = time_get_current_us();
    if(time_diff(now, poll_timestamp) >= spi->poll_period) {
      spi->time_transaction = now;
      spi->poll_scheduled = false;
      spi->poll_startpoint = now;
      spi_private_receive(spi->slave_own, spi->rx_buffer, spi->rx_bytes);
    }
  }
}
