/*
 * inputs_internal.c
 *
 *  Created on: Mar 23, 2024
 *      Author: VHEMaster
 */

#include "inputs_internal.h"
#include "compiler.h"

ITCM_FUNC void inputs_internal_loop(input_ctx_t *ctx, input_polling_mode_t polling_mode)
{
  input_if_ctx_t *interface;
  input_ch_ctx_t *ch;
  time_delta_us_t deltatime;
  time_us_t now = time_get_current_us();

  if(polling_mode == INPUT_POLLING_MODE_FAST) {
    for(int i = 0; i < ctx->ifs_count; i++) {
      interface = &ctx->ifs[i];
      deltatime = time_diff(now, interface->time_fast_last);
      if(interface->func_if_periodic_fast) {
        interface->func_if_periodic_fast(interface->id, deltatime, interface->usrdata);
      }
    }
  } else if(polling_mode == INPUT_POLLING_MODE_SLOW) {
    for(int i = 0; i < ctx->ifs_count; i++) {
      interface = &ctx->ifs[i];
      deltatime = time_diff(now, interface->time_slow_last);
      if(interface->func_if_periodic_slow) {
        interface->func_if_periodic_slow(interface->id, deltatime, interface->usrdata);
      }
    }
  } else if(polling_mode == INPUT_POLLING_MODE_MAIN) {
    for(int i = 0; i < ctx->ifs_count; i++) {
      interface = &ctx->ifs[i];
      deltatime = time_diff(now, interface->time_main_last);
      if(interface->func_if_periodic_main) {
        interface->func_if_periodic_main(interface->id, deltatime, interface->usrdata);
      }
    }
  }

  for(int c = 0; c < ctx->chs_count; c++) {
    ch = &ctx->chs[c];

    (void)inputs_internal_channel_poll(ch, polling_mode);
  }
}

ITCM_FUNC error_t inputs_internal_channel_poll(input_ch_ctx_t *ch, input_polling_mode_t polling_mode)
{
  input_value_t val, val_ret;
  time_us_t valtime, now;
  bool gpio_invert;
  bool irq_gen = false;
  error_t err = E_OK;

  val_ret = ch->value;
  now = time_get_current_us();

  if(ch->polling_mode == polling_mode) {
    if(ch->source == INPUT_SOURCE_GPIO) {
      gpio_invert = ch->gpio_invert;
      val = gpio_read(&ch->gpio) ? !gpio_invert : gpio_invert;
    } else if(ch->source == INPUT_SOURCE_PTR) {
      val = *ch->val_pointer;
    } else if(ch->source == INPUT_SOURCE_FUNC) {
      if(ch->func_get != NULL) {
        err = ch->func_get(ch->if_id, ch->id, &val, ch->usrdata);
      } else if(ch->if_ptr->func_ch_get != NULL) {
        err = ch->if_ptr->func_ch_get(ch->if_id, ch->id, &val, ch->usrdata);
      } else {
        val = 0;
      }
    } else if(ch->source == INPUT_SOURCE_DIRECT) {
      if(ch->debounce_time) {
        val = ch->val_direct;
      } else {
        val = val_ret;
      }
    }
    if(ch->debounce_time) {
      valtime = ch->value_time;
      if(val != val_ret) {
        if(time_diff(now, valtime) >= ch->debounce_time) {
          ch->value_change_time = now;
          ch->value_time = now;
          val_ret = val;
          irq_gen = true;
        }
      } else {
        ch->value_time = now;
      }
    } else {
      if(val != val_ret) {
          ch->value_change_time = now;
          val_ret = val;
          irq_gen = true;
      }
      ch->value_time = now;
    }
  }


  if(irq_gen == true) {
    ch->value = val_ret;
    if(ch->if_ptr->func_ch_irq_cb != NULL) {
      ch->if_ptr->func_ch_irq_cb(ch->if_id, ch->id, val_ret, ch->usrdata);
    }
    if(ch->func_irq_cb != NULL) {
      ch->func_irq_cb(ch->if_id, ch->id, val_ret, ch->usrdata);
    }
  }

  return err;
}
