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
  ECU_SPI_IF0_SLAVE_FLEXIO1 = 0,

  ECU_SPI_IF1_SLAVE_FLEXIO2 = 0,

  ECU_SPI_IF2_SLAVE_OUTS1 = 0,
  ECU_SPI_IF2_SLAVE_OUTS2 = 1,
  ECU_SPI_IF2_SLAVE_OUTSM1 = 2,
  ECU_SPI_IF2_SLAVE_OUTSM2 = 3,

  ECU_SPI_IF3_SLAVE_EGT1 = 0,
  ECU_SPI_IF3_SLAVE_EGT2 = 1,
  ECU_SPI_IF3_SLAVE_KNOCK = 2,
  ECU_SPI_IF3_SLAVE_LAMBDA1 = 3,
  ECU_SPI_IF3_SLAVE_LAMBDA2 = 4,

  ECU_SPI_IFS_SLAVE_MAX
}ecu_spi_if_slave_enum_t;

typedef enum {
  ECU_SPI_IF_FLEXIO1 = 0,
  ECU_SPI_IF_FLEXIO2,
  ECU_SPI_IF_OUTS,
  ECU_SPI_IF_MISC,
  ECU_SPI_IF_MAX
}ecu_spi_if_enum_t;

typedef enum {
  ECU_SPI_SLAVE_FLEXIO1 = 0,
  ECU_SPI_SLAVE_FLEXIO2,
  ECU_SPI_SLAVE_OUTS1,
  ECU_SPI_SLAVE_OUTS2,
  ECU_SPI_SLAVE_OUTSM1,
  ECU_SPI_SLAVE_OUTSM2,
  ECU_SPI_SLAVE_EGT1,
  ECU_SPI_SLAVE_EGT2,
  ECU_SPI_SLAVE_KNOCK,
  ECU_SPI_SLAVE_LAMBDA1,
  ECU_SPI_SLAVE_LAMBDA2,

  ECU_SPI_SLAVE_MAX
}ecu_spi_slave_enum_t;

typedef struct {
  gpio_t nss_pin;
  ecu_spi_slave_enum_t slave_enum;
}ecu_spi_slave_cfg_t;

error_t ecu_config_spi_register_callbacks(spi_cfg_t *spi_cfg, ecu_spi_if_enum_t interface);
error_t ecu_config_spi_get_if_cfg(spi_cfg_t *spi_cfg, ecu_spi_if_enum_t interface);
error_t ecu_config_spi_get_slave_cfg(ecu_spi_slave_cfg_t *config, ecu_spi_if_enum_t interface, ecu_spi_if_slave_enum_t slave);

#endif /* CONFIG_INC_CONFIG_SPI_H_ */
