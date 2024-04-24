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
  cj125_payload_t payload;
  cj125_reg_resp_t resp_payload;

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

        payload.word = ctx->rx_payload;
        resp_payload.data = payload.bytes[0];
        if(resp_payload.bits.check_value != CJ125_REG_RESP_CHECK) {
          err = E_BADRESP;
        }

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

error_t cj125_update_data(cj125_ctx_t *ctx, bool force)
{
  error_t err = E_OK;
  float ur_volt_sub_vcc, ua_volt_sub_vcc;
  float Vref_div_r17, Vref_div_radj;
  float Vvm, Irm, Vref;
  float ampfactor;
  const float r_17 = 0.05882352f;
  const float Vri = 15.5f;
  const float Vrefpump = 0.450f;
  sMathInterpolateInput mii_current, mii_temperature;

  if(ctx->configured || force) {
    if((ctx->voltages_updated) || force) {
      ctx->voltages_updated = false;

      Vref = ctx->data.ref_voltage;
      Vvm = Vref * 0.5f;

      Vref_div_r17 = Vref * r_17;
      ur_volt_sub_vcc = ctx->data.ur_voltage - Vref_div_r17;
      Irm = (Vvm - Vrefpump) / ctx->config.pushpull_resistance;
      Irm *= 2.0f;

      ctx->data.heat_resistance = ur_volt_sub_vcc / (Vri * Irm);
      mii_temperature = math_interpolate_input(ctx->data.heat_resistance, ctx->config.res_to_temp_relation.input, ctx->config.res_to_temp_relation.items);
      ctx->data.temp_value = math_interpolate_1d(mii_temperature, ctx->config.res_to_temp_relation.output);

      ctx->data_temp_valid = true;

      switch(ctx->data.ampfactor) {
        case CJ125_AF_8:
          ampfactor = 8.0f;
          break;
        case CJ125_AF_17:
        default:
          ampfactor = 17.0f;
          break;
      }

      Vref = ctx->data.ref_voltage;
      Vref_div_radj = Vref * ctx->data.lambda_radj;
      ua_volt_sub_vcc = ctx->data.ua_voltage - Vref_div_radj;

      ctx->data.lambda_current = ua_volt_sub_vcc / (ctx->config.shunt_resistance * ampfactor) * 1000.0f;
      mii_current = math_interpolate_input(ctx->data.lambda_current, ctx->config.curr_to_lambda_relation.input, ctx->config.curr_to_lambda_relation.items);
      ctx->data.lambda_value = math_interpolate_1d(mii_current, ctx->config.curr_to_lambda_relation.output);

      ctx->data_lambda_valid = true;
    }

  }

  return err;
}
