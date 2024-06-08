/*
 * inputs.c
 *
 *  Created on: Mar 23, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "bool.h"
#include "inputs.h"
#include "inputs_internal.h"
#include "compiler.h"

static input_ctx_t input_ctx;

error_t inputs_init(void)
{
  memset(&input_ctx, 0u, sizeof(input_ctx));

  return E_OK;
}

void inputs_loop_main(void)
{
  inputs_internal_loop(&input_ctx, INPUT_POLLING_MODE_MAIN);
}

void inputs_loop_slow(void)
{
  inputs_internal_loop(&input_ctx, INPUT_POLLING_MODE_SLOW);
}

ITCM_FUNC void inputs_loop_fast(void)
{
  inputs_internal_loop(&input_ctx, INPUT_POLLING_MODE_FAST);
}


input_if_id_t input_if_register(const input_if_cfg_t *cfg, void *usrdata)
{
  uint32_t id_count;
  input_if_id_t id_new = input_ctx.ifs_count;
  input_if_ctx_t *interface;
  time_us_t now;

  id_count = id_new + 1;
  if(id_count < INPUTS_IFS_MAX) {
    interface = &input_ctx.ifs[id_new];
    interface->chs_count = 0;
    interface->id = id_new;
    interface->func_if_init = cfg->if_init;
    interface->func_if_configure = cfg->if_configure;
    interface->func_if_periodic_main = cfg->if_periodic_main;
    interface->func_if_periodic_slow = cfg->if_periodic_slow;
    interface->func_if_periodic_fast = cfg->if_periodic_fast;

    interface->func_ch_init = cfg->ch_init;
    interface->func_ch_configure = cfg->ch_configure;
    interface->func_ch_get = cfg->ch_get;
    interface->func_ch_irq_cb = cfg->ch_irq_cb;
    interface->usrdata = usrdata;

    now = time_get_current_us();
    interface->time_main_last = now;
    interface->time_slow_last = now;
    interface->time_fast_last = now;

    input_ctx.ifs_count = id_count;
  } else {
    id_new = E_OVERFLOW;
  }

  return id_new;
}

error_t input_if_configure(input_if_id_t interface_id, const void *configuration)
{
  error_t err = E_OK;
  input_if_ctx_t *interface;

  do {
    if(interface_id < 0 || interface_id >= input_ctx.ifs_count) {
      err = E_PARAM;
      break;
    }

    interface = &input_ctx.ifs[interface_id];
    memset(interface, 0u, sizeof(input_if_ctx_t));

    if(interface->func_if_configure != NULL) {
      interface->func_if_configure(interface_id, configuration, interface->usrdata);
    }

  } while(0);

  return err;
}

input_id_t input_ch_register(input_if_id_t interface_id, input_ch_id_t channel_id, void *usrdata)
{
  uint32_t id_count;
  input_id_t id_new = input_ctx.chs_count;
  input_if_ctx_t *interface;
  input_ch_ctx_t *channel;
  error_t err;

  id_count = id_new + 1;
  if(interface_id < input_ctx.ifs_count && input_ctx.chs_count < INPUTS_CHS_MAX) {
    if(id_count < INPUTS_CHS_MAX) {
      interface = &input_ctx.ifs[interface_id];
      channel = &input_ctx.chs[id_new];

      memset(channel, 0u, sizeof(input_ch_ctx_t));

      channel->id = channel_id;
      channel->if_id = interface_id;
      channel->if_ptr = interface;
      channel->usrdata = usrdata;
      channel->func_get = interface->func_ch_get;

      interface->chs_ptrs[interface->chs_count++] = channel;
      input_ctx.chs_count = id_count;

      channel->polling_mode = INPUT_POLLING_MODE_MAX;
      err = input_ch_polling_mode(id_new, INPUT_POLLING_MODE_DIRECT);
      RETURN_IF(err != E_OK,  err);
    } else {
      id_new = E_OVERFLOW;
    }
  } else {
    id_new = E_PARAM;
  }

  return id_new;
}

error_t input_ch_configure(input_id_t channel_id, const void *configuration)
{
  error_t err = E_OK;
  input_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count) {
      err = E_PARAM;
      break;
    }

    ch = &input_ctx.chs[channel_id];
    if(ch->if_ptr->func_ch_configure != NULL) {
      ch->if_ptr->func_ch_configure(ch->if_id, ch->id, configuration, ch->usrdata);
    }

  } while(0);

  return err;
}

error_t input_ch_register_irq_cb(input_id_t channel_id, input_func_ch_irq_cb_t irq_callback)
{
  error_t err = E_OK;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count) {
      err = E_PARAM;
      break;
    }

    input_ctx.chs[channel_id].func_irq_cb = irq_callback;

  } while(0);

  return err;
}

error_t input_ch_debounce(input_id_t channel_id, time_delta_us_t debounce_time)
{
  error_t err = E_OK;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count) {
      err = E_PARAM;
      break;
    }

    input_ctx.chs[channel_id].debounce_time = debounce_time;

  } while(0);

  return err;
}

error_t input_ch_polling_mode(input_id_t channel_id, input_polling_mode_t polling_mode)
{
  error_t err = E_OK;
  input_ch_poll_ctx_t *poll_old;
  input_ch_poll_ctx_t *poll_new;
  input_polling_mode_t mode_old;
  input_ch_ctx_t *channel;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count || polling_mode >= INPUT_POLLING_MODE_MAX) {
      err = E_PARAM;
      break;
    }

    channel = &input_ctx.chs[channel_id];
    mode_old = channel->polling_mode;

    if(mode_old != polling_mode) {
      poll_old = &input_ctx.poll[mode_old];
      poll_new = &input_ctx.poll[polling_mode];
      channel->polling_mode = polling_mode;

      for(int c = 0; c < poll_old->channels_count; c++) {
        if(poll_old->channels[c] == channel) {
          poll_old->channels_count--;
          poll_old->channels[c] = NULL;

          for(int i = c; i < poll_old->channels_count; i++) {
            poll_old->channels[i] = poll_old->channels[i + 1];
          }
          poll_old->channels[poll_old->channels_count] = NULL;
          break;
        }

      }

      poll_new->channels[poll_new->channels_count] = channel;
      poll_new->channels_count++;

    }

  } while(0);

  return err;
}

error_t input_ch_source_gpio(input_id_t channel_id, const gpio_t *gpio, bool invert)
{
  error_t err = E_OK;
  input_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count || gpio == NULL) {
      err = E_PARAM;
      break;
    }

    if(gpio_valid(gpio) != true) {
      err = E_PARAM;
      break;
    }

    ch = &input_ctx.chs[channel_id];
    ch->source = INPUT_SOURCE_GPIO;
    ch->gpio = *gpio;
    ch->gpio_invert = invert;

  } while(0);

  return err;
}

error_t input_ch_source_func(input_id_t channel_id, input_func_ch_get_t func)
{
  error_t err = E_OK;
  input_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count || func == NULL) {
      err = E_PARAM;
      break;
    }

    ch = &input_ctx.chs[channel_id];
    ch->source = INPUT_SOURCE_FUNC;
    ch->func_get = func;

  } while(0);

  return err;
}

error_t input_ch_source_ptr(input_id_t channel_id, volatile const input_value_t *pointer)
{
  error_t err = E_OK;
  input_ch_ctx_t *ch;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count || pointer == NULL) {
      err = E_PARAM;
      break;
    }

    ch = &input_ctx.chs[channel_id];
    ch->source = INPUT_SOURCE_PTR;
    ch->val_pointer = pointer;

  } while(0);

  return err;
}

error_t input_ch_source_direct(input_id_t channel_id, input_value_t initial_value)
{
  error_t err = E_OK;
  input_ch_ctx_t *ch;
  time_us_t now;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count) {
      err = E_PARAM;
      break;
    }

    now = time_get_current_us();

    ch = &input_ctx.chs[channel_id];
    ch->source = INPUT_SOURCE_DIRECT;
    ch->val_direct = initial_value;
    ch->value = initial_value;
    ch->value_change_time = now;
    ch->value_time = now;

  } while(0);

  return err;
}

ITCM_FUNC error_t input_get_value(input_id_t channel_id, input_value_t *value, time_delta_us_t *time)
{
  error_t err = E_OK;
  input_ch_ctx_t *ch;
  time_us_t now, valtime;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count || value == NULL) {
      err = E_PARAM;
      break;
    }
    ch = &input_ctx.chs[channel_id];
    now = time_get_current_us();

    err = inputs_internal_channel_poll(ch, INPUT_POLLING_MODE_DIRECT);

    if(value != NULL) {
      *value = ch->value;
    }

    if(time != NULL) {
      valtime = ch->value_change_time;
      now = time_get_current_us();
      *time = time_diff(now, valtime);
    }
  } while(0);

  return err;
}

ITCM_FUNC error_t input_set_value(input_id_t channel_id, input_value_t value, bool force_irq)
{
  error_t err = E_OK;
  input_ch_ctx_t *ch;
  bool gen_cb = force_irq;
  time_us_t now;

  do {
    if(channel_id < 0 || channel_id >= input_ctx.chs_count) {
      err = E_PARAM;
      break;
    }

    ch = &input_ctx.chs[channel_id];

    if(ch->source != INPUT_SOURCE_DIRECT) {
      err = E_INVALACT;
      break;
    }

    now = time_get_current_us();
    if(ch->debounce_time) {
      ch->val_direct = value;
    } else {
      if(ch->value != value) {
        gen_cb = true;
        ch->value_change_time = now;
      }
      ch->val_direct = value;
      ch->value = value;
      ch->value_time = now;

      if(gen_cb == true) {
        if(ch->if_ptr->func_ch_irq_cb != NULL) {
          ch->if_ptr->func_ch_irq_cb(ch->if_id, ch->id, value, ch->usrdata);
        }
        if(ch->func_irq_cb != NULL) {
          ch->func_irq_cb(ch->if_id, ch->id, value, ch->usrdata);
        }
      }
    }
  } while(0);

  return err;
}
