/*
 * config_spi.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_spi.h"
#include "config_extern.h"
#include "middlelayer_spi.h"
#include "compiler.h"
#include "errors.h"
#include "spi.h"

static spi_cfg_t ecu_config_spi[ECU_SPI_IF_MAX] = {
    {
        .datasize = 32,
        .hspi = &hspi2,
        .mode = SPI_MODE_1,
        .prescaler = SPI_BAUDRATEPRESCALER_8,
        .timeout = 10 * TIME_US_IN_MS,
        .use_dma = false,
        .use_interrupt = true,
    }, //ECU_SPI_IF_FLEXIO1
    {
        .datasize = 32,
        .hspi = &hspi3,
        .mode = SPI_MODE_1,
        .prescaler = SPI_BAUDRATEPRESCALER_8,
        .timeout = 10 * TIME_US_IN_MS,
        .use_dma = false,
        .use_interrupt = true,
    }, //ECU_SPI_IF_FLEXIO2
    {
        .datasize = 8,
        .hspi = &hspi5,
        .mode = SPI_MODE_1,
        .prescaler = SPI_BAUDRATEPRESCALER_16,
        .timeout = 10 * TIME_US_IN_MS,
        .use_dma = false,
        .use_interrupt = true,
    }, //ECU_SPI_IF_OUTS
    {
        .datasize = 8,
        .hspi = &hspi6,
        .mode = SPI_MODE_1,
        .prescaler = SPI_BAUDRATEPRESCALER_16,
        .timeout = 10 * TIME_US_IN_MS,
        .use_dma = false,
        .use_interrupt = true,
    }, //ECU_SPI_IF_MISC
};

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)

#define ECU_CONFIG_SPI_CB(interface, type)  \
static ITCM_FUNC void ecu_config_spi_##interface##_##type##_cb(SPI_HandleTypeDef *hspi)   \
{                                                         \
  spi_t *spi = NULL;                                      \
  error_t err;                                            \
  err = middlelayer_spi_get_if(&spi, interface);          \
  if(err == E_OK && spi != NULL) {                        \
    spi_##type##_irq(spi);                                \
  }                                                       \
}

#define ECU_CONFIG_SPI_IF_CB_DEFINE(interface)    \
ECU_CONFIG_SPI_CB(interface, tx);                 \
ECU_CONFIG_SPI_CB(interface, rx);                 \
ECU_CONFIG_SPI_CB(interface, tx_rx);              \
ECU_CONFIG_SPI_CB(interface, error);              \

#define ECU_CONFIG_SPI_IF_CB_ASSIGN(spi_cfg, interface)           \
spi_cfg->tx_cplt_cb   = ecu_config_spi_##interface##_tx_cb;       \
spi_cfg->rx_cplt_cb   = ecu_config_spi_##interface##_rx_cb;       \
spi_cfg->txrx_cplt_cb = ecu_config_spi_##interface##_tx_rx_cb;    \
spi_cfg->err_cplt_cb  = ecu_config_spi_##interface##_error_cb;    \

ECU_CONFIG_SPI_IF_CB_DEFINE(ECU_SPI_IF_FLEXIO1);
ECU_CONFIG_SPI_IF_CB_DEFINE(ECU_SPI_IF_FLEXIO2);
ECU_CONFIG_SPI_IF_CB_DEFINE(ECU_SPI_IF_OUTS);
ECU_CONFIG_SPI_IF_CB_DEFINE(ECU_SPI_IF_MISC);

#else /* USE_HAL_SPI_REGISTER_CALLBACKS */

#define ECU_CONFIG_SPI_CB(hal_op, spi_op)  \
INLINE ITCM_FUNC void HAL_SPI_##hal_op##Callback(SPI_HandleTypeDef *hspi)    \
{                                                                 \
  spi_t *spi = NULL;                                              \
  error_t err;                                                    \
  for(int i = 0; i < ECU_SPI_IF_MAX; i++) {                       \
    err = middlelayer_spi_get_if(&spi, i);                        \
    if(err == E_OK) {                                             \
      if(spi->cfg.hspi == hspi) {                                 \
        spi_##spi_op##_irq(spi);                                  \
        break;                                                    \
      }                                                           \
    }                                                             \
  }                                                               \
}                                                                 \

ECU_CONFIG_SPI_CB(TxCplt, tx);
ECU_CONFIG_SPI_CB(RxCplt, rx);
ECU_CONFIG_SPI_CB(TxRxCplt, tx_rx);
ECU_CONFIG_SPI_CB(Error, error);

#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

error_t ecu_config_spi_register_callbacks(spi_cfg_t *spi_cfg, ecu_spi_if_enum_t interface)
{
  error_t err = E_OK;

  if(spi_cfg != NULL) {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
    switch(interface) {
      case ECU_SPI_IF_FLEXIO1:
        ECU_CONFIG_SPI_IF_CB_ASSIGN(spi_cfg, ECU_SPI_IF_FLEXIO1);
        break;
      case ECU_SPI_IF_FLEXIO2:
        ECU_CONFIG_SPI_IF_CB_ASSIGN(spi_cfg, ECU_SPI_IF_FLEXIO2);
        break;
      case ECU_SPI_IF_OUTS:
        ECU_CONFIG_SPI_IF_CB_ASSIGN(spi_cfg, ECU_SPI_IF_OUTS);
        break;
      case ECU_SPI_IF_MISC:
        ECU_CONFIG_SPI_IF_CB_ASSIGN(spi_cfg, ECU_SPI_IF_MISC);
        break;
      default:
        err = E_PARAM;
        break;
    }
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
  } else {
    err = E_PARAM;
  }

  return err;
}

error_t ecu_config_spi_get_if_cfg(spi_cfg_t *spi_cfg, ecu_spi_if_enum_t interface)
{
  error_t err = E_OK;

  if(spi_cfg != NULL && interface < ECU_SPI_IF_MAX) {
    memcpy(spi_cfg, &ecu_config_spi[interface], sizeof(spi_cfg_t));
  } else {
    err = E_PARAM;
  }

  return err;
}

