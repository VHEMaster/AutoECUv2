/*
 * cj125_internal.c
 *
 *  Created on: Apr 19, 2024
 *      Author: VHEMaster
 */

#include "cj125_internal.h"
#include "compiler.h"
#include "interpolation.h"

error_t cj125_serial_operation(cj125_ctx_t *ctx, cj125_payload_t request, cj125_payload_t *response)
{
  error_t err = E_OK;

  while(true) {
    if(ctx->spi_busy == false) {
      ctx->tx_payload = request.word;
      ctx->tx_payload = __REV16(ctx->tx_payload);

      ctx->spi_busy = true;
      err = E_AGAIN;
      continue;
    } else {
      err = spi_transmit_and_receive(ctx->init.spi_slave, &ctx->tx_payload, &ctx->rx_payload, 1);
      if(err == E_OK) {
        ctx->spi_busy = false;
        ctx->rx_payload = __REV16(ctx->rx_payload);

        if(response != NULL) {
          response->word = ctx->rx_payload;
        }
      } else if(err == E_BUSY) {
        err = E_AGAIN;
      }
    }
    break;
  }

  return err;
}

error_t cj125_update_data(cj125_ctx_t *ctx)
{
  error_t err = E_OK;
  float ur_volt_dist_vcc, ua_volt_dist_vcc;
  float Vref_div_r17, Vref_div_radj;
  float Vvm, Irm, Vref;
  const float r_17 = 0.05882352f;
  const float Vri = 15.5f;
  sMathInterpolateInput mii_current, mii_temperature;

  if(ctx->configured) {
    if(ctx->ur_updated) {
      ctx->ur_updated = false;

      Vref = ctx->data.ref_voltage;
      Vvm = Vref * 0.5f;

      Vref_div_r17 = Vref * r_17;
      ur_volt_dist_vcc = ctx->data.ur_voltage - Vref_div_r17;
      Irm = Vvm / ctx->config.pushpull_resistance;

      ctx->data.heat_resistance = ur_volt_dist_vcc / (Vri * Irm);
      mii_temperature = math_interpolate_input(ctx->data.heat_resistance, ctx->config.res_to_temp_relation.input, ctx->config.res_to_temp_relation.items);
      ctx->data.temp_value = math_interpolate_1d(mii_temperature, ctx->config.res_to_temp_relation.output);

      ctx->data_temp_valid = true;
    }

    if(ctx->ua_updated) {
      ctx->ua_updated = false;

      Vref_div_radj = Vref * ctx->data.lambda_radj;
      ua_volt_dist_vcc = ctx->data.ua_voltage - Vref_div_radj;

      ctx->data.lambda_current = ua_volt_dist_vcc / (ctx->config.shunt_resistance * (float)ctx->config.ampfactor);
      mii_current = math_interpolate_input(ctx->data.lambda_current, ctx->config.curr_to_lambda_relation.input, ctx->config.curr_to_lambda_relation.items);
      ctx->data.lambda_value = math_interpolate_1d(mii_current, ctx->config.curr_to_lambda_relation.output);

      ctx->data_lambda_valid = true;
    }
  }

  return err;
}
