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
    max31855_config_t config_default;
    max31855_ctx_t *ctx;
}ecu_devices_tcs_ctx_t;

static const max31855_config_t ecu_devices_tcs_config_default = {
    .poll_period = 50 * TIME_US_IN_MS,
};

static const bool ecu_devices_tcs_enabled_default[ECU_DEVICE_TCS_MAX] = {
    true,
    true,
};

static RAM_SECTION ecu_devices_tcs_ctx_t ecu_devices_tcs_ctx[ECU_DEVICE_TCS_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_TCS1,
      .config_default = ecu_devices_tcs_config_default,
    },
    {
      .slave_index = ECU_SPI_SLAVE_TCS2,
      .config_default = ecu_devices_tcs_config_default,
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
    tcs_ctx->config_default.enabled = ecu_devices_tcs_enabled_default[instance];

    err = middlelayer_spi_get_slave(&tcs_ctx->spi_slave, tcs_ctx->slave_index);
    BREAK_IF(err != E_OK);

    err = max31855_init(tcs_ctx->ctx, tcs_ctx->spi_slave);
    BREAK_IF(err != E_OK);

    err = ecu_devices_set_device_enabled(ECU_DEVICE_TYPE_TCS, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_tcs_configure(ecu_device_tcs_t instance, const max31855_config_t *config)
{
  error_t err = E_OK;
  ecu_devices_tcs_ctx_t *tcs_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_TCS_MAX || config == NULL, err = E_PARAM);

    tcs_ctx = &ecu_devices_tcs_ctx[instance];

    err = max31855_configure(tcs_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_devices_set_device_enabled(ECU_DEVICE_TYPE_TCS, instance, tcs_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_tcs_reset(ecu_device_tcs_t instance)
{
  error_t err = E_OK;
  ecu_devices_tcs_ctx_t *tcs_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_TCS_MAX, err = E_PARAM);

    tcs_ctx = &ecu_devices_tcs_ctx[instance];

    err = max31855_reset(tcs_ctx->ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_tcs_get_default_config(ecu_device_tcs_t instance, max31855_config_t *config)
{
  error_t err = E_OK;
  ecu_devices_tcs_ctx_t *tcs_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_TCS_MAX || config == NULL, err = E_PARAM);

    tcs_ctx = &ecu_devices_tcs_ctx[instance];

    memcpy(config, &tcs_ctx->config_default, sizeof(max31855_config_t));

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
