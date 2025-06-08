/*
 * config_tcs.c
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_tcs.h"
#include "middlelayer_spi.h"
#include "compiler.h"

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    spi_slave_t *spi_slave;
    max31855_ctx_t *ctx;
}ecu_devices_tcs_ctx_t;

static ecu_devices_tcs_ctx_t ecu_devices_tcs_ctx[ECU_DEVICE_TCS_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_TCS1,
    },
    {
      .slave_index = ECU_SPI_SLAVE_TCS2,
    },
};

error_t ecu_devices_tcs_init(ecu_device_tcs_t instance, max31855_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_tcs_ctx_t *tcs_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_TCS_MAX || ctx == NULL, err = E_PARAM);

    tcs_ctx = &ecu_devices_tcs_ctx[instance];
    tcs_ctx->ctx = ctx;

    err = middlelayer_spi_get_slave(&tcs_ctx->spi_slave, tcs_ctx->slave_index);
    BREAK_IF(err != E_OK);

    err = max31855_init(tcs_ctx->ctx, tcs_ctx->spi_slave);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_tcs_get_data(ecu_device_tcs_t instance, max31855_data_t *data)
{
  error_t err = E_OK;
  ecu_devices_tcs_ctx_t *tcs_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_TCS_MAX || data == NULL, err = E_PARAM);

    tcs_ctx = &ecu_devices_tcs_ctx[instance];

    err = max31855_get_data(tcs_ctx->ctx, data);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_tcs_get_diag(ecu_device_tcs_t instance, max31855_diag_t *diag)
{
  error_t err = E_OK;
  ecu_devices_tcs_ctx_t *tcs_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_TCS_MAX || diag == NULL, err = E_PARAM);

    tcs_ctx = &ecu_devices_tcs_ctx[instance];

    err = max31855_get_diag(tcs_ctx->ctx, diag);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
