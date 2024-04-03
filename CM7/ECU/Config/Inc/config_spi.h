/*
 * config_spi.h
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_SPI_H_
#define CONFIG_INC_CONFIG_SPI_H_

#include "spi.h"

typedef enum {
  ECU_SPI_IF_FLEXIO1 = 0,
  ECU_SPI_IF_FLEXIO2,
  ECU_SPI_IF_OUTS,
  ECU_SPI_IF_MISC,
  ECU_SPI_IF_MAX
}ecu_spi_if_enum_t;

error_t ecu_config_spi_register_callbacks(spi_cfg_t *spi_cfg, ecu_spi_if_enum_t interface);
error_t ecu_config_spi_get_if_cfg(spi_cfg_t *spi_cfg, ecu_spi_if_enum_t interface);

#endif /* CONFIG_INC_CONFIG_SPI_H_ */
