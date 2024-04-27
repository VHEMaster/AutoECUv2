/*
 * config_stepper.c
 *
 *  Created on: Apr 26, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_stepper.h"
#include "config_gpio.h"
#include "config_loop.h"
#include "inputs.h"
#include "compiler.h"

#define ECU_STEPPER_VOLTAGE_UPDATE_PERIOD    (50 * TIME_US_IN_MS)

typedef struct {
    tle4729_init_ctx_t init;
    tle4729_config_t config_default;
    tle4729_ctx_t *ctx;
    ecu_gpio_input_pin_t input_pwr;

    input_id_t input_id_pwr;

}ecu_devices_stepper_ctx_t;


static ecu_devices_stepper_ctx_t ecu_devices_stepper_ctx[ECU_DEVICE_STEPPER_MAX] = {
    {
      .input_pwr = ECU_IN_PORT2_VIGN,
      .init = {
          .i10 = { .port = STEP_I0_GPIO_Port, .pin = STEP_I0_Pin },
          .i11 = { .port = STEP_I1_GPIO_Port, .pin = STEP_I1_Pin },
          .i20 = { .port = STEP_I0_GPIO_Port, .pin = STEP_I0_Pin },
          .i21 = { .port = STEP_I1_GPIO_Port, .pin = STEP_I1_Pin },
          .ph1 = { .port = STEP_PH1_GPIO_Port, .pin = STEP_PH1_Pin },
          .ph2 = { .port = STEP_PH2_GPIO_Port, .pin = STEP_PH2_Pin },
          .error1 = { .port = STEP_ERR_GPIO_Port, .pin = STEP_ERR_Pin },
          .error2 = { .port = STEP_ERR_GPIO_Port, .pin = STEP_ERR_Pin },
      },
      .config_default = {
          .acceleration_steps = 8,
          .voltage_to_step_time_mult = {
              .items = 7,
              .input = { 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 15.0f, 16.0f },
              .output = { 3.8f, 2.00f, 1.36f, 1.13f, 1.05f, 0.96f, 0.93f },
          },
          .speed_to_step_time_ms = {
              .items = 7,
              .input = { 0.0f, 0.16f, 0.33f, 0.50f, 0.66f, 0.83f, 1.00f },
              .output = { 14.0f, 9.0f, 7.0f, 5.0f, 3.5f, 2.4f, 2.0f },
          },
          .pos_min = 0,
          .pos_max = 127,
      },
    },
};

static void ecu_devices_stepper_update_voltages_cb(ecu_devices_stepper_ctx_t *stepper_ctx)
{
  error_t err = E_OK;
  float power_voltage;
  input_value_t value;

  do {

    err = input_get_value(stepper_ctx->input_id_pwr, &value, NULL);
    BREAK_IF(err != E_OK);
    power_voltage = (float)value * INPUTS_ANALOG_MULTIPLIER_R;

    err = tle4729_set_pwr_voltage(stepper_ctx->ctx, power_voltage);
    BREAK_IF(err != E_OK);

  } while(0);

  if(err != E_OK && err != E_AGAIN) {
    //TODO: set DTC here
  }
}

error_t ecu_devices_stepper_init(ecu_device_stepper_t instance, tle4729_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_stepper_ctx_t *stepper_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_DEVICE_STEPPER_MAX || ctx == NULL, err = E_PARAM);

    stepper_ctx = &ecu_devices_stepper_ctx[instance];
    stepper_ctx->ctx = ctx;

    err = tle4729_init(stepper_ctx->ctx, &stepper_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&stepper_ctx->ctx->config, &stepper_ctx->config_default, sizeof(tle4729_config_t));

    err = ecu_config_gpio_input_get_id(stepper_ctx->input_pwr, &stepper_ctx->input_id_pwr);
    BREAK_IF(err != E_OK);

    err = ecu_config_gpio_input_set_mode(stepper_ctx->input_pwr, ECU_GPIO_INPUT_TYPE_ANALOG);
    BREAK_IF(err != E_OK);

    err = ecu_config_loop_register_slow((ecu_config_loop_cb_t)ecu_devices_stepper_update_voltages_cb, stepper_ctx, ECU_STEPPER_VOLTAGE_UPDATE_PERIOD);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
