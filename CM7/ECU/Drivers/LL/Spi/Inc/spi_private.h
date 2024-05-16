/*
 * spi_private.h
 *
 *  Created on: Mar 20, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_LL_SPI_INC_SPI_PRIVATE_H_
#define DRIVERS_LL_SPI_INC_SPI_PRIVATE_H_

#include "spi.h"

void spi_private_slave_reset(spi_slave_t *spi_slave);

void spi_private_poll_loop(spi_t *spi);
void spi_private_irq_handler(spi_t *spi, error_t errorcode);
void spi_private_call_slave_cb(spi_slave_t *spi_slave, error_t errorcode);
void spi_private_error_cb(spi_slave_t *spi_slave, error_t errorcode);

void spi_private_txrx_full_cplt_cb(spi_slave_t *spi_slave, error_t errorcode);
void spi_private_tx_then_rx_tx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode);
void spi_private_tx_and_poll_tx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode);
void spi_private_tx_and_poll_rx_cplt_cb(spi_slave_t *spi_slave, error_t errorcode);

error_t spi_private_transmit_receive(spi_slave_t *spi_slave, const void *tx_data, const void *rx_data, uint16_t size);
error_t spi_private_transmit(spi_slave_t *spi_slave, const void *data, uint16_t size);
error_t spi_private_receive(spi_slave_t *spi_slave, void *data, uint16_t size);

error_t spi_private_slave_reconfigure(spi_slave_t *spi_slave);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1U)
void spi_private_msp_config_dummy_cb(SPI_HandleTypeDef *hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

#endif /* DRIVERS_LL_SPI_INC_SPI_PRIVATE_H_ */
