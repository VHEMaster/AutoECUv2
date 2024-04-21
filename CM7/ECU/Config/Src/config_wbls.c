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
    .res_to_temp_relation = {
        .input = { 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f, 800.0f, 900.0f, 1000.0f, 1500.0f },
        .output = { 1176.0f, 1108.0f, 1056.0f, 1021.0f, 991.0f, 845.0f, 780.0f, 740.0f, 711.0f, 689.0f, 672.0f, 656.0f, 643.0f, 633.0f, 0.0f },
        .items = 14,
    },
    .curr_to_lambda_relation = {
        .input = { -2.000f, -1.602f, -1.243f, -0.927f, -0.800, -0.652f, -0.405, -0.183f, -0.106f, -0.040f, 0.000f, 0.015f, 0.097f, 0.193f, 0.250f, 0.329f, 0.671f, 0.938f, 1.150f, 1.385f, 1.700f, 2.000f, 2.150f, 2.250f },
        .output = {  0.650f, 0.700f, 0.750f, 0.800f, 0.822f, 0.850f, 0.900f, 0.950f, 0.970f, 0.990f, 1.003f, 1.010f, 1.050f, 1.100f, 1.132f, 1.179f, 1.429f, 1.701f, 1.990f, 2.434f, 3.413f, 5.391f, 7.506f, 10.119f },
        .items = 24,
    },
    .temp_ref_resistance_override = false,
    .temp_ref_resistance = 300.0f,
    .temp_ref_res_max_deviation = 50.0f,
    .shunt_resistance = 61.9f,
    .pushpull_resistance = 33000.0f,
    .ampfactor = CJ125_AF_8,
    .pump_ref_current = CJ125_CONFIG_PRC_OFF,
    .pid_cb_period = 5 * TIME_US_IN_MS,
    .pid_cb = NULL,
    .pid_cb_usrdata = NULL,
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
