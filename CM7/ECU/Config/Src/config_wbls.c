/*
 * config_wbls.c
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_wbls.h"
#include "config_extern.h"
#include "middlelayer_spi.h"
#include "compiler.h"

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    cj125_init_ctx_t init;
    cj125_config_t config_default;
    cj125_ctx_t *ctx;

    TIM_HandleTypeDef *heater_pwm;
    uint32_t heater_pwm_ch;
    gpio_t heater_en_pin;
    gpio_t heater_nen_pin;
}ecu_devices_wbls_ctx_t;

static const cj125_config_t ecu_devices_wbls_config_default = {

};

static ecu_devices_wbls_ctx_t ecu_devices_wbls_ctx[ECU_DEVICE_WBLS_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_WBLS1,
      .init = {
          .nrst_pin = { .port = LAMBDA_NRST_GPIO_Port, .pin = LAMBDA_NRST_Pin },
      },
      .config_default = ecu_devices_wbls_config_default,
      .heater_pwm = &htim15,
      .heater_pwm_ch = TIM_CHANNEL_1,
      .heater_nen_pin = { .port = LAMBDA_HEATER_NEN_GPIO_Port, .pin = LAMBDA_HEATER_NEN_Pin },
    },
    {
      .slave_index = ECU_SPI_SLAVE_WBLS2,
      .init = {
          .nrst_pin = { .port = LAMBDA_NRST_GPIO_Port, .pin = LAMBDA_NRST_Pin },
      },
      .config_default = ecu_devices_wbls_config_default,
      .heater_pwm = &htim15,
      .heater_pwm_ch = TIM_CHANNEL_2,
      .heater_nen_pin = { .port = LAMBDA_HEATER_NEN_GPIO_Port, .pin = LAMBDA_HEATER_NEN_Pin },
    },
};

error_t ecu_devices_wbls_init(ecu_device_wbls_t instance, cj125_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_wbls_ctx_t *wbls_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_WBLS_MAX || ctx == NULL, err = E_PARAM);

    wbls_ctx = &ecu_devices_wbls_ctx[instance];
    wbls_ctx->ctx = ctx;

    err = middlelayer_spi_get_slave(&wbls_ctx->init.spi_slave, wbls_ctx->slave_index);
    BREAK_IF(err != E_OK);

    err = cj125_init(wbls_ctx->ctx, &wbls_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&wbls_ctx->ctx->config, &wbls_ctx->config_default, sizeof(cj125_config_t));

  } while(0);

  return err;
}
