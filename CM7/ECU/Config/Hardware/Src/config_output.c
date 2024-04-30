/*
 * config_output.c
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_output.h"
#include "middlelayer_spi.h"
#include "compiler.h"

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    tle6240_init_ctx_t init;
    tle6240_ctx_t *ctx;
}ecu_devices_output_ctx_t;

static ecu_devices_output_ctx_t ecu_devices_output_ctx[ECU_DEVICE_OUTPUT_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_OUTS1,
      .init = {
          .reset_pin = { .port = OUTS_RESET_GPIO_Port, .pin = OUTS_RESET_Pin },
      },
    },
    {
      .slave_index = ECU_SPI_SLAVE_OUTS2,
      .init = {
          .reset_pin = { .port = OUTS_RESET_GPIO_Port, .pin = OUTS_RESET_Pin },
      },
    },
};

error_t ecu_devices_output_init(ecu_device_output_t instance, tle6240_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_output_ctx_t *output_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_OUTPUT_MAX || ctx == NULL, err = E_PARAM);

    output_ctx = &ecu_devices_output_ctx[instance];
    output_ctx->ctx = ctx;

    err = middlelayer_spi_get_slave(&output_ctx->init.spi_slave, output_ctx->slave_index);
    BREAK_IF(err != E_OK);

    err = tle6240_init(output_ctx->ctx, &output_ctx->init);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
