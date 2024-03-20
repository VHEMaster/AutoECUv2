/*
 * spicontainer.h
 *
 *  Created on: Mar 19, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_SPICONTAINER_INC_SPICONTAINER_H_
#define DRIVERS_SPICONTAINER_INC_SPICONTAINER_H_

#include "main.h"

#include "../../../Drivers/Spi/Inc/spi.h"

#define SPICONTAINER_SPIS_MAX   4u

typedef struct {
    spi_t spis[SPICONTAINER_SPIS_MAX];
}spicontainer_t;

error_t spicontainer_init(spicontainer_t *ctx);
error_t spicontainer_register_spi(spicontainer_t *ctx, spi_t *spi);

void spi_loop_main(spicontainer_t *ctx);
void spi_loop_slow(spicontainer_t *ctx);
void spi_loop_fast(spicontainer_t *ctx);

void spicontainer_tx_irq(spicontainer_t *ctx, SPI_HandleTypeDef *hspi);
void spicontainer_rx_irq(spicontainer_t *ctx, SPI_HandleTypeDef *hspi);
void spicontainer_tx_rx_irq(spicontainer_t *ctx, SPI_HandleTypeDef *hspi);
void spicontainer_error_irq(spicontainer_t *ctx, SPI_HandleTypeDef *hspi);

#endif /* DRIVERS_SPICONTAINER_INC_SPICONTAINER_H_ */
