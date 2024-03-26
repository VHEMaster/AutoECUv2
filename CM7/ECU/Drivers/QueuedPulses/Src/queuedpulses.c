/*
 * queuedpulses.c
 *
 *  Created on: Mar 25, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "queuedpulses.h"
#include "compiler.h"

static queuedpulse_ctx_t queuedpulse_ctx;

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

  return err;
}

void queuedpulses_loop_main(void)
{

}

void queuedpulses_loop_slow(void)
{

}

void queuedpulses_loop_fast(void)
{

}

error_t queuedpulses_timer_register(TIM_HandleTypeDef *htim, IRQn_Type irq)
{
  error_t err = E_OK;
  queuedpulse_timer_t *timer;

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

void queuedpulses_tim_irq_handler(TIM_HandleTypeDef *htim)
{
  queuedpulse_timer_t *timer = NULL;
  queuedpulse_output_t *out = NULL;
  queuedpulse_entry_t *entry = NULL;
  bool found = true;
  uint32_t prim, psc, prd, now, diff;

  if(htim != NULL) {
    for(int i = 0; i < queuedpulse_ctx.timers_count; i++) {
      timer = &queuedpulse_ctx.timers[i];
      if(timer->htim == htim) {
        found = true;
        break;
      }
    }

    if(found) {
      prim = EnterCritical();
      out = timer->output_assigned;
      if(out != NULL) {
        (void)output_set_value(out->id, out->value_off);
        out->timer = NULL;
        out->queue_entry = NULL;
      }

      entry = timer->entry_assigned;
      queuedpulse_ctx.queue.entries_bitmap &= ~(1 << entry->id);
      entry->timer = NULL;
      entry->output_assigned = NULL;
      entry = entry->next;
      timer->entry_assigned->next = NULL;
      timer->entry_assigned = NULL;

      __HAL_TIM_DISABLE_IT(timer->htim, TIM_IT_UPDATE);
      __HAL_TIM_DISABLE(timer->htim);

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
        if(diff > entry->pulse) {
          prd = 0;
          queuedpulses_tim_irq_handler(htim);
          ExitCritical(prim);
          return;
        } else{
          prd -= diff;
        }

        while(prd > 0x10000) {
          prd >>= 1;
          psc <<= 1;
        }

        timer->htim->Instance->PSC = psc - 1;
        timer->htim->Instance->ARR = prd - 1;
        timer->htim->Instance->EGR |= 1;

        __HAL_TIM_CLEAR_IT(timer->htim, TIM_IT_UPDATE);
        __HAL_TIM_CLEAR_FLAG(timer->htim, TIM_FLAG_UPDATE);

        __HAL_TIM_ENABLE_IT(timer->htim, TIM_IT_UPDATE);
        __HAL_TIM_ENABLE(timer->htim);
      }

      ExitCritical(prim);
    }
  }
}

error_t queuedpulses_enqueue(output_id_t output, time_delta_us_t pulse)
{
  error_t err = E_OK;
  queuedpulse_output_t *out = NULL;
  queuedpulse_entry_t *entry = NULL;
  queuedpulse_timer_t *timer = NULL;
  queuedpulse_timer_t *timer_temp = NULL;
  queuedpulse_entry_t *entry_temp_prev = NULL;
  queuedpulse_entry_t *entry_temp_next = NULL;
  uint32_t index, prim, psc, prd;
  bool bit_busy = true;
  bool timer_immediate_change = true;

  bool tim_free_found = false;
  bool out_already_active = false;
  time_us_t tim_abs_value;
  time_delta_us_t tim_rel_value;
  time_delta_us_t pulse_diff = 0u;

  time_us_t now;

  do {
    if(output < 0 || output >= OUTPUTS_CHS_MAX) {
      err = E_PARAM;
      break;
    }

    if(pulse < 25) {
      err = E_OK;
      break;
    }

    if(queuedpulse_ctx.queue.entries_bitmap == QUEUEDPULSE_ENTRIES_BITMAP_MAX) {
      err = E_OVERFLOW;
      break;
    }

    if(queuedpulse_ctx.timers_count == 0) {
      err = E_NOTRDY;
      break;
    }

    out = &queuedpulse_ctx.outputs[output];

    now = time_get_current_us();
    prim = EnterCritical();
    if(out->timer == NULL)
    {
      for(int i = 0; i < QUEUEDPULSE_QUEUE_ENTRIES && bit_busy == true; i++) {
        index = queuedpulse_ctx.queue.index_next;
        bit_busy = queuedpulse_ctx.queue.entries_bitmap & (1 << index);
        if(bit_busy == false) {
          entry = &queuedpulse_ctx.queue.entries[index];
        }
        if(++index >= QUEUEDPULSE_QUEUE_ENTRIES) {
          index = 0;
        }
        queuedpulse_ctx.queue.index_next = index;
      }
      ExitCritical(prim);

      if(entry == NULL) {
        err = E_FAULT;
        break;
      }

      now = time_get_current_us();
      prim = EnterCritical();

      for(int i = 0; i < queuedpulse_ctx.timers_count && tim_free_found == false; i++) {
        index = queuedpulse_ctx.timer_next;
        timer_temp = &queuedpulse_ctx.timers[index];

        tim_free_found = timer_temp->entry_assigned == NULL;
        if(tim_free_found == true) {
          timer_immediate_change = true;
          timer = timer_temp;
        }
        if(++index >= queuedpulse_ctx.timers_count) {
          index = 0;
        }
        queuedpulse_ctx.timer_next = index;
      }

      if(tim_free_found == false) {
        index = queuedpulse_ctx.timer_next;
        timer = &queuedpulse_ctx.timers[index];

        entry_temp_next = timer->entry_assigned;
        do {
          tim_abs_value = entry_temp_next->time + entry_temp_next->pulse;
          tim_rel_value = time_diff(tim_abs_value, now);

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
        queuedpulse_ctx.timer_next = index;
      }

      queuedpulse_ctx.queue.entries_bitmap |= 1 << entry->id;
      err = output_set_value(out->id, out->value_on);
      entry->pulse = pulse;
      entry->time = time_get_current_us();
      entry->timer = timer;
      entry->output_assigned = out;
      out->timer = timer;
      out->queue_entry = entry;
    } else {
      timer = out->timer;
      entry = out->queue_entry;
      tim_abs_value = entry->time + entry->pulse;
      tim_rel_value = time_diff(tim_abs_value, now);
      pulse_diff = tim_rel_value;
      entry->pulse = pulse + pulse_diff;
      entry->time = now;
      timer_immediate_change = true;
      if(timer->entry_assigned != NULL) {
        entry_temp_next = timer->entry_assigned;
        do {
          if(entry != entry_temp_next) {
            tim_abs_value = entry_temp_next->time + entry_temp_next->pulse;
            tim_rel_value = time_diff(tim_abs_value, now);

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
    }

    if(timer_immediate_change == true) {
      __HAL_TIM_DISABLE_IT(timer->htim, TIM_IT_UPDATE);
      __HAL_TIM_DISABLE(timer->htim);

      timer->output_assigned = out;
      timer->entry_assigned = entry;

      psc = timer->prescaler_default;
      prd = entry->pulse;
      while(prd > 0x10000) {
        prd >>= 1;
        psc <<= 1;
      }

      timer->htim->Instance->PSC = psc - 1;
      timer->htim->Instance->ARR = prd - 1;
      timer->htim->Instance->EGR |= 1;

      __HAL_TIM_CLEAR_IT(timer->htim, TIM_IT_UPDATE);
      __HAL_TIM_CLEAR_FLAG(timer->htim, TIM_FLAG_UPDATE);

      __HAL_TIM_ENABLE_IT(timer->htim, TIM_IT_UPDATE);
      __HAL_TIM_ENABLE(timer->htim);
    }

    ExitCritical(prim);

  } while(0);

  return err;
}

error_t queuedpulses_reset(output_id_t output)
{

}

error_t queuedpulses_reset_all(void)
{

}
