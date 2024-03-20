/*
 * spi.h
 *
 *  Created on: Mar 19, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_SPI_INC_SPI_H_
#define DRIVERS_SPI_INC_SPI_H_

#include "main.h"
#include "errors.h"
#include "gpio.h"
#include "time.h"
#include "bool.h"

#define SPI_TIMEOUT_DEFAULT_US    (10u * TIME_US_IN_MS)
#define SPI_DMA_THR_MIN           (8u)

typedef struct spi_slave_tag spi_slave_t;

typedef void (*spi_op_cplt_cb_t)(spi_slave_t *spi_slave, error_t errorcode);

typedef struct {
  SPI_HandleTypeDef *hspi;
  bool use_interrupt;
  bool use_dma;
  uint8_t dma_usage_threshold;
  time_delta_us_t timeout;
}spi_cfg_t;

typedef enum {
  SPI_STATE_IDLE = 0,
  SPI_STATE_WAIT_TX,
  SPI_STATE_WAIT_RX,
  SPI_STATE_WAIT_TXRX,
  SPI_STATE_TX_CPLT,
  SPI_STATE_RX_CPLT,
  SPI_STATE_TXRX_CPLT,
  SPI_STATE_IRQ_ERROR,
  SPI_STATE_DONE,
  SPI_STATE_ERROR,
}spi_state_t;

typedef struct {
  spi_cfg_t cfg;
  bool busy;
  volatile spi_state_t state;
  spi_op_cplt_cb_t cplt_callback;
  spi_op_cplt_cb_t err_callback;
  spi_slave_t *slave_own;

  const void *rx_mask;
  const void *rx_value;
  void *rx_buffer;
  uint16_t rx_bytes;
  time_delta_us_t poll_period;
  bool poll_scheduled;
  time_us_t poll_startpoint;
  time_us_t time_transaction;
  time_us_t time_poll_begin;
  time_delta_us_t time_poll_timeout;
  error_t errorcode;

}spi_t;

typedef struct spi_slave_tag {
  spi_t *spi;
  gpio_t nss_pin;
  spi_op_cplt_cb_t cplt_callback;
}spi_slave_t;

typedef enum {
  SPI_MODE_0 = 0,
  SPI_MODE_1,
  SPI_MODE_2,
  SPI_MODE_3,
  SPI_MODE_MAX
}spi_mode_t;

error_t spi_init(spi_t *spi, const spi_cfg_t *cfg);
void spi_loop_main(spi_t *spi);
void spi_loop_slow(spi_t *spi);
void spi_loop_fast(spi_t *spi);

error_t spi_slave_init(spi_t *spi, spi_slave_t *slave, const gpio_t *nss_pin);

error_t spi_configure_prescaler(spi_t *spi, uint16_t prescaler);
error_t spi_configure_mode(spi_t *spi, spi_mode_t mode);
error_t spi_configure_datasize(spi_t *spi, uint8_t datasize);
error_t spi_configure_timeout(spi_t *spi, time_delta_us_t timeout);

error_t spi_slave_configure_callback(spi_slave_t *spi_slave, spi_op_cplt_cb_t callback);

error_t spi_transmit(spi_slave_t *spi_slave, const void *data, uint16_t bytes);
error_t spi_transmit_byte(spi_slave_t *spi_slave, uint8_t data);
error_t spi_transmit_halfword(spi_slave_t *spi_slave, uint16_t data);
error_t spi_transmit_word(spi_slave_t *spi_slave, uint32_t data);

error_t spi_receive(spi_slave_t *spi_slave, void *data, uint16_t bytes);
error_t spi_receive_byte(spi_slave_t *spi_slave, uint8_t *data);
error_t spi_receive_halfword(spi_slave_t *spi_slave, uint16_t *data);
error_t spi_receive_word(spi_slave_t *spi_slave, uint32_t *data);

error_t spi_transmit_and_receive(spi_slave_t *spi_slave, const void *transmit, void *receive, uint16_t bytes);
error_t spi_transmit_then_receive(spi_slave_t *spi_slave, const void *transmit, uint8_t tx_bytes, void *receive, uint8_t rx_bytes);

error_t spi_transmit_and_poll(spi_slave_t *spi_slave, const void *transmit, uint16_t tx_bytes, void *receive, const void *rx_mask, const void *rx_value, uint16_t rx_bytes, time_delta_us_t poll_period, time_delta_us_t timeout);
error_t spi_transmit_byte_and_poll_byte(spi_slave_t *spi_slave, uint8_t transmit, uint8_t *receive, uint8_t rx_mask, uint8_t rx_value, time_delta_us_t poll_period, time_delta_us_t timeout);

error_t spi_sync(spi_slave_t *spi_slave);

void spi_tx_irq(spi_t *spi);
void spi_rx_irq(spi_t *spi);
void spi_tx_rx_irq(spi_t *spi);
void spi_error_irq(spi_t *spi);

#endif /* DRIVERS_SPI_INC_SPI_H_ */
