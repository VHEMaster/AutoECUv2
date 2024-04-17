/*
 * config_egt.c
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_egt.h"
#include "middlelayer_spi.h"
#include "compiler.h"

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    spi_slave_t *spi_slave;
    max31855_ctx_t *ctx;
}ecu_devices_egt_ctx_t;

static ecu_devices_egt_ctx_t ecu_devices_egt_ctx[ECU_DEVICE_EGT_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_EGT1,
    },
    {
      .slave_index = ECU_SPI_SLAVE_EGT2,
    },
};

error_t ecu_devices_egt_init(ecu_device_egt_t instance, max31855_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_egt_ctx_t *egt_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_EGT_MAX || ctx == NULL, err = E_PARAM);

    egt_ctx = &ecu_devices_egt_ctx[instance];
    egt_ctx->ctx = ctx;

    err = middlelayer_spi_get_slave(&egt_ctx->spi_slave, egt_ctx->slave_index);
    BREAK_IF(err != E_OK);

    err = max31855_init(egt_ctx->ctx, egt_ctx->spi_slave);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
