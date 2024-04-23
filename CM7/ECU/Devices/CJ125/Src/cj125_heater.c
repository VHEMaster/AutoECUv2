/*
 * cj125_heater.c
 *
 *  Created on: Apr 23, 2024
 *      Author: VHEMaster
 */

#include "cj125_heater.h"
#include "compiler.h"

error_t cj125_update_heater_voltage(cj125_ctx_t *ctx)
{
  error_t err = E_OK;
  float dutycycle = 0.0f;
  float power_voltage = ctx->data.pwr_voltage;
  float voltage = ctx->data.heater_voltage;

  if(power_voltage > CJ125_HEATER_MINIMUM_POWER_VOLTAGE) {
    dutycycle = voltage / power_voltage;
  }

  dutycycle = CLAMP(dutycycle, 0.0f, 1.0f);

  ctx->data.heater_dutycycle = dutycycle;

  if(dutycycle > 0.0f) {
    *ctx->heater.tim_pulse = (float)ctx->heater.tim_period * dutycycle * dutycycle;
  } else {
    *ctx->heater.tim_pulse = 0.0f;
  }

  if(dutycycle > 0) {
    if(gpio_valid(&ctx->heater.heater_en_pin)) {
      gpio_set(&ctx->heater.heater_en_pin);
    }
    if(gpio_valid(&ctx->heater.heater_nen_pin)) {
      gpio_reset(&ctx->heater.heater_nen_pin);
    }
  }

  return err;
}
