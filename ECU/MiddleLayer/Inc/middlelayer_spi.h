/*
 * middlelayer_spi.h
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#ifndef MIDDLELAYER_INC_MIDDLELAYER_SPI_H_
#define MIDDLELAYER_INC_MIDDLELAYER_SPI_H_

#include "middlelayer.h"
#include "config_spi.h"
#include "spi.h"

void middlelayer_spi_init(void);
void middlelayer_spi_loop_main(void);
void middlelayer_spi_loop_slow(void);
void middlelayer_spi_loop_fast(void);

error_t middlelayer_spi_get_if(spi_t **spi_if, ecu_spi_if_enum_t interface);
error_t middlelayer_spi_get_slave(spi_slave_t **spi_slave, ecu_spi_slave_enum_t slave);

#endif /* MIDDLELAYER_INC_MIDDLELAYER_SPI_H_ */
