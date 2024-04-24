/*
 * outputs_internal.c
 *
 *  Created on: Mar 24, 2024
 *      Author: VHEMaster
 */

#include "outputs_internal.h"
#include "compiler.h"

ITCM_FUNC void outputs_internal_loop(output_ctx_t *ctx, output_polling_mode_t polling_mode)
{
  output_if_ctx_t *interface;
  output_ch_ctx_t *ch;
  time_delta_us_t deltatime;
  time_us_t now = time_get_current_us();

  if(polling_mode == OUTPUT_POLLING_MODE_FAST) {
    for(int i = 0; i < ctx->ifs_count; i++) {
      interface = &ctx->ifs[i];
      deltatime = time_diff(now, interface->time_fast_last);
      if(interface->func_if_periodic_fast) {
        interface->func_if_periodic_fast(interface->id, deltatime, interface->usrdata);
      }
    }
  } else if(polling_mode == OUTPUT_POLLING_MODE_SLOW) {
    for(int i = 0; i < ctx->ifs_count; i++) {
      interface = &ctx->ifs[i];
      deltatime = time_diff(now, interface->time_slow_last);
      if(interface->func_if_periodic_slow) {
        interface->func_if_periodic_slow(interface->id, deltatime, interface->usrdata);
      }
    }
  } else if(polling_mode == OUTPUT_POLLING_MODE_MAIN) {
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

    (void)outputs_internal_channel_poll(ch, polling_mode);
  }
}

ITCM_FUNC error_t outputs_internal_channel_poll(output_ch_ctx_t *ch, output_polling_mode_t polling_mode)
{
  error_t err = E_OK;
  time_us_t now, valtime;
  time_delta_us_t diff;
  time_delta_us_t cmp_time = 0;
  output_value_t value = ch->value;
  output_value_t value_expected = value;

  if(ch->periodic_polling_mode == polling_mode) {
    if(ch->periodic_mode != OUTPUT_PERIODIC_IDLE) {
      valtime = ch->periodic_time;
      now = time_get_current_us();
      diff = time_diff(now, valtime);

      if(ch->periodic_mode == OUTPUT_PERIODIC_ONCE_ON || ch->periodic_mode == OUTPUT_PERIODIC_ACTIVE_ON) {
        cmp_time = ch->periodic_time_on;
        value_expected = ch->periodic_value_on;
      } else if(ch->periodic_mode == OUTPUT_PERIODIC_ONCE_OFF || ch->periodic_mode == OUTPUT_PERIODIC_ACTIVE_OFF) {
        cmp_time = ch->periodic_time_off;
        value_expected = ch->periodic_value_off;
      }

      if(diff >= cmp_time) {
        now = time_get_current_us();
        ch->periodic_time = now;
        if(ch->periodic_mode == OUTPUT_PERIODIC_ONCE_ON) {
          ch->periodic_mode = OUTPUT_PERIODIC_ONCE_OFF;
          value_expected = ch->periodic_value_off;
        } else if(ch->periodic_mode == OUTPUT_PERIODIC_ACTIVE_ON) {
          ch->periodic_mode = OUTPUT_PERIODIC_ACTIVE_OFF;
          value_expected = ch->periodic_value_off;
        } else if(ch->periodic_mode == OUTPUT_PERIODIC_ONCE_OFF) {
          ch->periodic_mode = OUTPUT_PERIODIC_IDLE;
        } else if(ch->periodic_mode == OUTPUT_PERIODIC_ACTIVE_OFF) {
          ch->periodic_mode = OUTPUT_PERIODIC_ACTIVE_ON;
          value_expected = ch->periodic_value_on;
        }
      }

      if(value != value_expected) {
        value = value_expected;
        outputs_internal_set_value(ch, value);
      }
    }
  }

  return err;
}

ITCM_FUNC error_t outputs_internal_set_value(output_ch_ctx_t *ch, output_value_t value)
{
  error_t err = E_OK;
  bool gpio_invert;

  ch->value = value;

  if(ch->dest == OUTPUT_DEST_GPIO) {
    gpio_invert = ch->gpio_invert;
    gpio_write(&ch->gpio, value ? !gpio_invert : gpio_invert);
  } else if(ch->dest == OUTPUT_DEST_PTR) {
    *ch->val_pointer = value;
  } else if(ch->dest == OUTPUT_DEST_FUNC) {
    if(ch->func_set != NULL) {
      err = ch->func_set(ch->if_id, ch->id, value, ch->usrdata);
    } else if(ch->if_ptr->func_ch_set != NULL) {
      err = ch->if_ptr->func_ch_set(ch->if_id, ch->id, value, ch->usrdata);
    }
  }

  return err;
}

ITCM_FUNC error_t outputs_internal_get_value(output_ch_ctx_t *ch, output_value_t *value)
{
  error_t err = E_OK;
  bool gpio_invert;
  output_value_t val;

  val = ch->value;

  if(ch->dest == OUTPUT_DEST_GPIO) {
    gpio_invert = ch->gpio_invert;
    val = gpio_read(&ch->gpio) ? !gpio_invert : gpio_invert;
  } else if(ch->dest == OUTPUT_DEST_PTR) {
    val = *ch->val_pointer;
  } else if(ch->dest == OUTPUT_DEST_FUNC) {
    if(ch->if_ptr->func_ch_get != NULL) {
      err = ch->if_ptr->func_ch_get(ch->if_id, ch->id, &val, ch->usrdata);
    }
  }

  *value = val;

  return err;
}
