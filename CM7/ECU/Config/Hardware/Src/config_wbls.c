/*
 * config_wbls.c
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_rcc.h"
#include "config_wbls.h"
#include "config_loop.h"
#include "config_gpio.h"
#include "config_extern.h"
#include "middlelayer_spi.h"
#include "compiler.h"

#define ECU_WBLS_VOLTAGE_UPDATE_PERIOD    (5 * TIME_US_IN_MS)

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    cj125_init_ctx_t init;
    cj125_config_t config_default;
    cj125_heater_t heater;
    cj125_ctx_t *ctx;
    ecu_gpio_input_pin_t input_ua;
    ecu_gpio_input_pin_t input_ur;
    ecu_gpio_input_pin_t input_ref;
    ecu_gpio_input_pin_t input_pwr;

    input_id_t input_id_ua;
    input_id_t input_id_ur;
    input_id_t input_id_ref;
    input_id_t input_id_pwr;

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
    .ampfactor = CJ125_AF_17,
    .pump_ref_current = CJ125_CONFIG_PRC_OFF,

    .reg_enscun = true,
    .reg_set_dia_q = false,

    .heater_preheat_voltage = 2.0f,
    .heater_initial_voltage = 5.0f,
    .heater_initial_max_voltage = 11.0f,
    .heater_max_voltage = 13.0f,
    .heater_ramp_rate = 0.3f,
    .heater_nominal_voltage = 7.5f,
    .heater_temperature_timeout = 10 * TIME_US_IN_S,
    .heater_pid_update_period = 5 * TIME_US_IN_MS,
    .heater_pid_koffs = {
        .Kp = 0.1f,
        .Ki = 0.01f,
        .Kd = 0.0001f,
    },
};

static ecu_devices_wbls_ctx_t ecu_devices_wbls_ctx[ECU_DEVICE_WBLS_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_WBLS1,
      .init = {
          .nrst_pin = { .port = LAMBDA_NRST_GPIO_Port, .pin = LAMBDA_NRST_Pin },
      },
      .config_default = ecu_devices_wbls_config_default,
      .heater = {
          .heater_pwm = &htim15,
          .heater_pwm_ch = TIM_CHANNEL_1,
          .heater_nen_pin = { .port = LAMBDA_HEATER_NEN_GPIO_Port, .pin = LAMBDA_HEATER_NEN_Pin },
      },
      .input_ua = ECU_IN_PORT2_PIN5,
      .input_ur = ECU_IN_PORT2_PIN6,
      .input_ref = ECU_IN_PORT2_V5V,
      .input_pwr = ECU_IN_PORT2_VIGN,
    },
    {
      .slave_index = ECU_SPI_SLAVE_WBLS2,
      .init = {
          .nrst_pin = { .port = LAMBDA_NRST_GPIO_Port, .pin = LAMBDA_NRST_Pin },
      },
      .config_default = ecu_devices_wbls_config_default,
      .heater = {
          .heater_pwm = &htim15,
          .heater_pwm_ch = TIM_CHANNEL_2,
          .heater_nen_pin = { .port = LAMBDA_HEATER_NEN_GPIO_Port, .pin = LAMBDA_HEATER_NEN_Pin },
      },
      .input_ua = ECU_IN_PORT2_PIN7,
      .input_ur = ECU_IN_PORT2_PIN8,
      .input_ref = ECU_IN_PORT2_V5V,
      .input_pwr = ECU_IN_PORT2_VIGN,
    },
};

static void ecu_devices_wbls_update_voltages_cb(ecu_devices_wbls_ctx_t *wbls_ctx)
{
  error_t err = E_OK;
  cj125_voltages_t voltages;
  input_value_t value;

  do {
    err = input_get_value(wbls_ctx->input_id_ua, &value, NULL);
    BREAK_IF(err != E_OK);
    voltages.ua = (float)value * INPUTS_ANALOG_MULTIPLIER_R;

    err = input_get_value(wbls_ctx->input_id_ur, &value, NULL);
    BREAK_IF(err != E_OK);
    voltages.ur = (float)value * INPUTS_ANALOG_MULTIPLIER_R;

    err = input_get_value(wbls_ctx->input_id_ref, &value, NULL);
    BREAK_IF(err != E_OK);
    voltages.ref = (float)value * INPUTS_ANALOG_MULTIPLIER_R;

    err = input_get_value(wbls_ctx->input_id_pwr, &value, NULL);
    BREAK_IF(err != E_OK);
    voltages.pwr = (float)value * INPUTS_ANALOG_MULTIPLIER_R;

    err = cj125_update_voltages(wbls_ctx->ctx, &voltages);
    BREAK_IF(err != E_OK);

  } while(0);

  if(err != E_OK && err != E_AGAIN) {
    //TODO: set DTC here
  }
}


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

    err = ecu_config_get_tim_base_frequency(wbls_ctx->heater.heater_pwm, &wbls_ctx->heater.tim_base_freq);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(wbls_ctx->heater.tim_base_freq == 0, err = E_FAULT);

    err = cj125_heater_init(wbls_ctx->ctx, &wbls_ctx->heater);
    BREAK_IF(err != E_OK);

    err = ecu_config_gpio_input_get_id(wbls_ctx->input_ua, &wbls_ctx->input_id_ua);
    BREAK_IF(err != E_OK);
    err = ecu_config_gpio_input_get_id(wbls_ctx->input_ur, &wbls_ctx->input_id_ur);
    BREAK_IF(err != E_OK);
    err = ecu_config_gpio_input_get_id(wbls_ctx->input_ref, &wbls_ctx->input_id_ref);
    BREAK_IF(err != E_OK);
    err = ecu_config_gpio_input_get_id(wbls_ctx->input_pwr, &wbls_ctx->input_id_pwr);
    BREAK_IF(err != E_OK);

    err = ecu_config_gpio_input_set_mode(wbls_ctx->input_ua, ECU_GPIO_INPUT_TYPE_ANALOG);
    BREAK_IF(err != E_OK);
    err = ecu_config_gpio_input_set_mode(wbls_ctx->input_ur, ECU_GPIO_INPUT_TYPE_ANALOG);
    BREAK_IF(err != E_OK);
    err = ecu_config_gpio_input_set_mode(wbls_ctx->input_ref, ECU_GPIO_INPUT_TYPE_ANALOG);
    BREAK_IF(err != E_OK);
    err = ecu_config_gpio_input_set_mode(wbls_ctx->input_pwr, ECU_GPIO_INPUT_TYPE_ANALOG);
    BREAK_IF(err != E_OK);

    err = ecu_config_loop_register_slow((ecu_config_loop_cb_t)ecu_devices_wbls_update_voltages_cb, wbls_ctx, ECU_WBLS_VOLTAGE_UPDATE_PERIOD);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_wbls_get_default_config(ecu_device_wbls_t instance, cj125_config_t *config)
{
  error_t err = E_OK;
  ecu_devices_wbls_ctx_t *wbls_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_WBLS_MAX || config == NULL, err = E_PARAM);

    wbls_ctx = &ecu_devices_wbls_ctx[instance];

    memcpy(config, &wbls_ctx->config_default, sizeof(cj125_config_t));

  } while(0);

  return err;
}

error_t ecu_devices_wbls_configure(ecu_device_wbls_t instance, const cj125_config_t *config)
{
  error_t err = E_OK;
  ecu_devices_wbls_ctx_t *wbls_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_WBLS_MAX || config == NULL, err = E_PARAM);

    wbls_ctx = &ecu_devices_wbls_ctx[instance];

    err = cj125_configure(wbls_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_devices_wbls_reset(ecu_device_wbls_t instance)
{
  error_t err = E_OK;
  ecu_devices_wbls_ctx_t *wbls_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_WBLS_MAX, err = E_PARAM);

    wbls_ctx = &ecu_devices_wbls_ctx[instance];

    err = cj125_reset(wbls_ctx->ctx);

  } while(0);

  return err;
}

error_t ecu_devices_wbls_set_heatup(ecu_device_wbls_t instance, cj125_heatup_type_t type)
{
  error_t err = E_OK;
  ecu_devices_wbls_ctx_t *wbls_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_WBLS_MAX, err = E_PARAM);

    wbls_ctx = &ecu_devices_wbls_ctx[instance];

    err = cj125_set_heatup(wbls_ctx->ctx, type);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_wbls_get_data(ecu_device_wbls_t instance, cj125_data_t *data)
{
  error_t err = E_OK;
  ecu_devices_wbls_ctx_t *wbls_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_WBLS_MAX || data == NULL, err = E_PARAM);

    wbls_ctx = &ecu_devices_wbls_ctx[instance];

    err = cj125_get_data(wbls_ctx->ctx, data);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_wbls_get_diag(ecu_device_wbls_t instance, cj125_diag_t *diag)
{
  error_t err = E_OK;
  ecu_devices_wbls_ctx_t *wbls_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_WBLS_MAX || diag == NULL, err = E_PARAM);

    wbls_ctx = &ecu_devices_wbls_ctx[instance];

    err = cj125_get_diag(wbls_ctx->ctx, diag);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}


