/*
 * queuedpulses.c
 *
 *  Created on: Mar 25, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "queuedpulses.h"
#include "queuedpulses_internal.h"
#include "compiler.h"

static queuedpulse_ctx_t queuedpulse_ctx;

static void queuedpulses_tim_irq_handler(TIM_HandleTypeDef *htim);

error_t queuedpulses_init(void)
{
  error_t err = E_OK;

  memset(&queuedpulse_ctx, 0u, sizeof(queuedpulse_ctx));

  for(int i = 0; i < QUEUEDPULSE_QUEUE_ENTRIES; i++) {
    queuedpulse_ctx.queue.entries[i].id = i;
  }

  for(int i = 0; i < OUTPUTS_CHS_MAX; i++) {
    queuedpulse_ctx.outputs[i].id = i;
  }

  for(int i = 0; i < QUEUEDPULSE_TIMERS; i++) {
    queuedpulse_ctx.timers[i].id = i;
  }

  return err;
}

error_t queuedpulses_timer_register(TIM_HandleTypeDef *htim, IRQn_Type irq)
{
  error_t err = E_OK;
  queuedpulse_timer_t *timer;
  HAL_StatusTypeDef status;

  do {
    if(htim == NULL || irq <= 0) {
      err = E_PARAM;
      break;
    }
    if(queuedpulse_ctx.timers_count >= QUEUEDPULSE_QUEUE_ENTRIES ) {
      err = E_OVERFLOW;
      break;
    }
    timer = &queuedpulse_ctx.timers[queuedpulse_ctx.timers_count];
    timer->htim = htim;
    timer->irq = irq;
    timer->prescaler_default = htim->Instance->PSC + 1;
    timer->entry_assigned = NULL;
    timer->output_assigned = NULL;

    timer->htim->usrdata[0] = timer;

    status = HAL_TIM_RegisterCallback(timer->htim, HAL_TIM_PERIOD_ELAPSED_CB_ID, queuedpulses_tim_irq_handler);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    queuedpulse_ctx.timers_count++;

  } while(0);

  return err;
}

error_t queuedpulses_output_configure(output_id_t output, output_value_t value_on, output_value_t value_off)
{
  error_t err = E_OK;
  queuedpulse_output_t *out;

  do {
    if(output < 0 || output >= OUTPUTS_CHS_MAX) {
      err = E_PARAM;
      break;
    }
    out = &queuedpulse_ctx.outputs[output];
    out->value_on = value_on;
    out->value_off = value_off;

  } while(0);

  return err;
}

ITCM_FUNC static void queuedpulses_tim_irq_handler(TIM_HandleTypeDef *htim)
{
  queuedpulse_timer_t *timer = NULL;
  queuedpulse_output_t *out = NULL;
  queuedpulse_entry_t *entry = NULL;
  uint32_t prim, psc, prd, now, diff;
  output_value_t value_temp;
  uint32_t entry_id_to_release;

  if(htim != NULL) {
    timer = htim->usrdata[0];

    if(timer != NULL) {
      prim = EnterCritical();
      out = timer->output_assigned;
      entry = timer->entry_assigned;

      if(out != NULL) {
        if(entry->out_seq_next == NULL) {
          value_temp = out->value_off;
          if(out->value_cur != value_temp) {
            (void)output_set_value(out->id, value_temp);
            out->value_cur = value_temp;
          }
          out->queue_entry = NULL;
          out->timer = NULL;
        } else {
          value_temp = entry->out_seq_next->value_on;
          if(out->value_cur != value_temp) {
            (void)output_set_value(out->id, value_temp);
            out->value_cur = value_temp;
          }
          out->queue_entry = entry->out_seq_next;
        }
      }

      entry_id_to_release = entry->id;
      entry->timer = NULL;
      entry->out_seq_next = NULL;
      entry->output_assigned = NULL;

      entry = entry->next;
      timer->entry_assigned->next = NULL;
      timer->entry_assigned = NULL;
      timer->output_assigned = NULL;

      queuedpulse_ctx.queue.entries_bitmap &= ~(1 << entry_id_to_release);

      queuedpulses_internal_tim_disable(timer);

      if(entry) {
        out = entry->output_assigned;
        out->timer = timer;
        out->queue_entry = entry;
        timer->output_assigned = out;
        timer->entry_assigned = entry;


        psc = timer->prescaler_default;
        prd = entry->pulse;

        now = time_get_current_us();
        diff = time_diff(now, entry->time);
        if(diff + 1 >= prd) {
          prd = 0;
          queuedpulses_tim_irq_handler(htim);
        } else {
          prd -= diff;
          queuedpulses_internal_tim_enable(timer, prd, psc);
        }

      } else {
        queuedpulse_ctx.timers_bitmap &= ~(1 << timer->id);
      }

      ExitCritical(prim);
    }
  }
}

ITCM_FUNC error_t queuedpulses_enqueue(output_id_t output, time_delta_us_t pulse)
{
  error_t err = E_OK;
  queuedpulse_output_t *out = NULL;
  do {
    BREAK_IF_ACTION(output < 0 || output >= OUTPUTS_CHS_MAX, err = E_PARAM);

    out = &queuedpulse_ctx.outputs[output];

    err = queuedpulses_enqueue_ex(output, pulse, out->value_on);

  } while(0);

  return err;
}

ITCM_FUNC error_t queuedpulses_enqueue_ex(output_id_t output, time_delta_us_t pulse, output_value_t value_on)
{
  error_t err = E_OK;
  queuedpulse_output_t *out = NULL;
  queuedpulse_entry_t *entry = NULL;
  queuedpulse_timer_t *timer = NULL;
  queuedpulse_entry_t *entry_temp_seq = NULL;
  queuedpulse_entry_t *entry_temp_prev = NULL;
  queuedpulse_entry_t *entry_temp_next = NULL;
  uint32_t index, prim;
  bool timer_immediate_change = true;

  time_delta_us_t tim_rel_value;
  time_delta_us_t pulse_diff = 0u;

  time_us_t now;

  do {
    BREAK_IF_ACTION(output < 0 || output >= OUTPUTS_CHS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(pulse < 10, err = E_OK);
    BREAK_IF_ACTION(queuedpulse_ctx.timers_count == 0, err = E_NOTRDY);

    out = &queuedpulse_ctx.outputs[output];

    now = time_get_current_us();
    prim = EnterCritical();

    BREAK_IF_ACTION(queuedpulse_ctx.queue.entries_bitmap == QUEUEDPULSE_ENTRIES_BITMAP_MAX, (ExitCritical(prim), err = E_OVERFLOW));

    timer = out->timer;
    entry = out->queue_entry;

    if(timer == NULL || entry == NULL) {
      index = POSITION_VAL(~queuedpulse_ctx.queue.entries_bitmap);
      BREAK_IF_ACTION(index >= QUEUEDPULSE_QUEUE_ENTRIES, (ExitCritical(prim), err = E_FAULT));

      entry = &queuedpulse_ctx.queue.entries[index];

      index = POSITION_VAL(~queuedpulse_ctx.timers_bitmap);
      if(index < queuedpulse_ctx.timers_count) {
        timer = &queuedpulse_ctx.timers[index];
        timer_immediate_change = true;
      }

      if(timer == NULL) {
        index = 0;
        timer = &queuedpulse_ctx.timers[index];

        entry_temp_next = timer->entry_assigned;
        do {
          tim_rel_value = queuedpulses_internal_calculate_pulse_cplt_time(entry_temp_next, now);

          if(pulse >= tim_rel_value) {
            entry_temp_prev = entry_temp_next;
          } else {
            break;
          }

          entry_temp_next = entry_temp_next->next;
        } while(entry_temp_next);

        if(entry_temp_prev == NULL) {
          timer_immediate_change = true;
          timer->entry_assigned = entry;
        } else {
          timer_immediate_change = false;
          entry_temp_prev->next = entry;
        }
        entry->next = entry_temp_next;

        if(++index >= queuedpulse_ctx.timers_count) {
          index = 0;
        }
      }

      queuedpulse_ctx.queue.entries_bitmap |= 1 << entry->id;
      queuedpulse_ctx.timers_bitmap |= 1 << timer->id;
      entry->value_on = value_on;
      if(out->value_cur != value_on) {
        err = output_set_value(out->id, value_on);
        out->value_cur = value_on;
      }
      entry->pulse = pulse;
      entry->time = time_get_current_us();
      entry->timer = timer;
      entry->out_seq_next = NULL;
      entry->output_assigned = out;
      out->timer = timer;
      out->queue_entry = entry;
    } else if(entry->value_on == value_on && entry->out_seq_next == NULL) {
      tim_rel_value = queuedpulses_internal_calculate_pulse_cplt_time(entry, now);

      pulse_diff = tim_rel_value;
      entry->pulse = pulse + pulse_diff;
      entry->time = now;
      timer_immediate_change = true;
      if(timer->entry_assigned != NULL) {
        entry_temp_next = timer->entry_assigned;
        do {
          if(entry != entry_temp_next) {
            tim_rel_value = queuedpulses_internal_calculate_pulse_cplt_time(entry_temp_next, now);

            if(entry->pulse >= tim_rel_value) {
              entry_temp_prev = entry_temp_next;
              pulse_diff = tim_rel_value;
            } else {
              break;
            }
          }

          entry_temp_next = entry_temp_next->next;
        } while(entry_temp_next);

        if(entry_temp_prev != NULL) {
          entry_temp_prev->next = entry;
          entry->next = entry_temp_next;
          entry = entry_temp_prev;
          entry->time = now;
          entry->pulse = pulse_diff;
          out = entry->output_assigned;
        }
      }
    } else {
      index = POSITION_VAL(~queuedpulse_ctx.queue.entries_bitmap);
      BREAK_IF_ACTION(index >= QUEUEDPULSE_QUEUE_ENTRIES, (ExitCritical(prim), err = E_FAULT));

      entry = &queuedpulse_ctx.queue.entries[index];
      queuedpulse_ctx.queue.entries_bitmap |= 1 << entry->id;

      entry_temp_next = timer->entry_assigned;
      do {
        tim_rel_value = queuedpulses_internal_calculate_pulse_cplt_time(entry_temp_next, now);

        if(entry_temp_next->output_assigned == out) {
          entry_temp_seq = entry_temp_next;
          pulse_diff = tim_rel_value;
        }

        if(entry_temp_seq != NULL) {
          if(pulse + pulse_diff >= tim_rel_value) {
            entry_temp_prev = entry_temp_next;
          } else {
            break;
          }
        }

        entry_temp_next = entry_temp_next->next;
      } while(entry_temp_next);

      if(entry_temp_prev == NULL) {
        timer_immediate_change = true;
        timer->entry_assigned = entry;
      } else {
        timer_immediate_change = false;
        entry_temp_prev->next = entry;
      }

      if(entry_temp_seq != NULL) {
        entry_temp_seq->out_seq_next = entry;
      }

      entry->time = time_get_current_us();
      entry->timer = timer;
      entry->output_assigned = out;
      entry->value_on = value_on;
      entry->pulse = pulse + pulse_diff;
      entry->next = entry_temp_next;
    }

    if(timer_immediate_change == true) {
      queuedpulses_internal_tim_disable(timer);

      timer->output_assigned = out;
      timer->entry_assigned = entry;

      queuedpulses_internal_tim_enable(timer, entry->pulse, timer->prescaler_default);
    }

    ExitCritical(prim);

  } while(0);

  return err;
}

ITCM_FUNC error_t queuedpulses_reset_all(void)
{
  error_t err = E_OK;
  error_t err_tmp;
  queuedpulse_output_t *out;
  queuedpulse_timer_t *timer;
  queuedpulse_entry_t *entry;
  uint32_t prim;

  prim = EnterCritical();

  for(int i = 0; i < OUTPUTS_CHS_MAX; i++) {
    out = &queuedpulse_ctx.outputs[i];
    out->queue_entry = NULL;
    out->timer = NULL;

    err_tmp = output_set_value(out->id, out->value_off);
    out->value_cur = out->value_off;
    if(err_tmp != E_OK) {
      err = err_tmp;
    }
  }

  for(int i = 0; i < queuedpulse_ctx.timers_count; i++) {
    timer = &queuedpulse_ctx.timers[i];
    timer->entry_assigned = NULL;
    timer->output_assigned = NULL;
    queuedpulses_internal_tim_disable(timer);
  }

  for(int i = 0; i < QUEUEDPULSE_QUEUE_ENTRIES; i++) {
    entry = &queuedpulse_ctx.queue.entries[i];
    entry->next = NULL;
    entry->output_assigned = NULL;
    entry->out_seq_next = NULL;
    entry->timer = NULL;
    entry->time = 0u;
    entry->pulse = 0u;
    entry->value_on = 0;
  }

  queuedpulse_ctx.queue.entries_bitmap = 0u;
  queuedpulse_ctx.timers_bitmap = 0u;

  ExitCritical(prim);

  return err;
}
