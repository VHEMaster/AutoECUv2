/*
 * outputs.c
 *
 *  Created on: Mar 24, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "bool.h"
#include "outputs.h"
#include "outputs_internal.h"
#include "compiler.h"

static output_ctx_t output_ctx;

void outputs_init(void)
{
  memset(&output_ctx, 0u, sizeof(output_ctx));
}

void outputs_loop_main(void)
{
  outputs_internal_loop(&output_ctx, OUTPUT_POLLING_MODE_MAIN);
}

void outputs_loop_slow(void)
{
  outputs_internal_loop(&output_ctx, OUTPUT_POLLING_MODE_SLOW);
}

void outputs_loop_fast(void)
{
  outputs_internal_loop(&output_ctx, OUTPUT_POLLING_MODE_FAST);
}

output_if_id_t output_if_register(const output_if_cfg_t *cfg, void *usrdata)
{
  uint32_t id_count;
  output_if_id_t id_new = output_ctx.ifs_count;
  output_if_ctx_t *interface;
  time_us_t now;

  id_count = id_new + 1;
  if(id_count < OUTPUTS_IFS_MAX) {
    interface = &output_ctx.ifs[id_new];
    memset(interface, 0u, sizeof(output_if_ctx_t));

    interface->chs_count = 0;
    interface->id = id_new;

    if(cfg != NULL) {
      interface->func_if_init = cfg->if_init;
      interface->func_if_configure = cfg->if_configure;
      interface->func_if_periodic_main = cfg->if_periodic_main;
      interface->func_if_periodic_slow = cfg->if_periodic_slow;
      interface->func_if_periodic_fast = cfg->if_periodic_fast;

      interface->func_ch_init = cfg->ch_init;
      interface->func_ch_configure = cfg->ch_configure;
      interface->func_ch_get = cfg->ch_get;
      interface->func_ch_set = cfg->ch_set;
    }
    interface->usrdata = usrdata;

    now = time_get_current_us();
    interface->time_main_last = now;
    interface->time_slow_last = now;
    interface->time_fast_last = now;

    output_ctx.ifs_count = id_count;

    if(interface->func_if_init != NULL) {
      interface->func_if_init(interface->id, interface->usrdata);
    }
  } else {
    id_new = E_OVERFLOW;
  }

  return id_new;
}

error_t output_if_configure(output_if_id_t interface_id, const void *configuration)
{
  error_t err = E_OK;
  output_if_ctx_t *interface;

  do {
    if(interface_id < 0 || interface_id >= output_ctx.ifs_count) {
      err = E_PARAM;
      break;
    }

    interface = &output_ctx.ifs[interface_id];
    memset(interface, 0u, sizeof(output_if_ctx_t));

    if(interface->func_if_configure != NULL) {
      interface->func_if_configure(interface_id, configuration, interface->usrdata);
    }

  } while(0);

  return err;
}

output_id_t output_ch_register(output_if_id_t interface_id, output_ch_id_t channel_id, void *usrdata)
{
  uint32_t id_count;
  output_id_t id_new = output_ctx.chs_count;
  output_if_ctx_t *interface;
  output_ch_ctx_t *channel;

  id_count = id_new + 1;
  if(interface_id < output_ctx.ifs_count && output_ctx.chs_count < OUTPUTS_CHS_MAX) {
    if(id_count < OUTPUTS_CHS_MAX) {
      interface = &output_ctx.ifs[interface_id];
      channel = &output_ctx.chs[id_new];

      memset(channel, 0u, sizeof(output_ch_ctx_t));

      channel->id = channel_id;
      channel->if_id = interface_id;
      channel->if_ptr = interface;
      channel->usrdata = usrdata;
      channel->func_set = interface->func_ch_set;

      interface->chs_ptrs[interface->chs_count++] = channel;
      output_ctx.chs_count = id_count;

      if(interface->func_ch_init != NULL) {
        interface->func_ch_init(interface->id, channel->id, interface->usrdata);
      }
    } else {
      id_new = E_OVERFLOW;
    }
  } else {
    id_new = E_PARAM;
  }

  return id_new;
}

error_t output_ch_configure(output_id_t channel_id, const void *configuration)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count) {
      err = E_PARAM;
      break;
    }

    ch = &output_ctx.chs[channel_id];
    if(ch->if_ptr->func_ch_configure != NULL) {
      ch->if_ptr->func_ch_configure(ch->if_id, ch->id, configuration, ch->usrdata);
    }

  } while(0);

  return err;
}

error_t output_ch_dest_gpio(output_id_t channel_id, const gpio_t *gpio, bool invert)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count || gpio == NULL) {
      err = E_PARAM;
      break;
    }

    if(gpio_valid(gpio) != true) {
      err = E_PARAM;
      break;
    }

    ch = &output_ctx.chs[channel_id];
    ch->dest = OUTPUT_DEST_GPIO;
    ch->gpio = *gpio;
    ch->gpio_invert = invert;

  } while(0);

  return err;
}

error_t output_ch_dest_func(output_id_t channel_id, output_func_ch_set_t func)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count || func == NULL) {
      err = E_PARAM;
      break;
    }

    ch = &output_ctx.chs[channel_id];
    ch->dest = OUTPUT_DEST_FUNC;
    ch->func_set = func;

  } while(0);

  return err;
}

error_t output_ch_dest_ptr(output_id_t channel_id, volatile output_value_t *pointer)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count || pointer == NULL) {
      err = E_PARAM;
      break;
    }

    ch = &output_ctx.chs[channel_id];
    ch->dest = OUTPUT_DEST_PTR;
    ch->val_pointer = pointer;

  } while(0);

  return err;
}

error_t output_ch_dest_void(output_id_t channel_id)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count) {
      err = E_PARAM;
      break;
    }

    ch = &output_ctx.chs[channel_id];
    ch->dest = OUTPUT_DEST_VOID;

  } while(0);

  return err;
}

error_t output_ch_periodic_polling_mode(output_id_t channel_id, output_polling_mode_t polling_mode)
{
  error_t err = E_OK;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count || polling_mode >= OUTPUT_POLLING_MODE_MAX) {
      err = E_PARAM;
      break;
    }

    output_ctx.chs[channel_id].periodic_polling_mode = polling_mode;

  } while(0);

  return err;
}

ITCM_FUNC error_t output_set_value(output_id_t channel_id, output_value_t value)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count) {
      err = E_PARAM;
      break;
    }
    ch = &output_ctx.chs[channel_id];

    if(ch->periodic_mode == OUTPUT_PERIODIC_IDLE) {
      err = outputs_internal_set_value(ch, value);
    } else {
      err = E_BUSY;
    }

  } while(0);

  return err;
}

ITCM_FUNC error_t output_get_value(output_id_t channel_id, output_value_t *value)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count || value == NULL) {
      err = E_PARAM;
      break;
    }
    ch = &output_ctx.chs[channel_id];

    err = outputs_internal_get_value(ch, value);

  } while(0);

  return err;
}

ITCM_FUNC error_t output_set_periodic(output_id_t channel_id, bool once, output_value_t value_on, output_value_t value_off, time_delta_us_t time_on, time_delta_us_t time_off)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;
  time_us_t now;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count) {
      err = E_PARAM;
      break;
    }
    ch = &output_ctx.chs[channel_id];

    if((once && ch->periodic_mode != OUTPUT_PERIODIC_ONCE_ON && ch->periodic_mode != OUTPUT_PERIODIC_ONCE_OFF) ||
        (!once && ch->periodic_mode != OUTPUT_PERIODIC_ACTIVE_ON && ch->periodic_mode != OUTPUT_PERIODIC_ACTIVE_OFF)) {
      ch->periodic_mode = OUTPUT_PERIODIC_IDLE;
      ch->periodic_value_on = value_on;
      ch->periodic_value_off = value_off;
      ch->periodic_time_on = time_on;
      ch->periodic_time_off = time_off;
      now = time_get_current_us();
      err = outputs_internal_set_value(ch, ch->periodic_value_on);

      ch->periodic_time = now;
      ch->periodic_mode = once ? OUTPUT_PERIODIC_ONCE_ON : OUTPUT_PERIODIC_ACTIVE_ON;
    } else {
      err = E_BUSY;
    }
  } while(0);

  return err;
}

ITCM_FUNC error_t output_reset_periodic(output_id_t channel_id, output_value_t value)
{
  error_t err = E_OK;
  output_ch_ctx_t *ch;
  time_us_t now;

  do {
    if(channel_id < 0 || channel_id >= output_ctx.chs_count) {
      err = E_PARAM;
      break;
    }
    ch = &output_ctx.chs[channel_id];

    now = time_get_current_us();
    ch->periodic_mode = OUTPUT_PERIODIC_IDLE;
    ch->periodic_time = now;
    ch->periodic_value_on = value;
    ch->periodic_value_off = value;
    ch->periodic_time_on = 0u;
    ch->periodic_time_off = 0u;
    err = outputs_internal_set_value(ch, value);

  } while(0);

  return err;
}
