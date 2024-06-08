/*
 * analoginputs.c
 *
 *  Created on: Mar 23, 2024
 *      Author: VHEMaster
 */

#include "analoginputs.h"
#include "compiler.h"

typedef struct {
  float gain;
  float offset;
}analog_input_ch_t;

typedef struct {
  analog_input_ch_t chs[INPUTS_CHS_MAX];
}analog_input_ctx_t;

static analog_input_ctx_t analog_input_ctx = {0};

error_t analog_input_configure(input_id_t channel_id, float gain, float offset)
{
  error_t err = E_OK;

  do {
    if(channel_id < 0 || channel_id >= INPUTS_CHS_MAX) {
      err = E_PARAM;
      break;
    }

    analog_input_ctx.chs[channel_id].gain = gain;
    analog_input_ctx.chs[channel_id].offset = offset;

  } while(0);

  return err;
}

ITCM_FUNC error_t analog_input_get_value(input_id_t channel_id, analog_input_value_t *value)
{
  error_t err = E_OK;
  input_value_t in_val = 0;
  analog_input_ch_t *channel;

  do {
    if(channel_id < 0 || channel_id >= INPUTS_CHS_MAX || value == NULL) {
      err = E_PARAM;
      break;
    }

    err = input_get_value(channel_id, &in_val, NULL);

    channel = &analog_input_ctx.chs[channel_id];

    *value = ((float)in_val) * channel->gain + channel->offset;

  } while(0);

  return err;
}
