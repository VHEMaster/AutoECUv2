/*
 * spi_private.c
 *
 *  Created on: Mar 20, 2024
 *      Author: VHEMaster
 */

#include "spi_private.h"
#include "compiler.h"

ITCM_FUNC INLINE void spi_private_slave_reset(spi_slave_t *spi_slave)
{
  spi_t *spi = spi_slave->spi;
  spi->state = SPI_STATE_IDLE;
  spi->slave_own = NULL;
  spi->poll_scheduled = false;
  spi->errorcode = E_OK;
  spi->busy = false;
}

ITCM_FUNC INLINE void spi_private_irq_handler(spi_t *spi, error_t errorcode)
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

ITCM_FUNC void spi_private_call_slave_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  if(spi_slave->cplt_callback) {
    spi_slave->cplt_callback(spi_slave, errorcode);
  }
}

ITCM_FUNC void spi_private_error_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  spi_t *spi = spi_slave->spi;

  gpio_set(&spi_slave->nss_pin);
  spi->errorcode = errorcode;
  spi->state = SPI_STATE_ERROR;

  spi_private_call_slave_cb(spi_slave, errorcode);
}

ITCM_FUNC void spi_private_txrx_full_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  spi_t *spi = spi_slave->spi;

  gpio_set(&spi_slave->nss_pin);
  spi->errorcode = errorcode;
  spi->state = SPI_STATE_DONE;

  spi_private_call_slave_cb(spi_slave, errorcode);
}

ITCM_FUNC void spi_private_tx_then_rx_tx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  spi_t *spi = spi_slave->spi;

  spi->errorcode = errorcode;
  if(errorcode == E_OK) {
    spi->cplt_callback = spi_private_txrx_full_cplt_cb;
    spi_private_receive(spi_slave, spi->rx_buffer, spi->rx_bytes);
  } else {
    spi_private_error_cb(spi_slave, errorcode);
  }
}

ITCM_FUNC void spi_private_tx_and_poll_tx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  spi_t *spi = spi_slave->spi;

  spi->errorcode = errorcode;
  if(errorcode == E_OK) {
    spi->cplt_callback = spi_private_tx_and_poll_rx_cplt_cb;
    spi_private_receive(spi_slave, spi->rx_buffer, spi->rx_bytes);
  } else {
    spi_private_error_cb(spi_slave, errorcode);
  }
}

ITCM_FUNC void spi_private_tx_and_poll_rx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode)
{
  spi_t *spi = spi_slave->spi;
  const uint8_t *mask = spi->rx_mask;
  const uint8_t *value = spi->rx_value;
  const uint8_t *rxbyte = spi->rx_buffer;
  uint32_t poll_begin_time, now;
  bool match = true;

  if(errorcode == E_OK) {

    for(int i = 0; i < spi->rx_bytes; i++) {
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
      poll_begin_time = spi->time_poll_begin;
      now = time_get_current_us();
      if(time_diff(now, poll_begin_time) <= spi->time_poll_timeout) {
        if(spi->poll_period == 0u) {
          spi_private_receive(spi_slave, spi->rx_buffer, spi->rx_bytes);
        } else {
          spi->poll_startpoint = time_get_current_us();
          spi->poll_scheduled = true;
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

ITCM_FUNC INLINE error_t spi_private_transmit_receive(spi_slave_t *spi_slave, const void *tx_data, const void *rx_data, uint16_t size)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  error_t err = E_HAL;
  spi_t *spi = spi_slave->spi;

  spi->state = SPI_STATE_WAIT_TXRX;
  spi->time_transaction = time_get_current_us();

  gpio_reset(&spi_slave->nss_pin);
  if(spi->cfg.use_dma && size >= spi->cfg.dma_usage_threshold) {
    status = HAL_SPI_TransmitReceive_DMA(spi->cfg.hspi, (const uint8_t *)tx_data, (uint8_t *)rx_data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else if(spi->cfg.use_interrupt) {
    status = HAL_SPI_TransmitReceive_IT(spi->cfg.hspi, (const uint8_t *)tx_data, (uint8_t *)rx_data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else {
    status = HAL_SPI_TransmitReceive(spi->cfg.hspi, (const uint8_t *)tx_data, (uint8_t *)rx_data, size, spi->cfg.timeout / TIME_US_IN_MS);
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

ITCM_FUNC INLINE error_t spi_private_transmit(spi_slave_t *spi_slave, const void *data, uint16_t size)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  error_t err = E_HAL;
  spi_t *spi = spi_slave->spi;

  spi->state = SPI_STATE_WAIT_TX;
  spi->time_transaction = time_get_current_us();

  gpio_reset(&spi_slave->nss_pin);
  if(spi->cfg.use_dma && size >= spi->cfg.dma_usage_threshold) {
    status = HAL_SPI_Transmit_DMA(spi->cfg.hspi, (const uint8_t *)data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else if(spi->cfg.use_interrupt) {
    status = HAL_SPI_Transmit_IT(spi->cfg.hspi, (const uint8_t *)data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else {
    status = HAL_SPI_Transmit(spi->cfg.hspi, (const uint8_t *)data, size, spi->cfg.timeout / TIME_US_IN_MS);
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

ITCM_FUNC INLINE error_t spi_private_receive(spi_slave_t *spi_slave, void *data, uint16_t size)
{
  HAL_StatusTypeDef status = HAL_ERROR;
  error_t err = E_HAL;
  spi_t *spi = spi_slave->spi;

  spi->state = SPI_STATE_WAIT_RX;
  spi->time_transaction = time_get_current_us();

  gpio_reset(&spi_slave->nss_pin);
  if(spi->cfg.use_dma && size >= spi->cfg.dma_usage_threshold) {
    status = HAL_SPI_Receive_DMA(spi->cfg.hspi, (uint8_t *)data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else if(spi->cfg.use_interrupt) {
    status = HAL_SPI_Receive_IT(spi->cfg.hspi, (uint8_t *)data, size);
    if(status == HAL_OK) {
      err = E_AGAIN;
    }
  } else {
    status = HAL_SPI_Receive(spi->cfg.hspi, (uint8_t *)data, size, spi->cfg.timeout / TIME_US_IN_MS);
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

ITCM_FUNC void spi_private_poll_loop(spi_t *spi)
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

ITCM_FUNC error_t spi_private_slave_reconfigure(spi_slave_t *spi_slave)
{
  error_t err = E_OK;
  bool config_change = false;
  spi_cfg_t *spi_cfg = &spi_slave->spi->cfg;

  do {
    if(spi_slave->prescaler_configured && spi_cfg->prescaler != spi_slave->prescaler) {
      err |= spi_configure_prescaler(spi_slave->spi, spi_slave->prescaler);
      config_change = true;
    }
    if(spi_slave->datasize_configured && spi_cfg->datasize != spi_slave->datasize) {
      err |= spi_configure_datasize(spi_slave->spi, spi_slave->datasize);
      config_change = true;
    }
    if(spi_slave->mode_configured && spi_cfg->mode != spi_slave->mode) {
      err |= spi_configure_mode(spi_slave->spi, spi_slave->mode);
      config_change = true;
    }
    if(err != E_OK) {
      break;
    }

    if(config_change == true) {
      err |= spi_configure_flush(spi_slave->spi);
      if(err != E_OK) {
        break;
      }
    }
  } while(0);

  return err;
}

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
void spi_private_msp_config_dummy_cb(SPI_HandleTypeDef *hspi)
{

}
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
