/*
 * config_flexio.c
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_flexio.h"
#include "middlelayer_spi.h"
#include "compiler.h"

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    l9966_init_ctx_t init;
    l9966_config_t config_default;
    l9966_ctx_t *ctx;
}ecu_devices_flexio_ctx_t;

static ecu_devices_flexio_ctx_t ecu_devices_flexio_ctx[ECU_DEVICE_FLEXIO_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_FLEXIO1,
      .init = {
          .nrst_pin = { .port = FLEXIO_NRST_GPIO_Port, .pin = FLEXIO_NRST_Pin },
          .wake_pin = { .port = FLEXIO1_WAKE_GPIO_Port, .pin = FLEXIO1_WAKE_Pin },
          .sync_pin = { .port = FLEXIO1_SYNC_GPIO_Port, .pin = FLEXIO1_SYNC_Pin },
          .int_pin = { .port = FLEXIO1_INT_GPIO_Port, .pin = FLEXIO1_INT_Pin },
          .chip_address = L9966_FRAME_CFG_ADDR_LOW,
      },
      .config_default = {

      },
    },
    {
      .slave_index = ECU_SPI_SLAVE_FLEXIO2,
      .init = {
          //.nrst_pin = { .port = FLEXIO_NRST_GPIO_Port, .pin = FLEXIO_NRST_Pin },
          .wake_pin = { .port = FLEXIO2_WAKE_GPIO_Port, .pin = FLEXIO2_WAKE_Pin },
          .sync_pin = { .port = FLEXIO2_SYNC_GPIO_Port, .pin = FLEXIO2_SYNC_Pin },
          .int_pin = { .port = FLEXIO2_INT_GPIO_Port, .pin = FLEXIO2_INT_Pin },
          .chip_address = L9966_FRAME_CFG_ADDR_HIGH,
      },
      .config_default = {

      },
    },
};

error_t ecu_devices_flexio_init(ecu_device_flexio_t instance, l9966_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_flexio_ctx_t *flexio_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_FLEXIO_MAX || ctx == NULL, err = E_PARAM);

    flexio_ctx = &ecu_devices_flexio_ctx[instance];
    flexio_ctx->ctx = ctx;

    err = middlelayer_spi_get_slave(&flexio_ctx->init.spi_slave, flexio_ctx->slave_index);
    BREAK_IF(err != E_OK);

    err = l9966_init(flexio_ctx->ctx, &flexio_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&flexio_ctx->ctx->config, &flexio_ctx->config_default, sizeof(l9966_config_t));

  } while(0);

  return err;
}
