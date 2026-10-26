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
static error_t queuedpulses_tim_handler(queuedpulse_timer_t *timer, bool allowed_to_loop);

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
    if(queuedpulse_ctx.timers_count >= QUEUEDPULSE_TIMERS) {
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

OPTIMIZE_FAST
ITCM_FUNC static void queuedpulses_tim_irq_handler(TIM_HandleTypeDef *htim)
{
  queuedpulse_timer_t *timer = NULL;
  uint32_t prim;
  error_t err, i;

  if(htim != NULL) {
    timer = htim->usrdata[0];

    if(timer != NULL) {
      i = 0;
      prim = EnterCritical();

      while(true) {
        err = queuedpulses_tim_handler(timer, true);
#if QUEUEDPULSE_MAX_EVENTS_PER_IRQ > 1u
        BREAK_IF(err != E_AGAIN);
        if(++i >= QUEUEDPULSE_MAX_EVENTS_PER_IRQ - 1u) {
          err = queuedpulses_tim_handler(timer, false);
          break;
        }
#else
        break;
#endif
      }

      ExitCritical(prim);
    }
  }
}

OPTIMIZE_FAST
ITCM_FUNC static error_t queuedpulses_tim_handler(queuedpulse_timer_t *timer, bool allowed_to_loop)
{
  error_t err = E_OK;
  queuedpulse_output_t *out = NULL;
  queuedpulse_entry_t *entry = NULL;
  uint32_t now, remaining;
  output_value_t value_temp;
  uint32_t entry_id_to_release;
  const time_us_t time_mask_2 = time_mask_us() >> 1;

  out = timer->output_assigned;
  entry = timer->entry_assigned;

  now = time_now_us();
  remaining = time_diff(entry->pulse_deadline, now);
  if(remaining != 0u && remaining <= time_mask_2) {
    queuedpulses_internal_tim_disable(timer);
    queuedpulses_internal_tim_enable(timer, remaining, timer->prescaler_default);
    return E_OK;
  }

  if(out != NULL) {
    if(entry->out_seq_next == NULL) {
      value_temp = out->value_off;
      if(out->value_cur != value_temp) {
        (void)output_set_value(out->id, value_temp);
        out->value_cur = value_temp;
      }
      out->queue_entry = NULL;
      out->queue_tail = NULL;
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

  queuedpulse_ctx.queue.entries_bitmap &= ~(1u << entry_id_to_release);

  queuedpulses_internal_tim_disable(timer);

  if(entry) {
    out = entry->output_assigned;
    out->timer = timer;
    out->queue_entry = entry;
    timer->output_assigned = out;
    timer->entry_assigned = entry;

    // TODO: do the diagnostics of late pulses with defined deadline

    now = time_now_us();
    remaining = time_diff(entry->pulse_deadline, now);
    if(remaining > time_mask_2) {
      remaining = 0u;
    }

    if(remaining == 0u) {
      if(allowed_to_loop) {
        err = E_AGAIN;
      } else {
        queuedpulses_internal_tim_enable(timer, 0u, timer->prescaler_default);
      }
    } else {
      queuedpulses_internal_tim_enable(timer, remaining, timer->prescaler_default);
    }

  } else {
    queuedpulse_ctx.timers_bitmap &= ~(1u << timer->id);
  }

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC error_t queuedpulses_enqueue(output_id_t output, time_us_t deadline)
{
  error_t err = E_OK;
  queuedpulse_output_t *out = NULL;
  do {
    BREAK_IF_ACTION(output < 0 || output >= OUTPUTS_CHS_MAX, err = E_PARAM);

    out = &queuedpulse_ctx.outputs[output];

    err = queuedpulses_enqueue_ex(output, deadline, out->value_on);

  } while(0);

  return err;
}

OPTIMIZE_FAST
STATIC_INLINE time_delta_us_t queuedpulses_remaining(time_us_t deadline, time_us_t now)
{
  time_delta_us_t delta = time_diff(deadline, now);
  return delta <= (time_mask_us() >> 1u) ? delta : 0u;
}

OPTIMIZE_FAST
ITCM_FUNC static void queuedpulses_insert_pulse(queuedpulse_timer_t *timer,
    queuedpulse_entry_t *tail, queuedpulse_entry_t *first, queuedpulse_entry_t *last, time_us_t now)
{
  queuedpulse_entry_t **link = tail != NULL ? &tail->next : &timer->entry_assigned;
  time_delta_us_t delta = queuedpulses_remaining(first->pulse_deadline, now);

  while(*link != NULL && queuedpulses_remaining((*link)->pulse_deadline, now) <= delta) {
    link = &(*link)->next;
  }

  first->next = *link;
  *link = first;

  if(first != last) {
    delta = queuedpulses_remaining(last->pulse_deadline, now);
    link = &first->next;

    while(*link != NULL && queuedpulses_remaining((*link)->pulse_deadline, now) <= delta) {
      link = &(*link)->next;
    }

    last->next = *link;
    *link = last;
  }
}

OPTIMIZE_FAST
ITCM_FUNC static error_t queuedpulses_schedule_gap_locked(queuedpulse_output_t *out,
    time_delta_us_t lead, time_delta_us_t end_delta, output_value_t value_on,
    bool keep_width, time_us_t now, time_delta_us_t width)
{
  queuedpulse_timer_t *timer = out->timer;
  queuedpulse_entry_t *previous = NULL;
  queuedpulse_entry_t *gap = out->queue_entry;
  queuedpulse_entry_t *first = NULL, *last = NULL;
  queuedpulse_entry_t *old_head = timer->entry_assigned;
  time_delta_us_t begin = 0u, finish, start_delta, limit;
  const time_delta_us_t max_delay = time_mask_us() >> 1u;
  uint32_t free, used = 0u, index;
  bool waiting, reuse_end;
  error_t err = E_OK;

  do {
    while(true) {
      finish = gap != NULL ? queuedpulses_remaining(gap->pulse_deadline, now) : max_delay;
      if(gap == NULL || gap->value_on == out->value_off) {
        start_delta = begin;
        if(!keep_width && previous != NULL && previous->value_on != out->value_off) {
          start_delta += QUEUEDPULSE_MINIMUM_PULSE_US;
        }
        if(start_delta < lead) {
          start_delta = lead;
        }
        limit = finish;
        if(!keep_width && gap != NULL && gap->out_seq_next != NULL &&
            gap->out_seq_next->value_on != out->value_off) {
          limit = finish >= QUEUEDPULSE_MINIMUM_PULSE_US ?
              finish - QUEUEDPULSE_MINIMUM_PULSE_US : 0u;
        }
        if(keep_width) {
          end_delta = start_delta + width;
        }
        BREAK_IF(end_delta <= limit && end_delta > start_delta &&
            end_delta - start_delta >= QUEUEDPULSE_MINIMUM_PULSE_US);
        if(gap == NULL || (!keep_width && end_delta <= finish)) {
          return keep_width ? E_PARAM : E_OK;
        }
      }
      previous = gap;
      begin = finish;
      gap = gap->out_seq_next;
    }
    BREAK_IF(err != E_OK);

    waiting = start_delta != 0u && !(previous != NULL &&
        start_delta == queuedpulses_remaining(previous->pulse_deadline, now) &&
        previous->value_on == value_on);
    reuse_end = gap != NULL && end_delta == finish;

    free = ~queuedpulse_ctx.queue.entries_bitmap;

    if(waiting) {
      BREAK_IF_ACTION(free == 0u, err = E_OVERFLOW);
      index = POSITION_VAL(free);
      first = &queuedpulse_ctx.queue.entries[index];
      used = 1u << index;
      free &= ~used;
    }

    if(reuse_end) {
      last = gap;
    } else {
      BREAK_IF_ACTION(free == 0u, err = E_OVERFLOW);
      index = POSITION_VAL(free);
      last = &queuedpulse_ctx.queue.entries[index];
      used |= 1u << index;
    }

    if(start_delta == 0u && out->value_cur != value_on) {
      err = output_set_value(out->id, value_on);
      BREAK_IF(err != E_OK);
      out->value_cur = value_on;
    }

    if(!reuse_end) {
      last->timer = timer;
      last->output_assigned = out;
      last->pulse_deadline = time_add(now, end_delta);
      last->out_seq_next = gap;
    }

    last->value_on = value_on;
    if(waiting) {
      first->timer = timer;
      first->output_assigned = out;
      first->pulse_deadline = time_add(now, start_delta);
      first->value_on = out->value_off;
      first->out_seq_next = last;
    } else {
      first = last;
    }

    if(previous != NULL) {
      previous->out_seq_next = first;
    } else {
      out->queue_entry = first;
    }
    if(gap == NULL) {
      out->queue_tail = last;
    }
    if(waiting || !reuse_end) {
      queuedpulses_insert_pulse(timer, previous, first, reuse_end ? first : last, now);
    }

    queuedpulse_ctx.queue.entries_bitmap |= used;

    if(timer->entry_assigned != old_head) {
      queuedpulses_internal_tim_disable(timer);

      timer->output_assigned = timer->entry_assigned->output_assigned;

      queuedpulses_internal_tim_enable(timer,
          queuedpulses_remaining(timer->entry_assigned->pulse_deadline, time_now_us()),
          timer->prescaler_default);
    }

  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC static error_t queuedpulses_schedule_locked(queuedpulse_output_t *out,
    time_us_t start, time_us_t end, output_value_t value_on, bool keep_width,
    time_us_t now, time_delta_us_t width)
{
  queuedpulse_timer_t *timer;
  queuedpulse_entry_t *tail, *first, *last;
  time_delta_us_t lead, end_delta, tail_delta;
  const time_delta_us_t max_delay = time_mask_us() >> 1u;
  uint32_t free, first_id, last_id, used, index;
  bool waiting;
  error_t err = E_OK;
  output_id_t output = out->id;

  do {
    if(out->queue_entry == NULL && queuedpulses_remaining(start, now) != 0u &&
        out->value_cur != out->value_off) {
      err = output_set_value(output, out->value_off);
      BREAK_IF(err != E_OK);
      out->value_cur = out->value_off;
      now = time_now_us();
    }

    lead = queuedpulses_remaining(start, now);
    end_delta = queuedpulses_remaining(end, now);

    if(keep_width) {
      end_delta = lead + width;
    } else {
      BREAK_IF(end_delta == 0u);
    }

    tail = out->queue_tail;
    if(tail != NULL) {
      tail_delta = queuedpulses_remaining(tail->pulse_deadline, now);
      if(tail_delta > lead) {
        err = queuedpulses_schedule_gap_locked(out, lead, end_delta, value_on, keep_width, now, width);
        break;
      }
      if(tail_delta != 0u && tail_delta >= lead) {
        lead = tail_delta;
        if(keep_width) {
          end_delta = lead + width;
        } else {
          lead += QUEUEDPULSE_MINIMUM_PULSE_US;
        }
      }
    }

    BREAK_IF_ACTION(end_delta > max_delay, err = E_PARAM);
    BREAK_IF(end_delta <= lead || end_delta - lead < QUEUEDPULSE_MINIMUM_PULSE_US);

    start = time_add(now, lead);
    end = time_add(now, end_delta);

    waiting = lead != 0u && !(tail != NULL && start == tail->pulse_deadline &&
        tail->value_on == value_on);

    free = ~queuedpulse_ctx.queue.entries_bitmap;
    BREAK_IF_ACTION(free == 0u, err = E_OVERFLOW);
    first_id = POSITION_VAL(free);
    used = 1u << first_id;
    last_id = first_id;
    if(waiting) {
      free &= ~used;
      BREAK_IF_ACTION(free == 0u, err = E_OVERFLOW);
      last_id = POSITION_VAL(free);
      used |= 1u << last_id;
    }
    first = &queuedpulse_ctx.queue.entries[first_id];
    last = &queuedpulse_ctx.queue.entries[last_id];
    timer = out->timer;
    if(timer == NULL) {
      index = POSITION_VAL(~queuedpulse_ctx.timers_bitmap);
      if(index >= queuedpulse_ctx.timers_count) {
        index = queuedpulse_ctx.timer_index_next;
      }
      timer = &queuedpulse_ctx.timers[index];
    }

    if(out->queue_entry == NULL) {
      output_value_t value = waiting ? out->value_off : value_on;
      if(out->value_cur != value) {
        err = output_set_value(output, value);
        BREAK_IF(err != E_OK);
        out->value_cur = value;
      }
    }

    last->timer = timer;
    last->output_assigned = out;
    last->pulse_deadline = end;
    last->value_on = value_on;
    last->out_seq_next = NULL;
    if(waiting) {
      first->timer = timer;
      first->output_assigned = out;
      first->pulse_deadline = start;
      first->value_on = out->value_off;
      first->out_seq_next = last;
    }

    queuedpulses_insert_pulse(timer, tail, first, last, now);
    if(tail != NULL) {
      tail->out_seq_next = first;
    } else {
      out->queue_entry = first;
    }

    out->queue_tail = last;
    out->timer = timer;

    queuedpulse_ctx.queue.entries_bitmap |= used;
    if((queuedpulse_ctx.timers_bitmap & (1u << timer->id)) != 0u && tail == NULL) {
      queuedpulse_ctx.timer_index_next = timer->id + 1u;
      if(queuedpulse_ctx.timer_index_next >= queuedpulse_ctx.timers_count) {
        queuedpulse_ctx.timer_index_next = 0u;
      }
    }
    queuedpulse_ctx.timers_bitmap |= 1u << timer->id;
    if(timer->entry_assigned == first) {
      queuedpulses_internal_tim_disable(timer);
      timer->output_assigned = out;
      queuedpulses_internal_tim_enable(timer, queuedpulses_remaining(first->pulse_deadline, time_now_us()),
          timer->prescaler_default);
    }
  } while(0);

  return err;
}

OPTIMIZE_FAST
ITCM_FUNC error_t queuedpulses_schedule(output_id_t output, time_us_t start, time_us_t end,
    output_value_t value_on, bool keep_width)
{
  queuedpulse_output_t *out;
  queuedpulse_timer_t *timer;
  queuedpulse_entry_t *first, *last;
  const time_delta_us_t max_delay = time_mask_us() >> 1u;
  time_delta_us_t width = time_diff(end, start), lead;
  time_us_t now;
  uint32_t prim, free, rest, index;
  bool idle;
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(width < QUEUEDPULSE_MINIMUM_PULSE_US, err = E_OK);
    BREAK_IF_ACTION(width > max_delay, err = E_PARAM);
    BREAK_IF_ACTION(output >= OUTPUTS_CHS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(queuedpulse_ctx.timers_count == 0u, err = E_NOTRDY);

    out = &queuedpulse_ctx.outputs[output];

    prim = EnterCritical();

    now = time_now_us();
    lead = time_diff(start, now);

    BREAK_IF_ACTION(lead <= max_delay && width > max_delay - lead, (ExitCritical(prim), err = E_PARAM));

    if(out->timer == NULL && lead != 0u && lead <= max_delay &&
        lead + width <= max_delay && out->value_cur == out->value_off) {
      free = ~queuedpulse_ctx.queue.entries_bitmap;
      rest = free & (free - 1u);
      if(rest != 0u) {
        index = POSITION_VAL(~queuedpulse_ctx.timers_bitmap);
        idle = index < queuedpulse_ctx.timers_count;
        if(!idle) {
          index = queuedpulse_ctx.timer_index_next;
          queuedpulse_ctx.timer_index_next = index + 1u;
          if(queuedpulse_ctx.timer_index_next >= queuedpulse_ctx.timers_count) {
            queuedpulse_ctx.timer_index_next = 0u;
          }
        }
        timer = &queuedpulse_ctx.timers[index];
        index = POSITION_VAL(free);
        first = &queuedpulse_ctx.queue.entries[index];
        index = POSITION_VAL(rest);

        last = &queuedpulse_ctx.queue.entries[index];
        first->timer = last->timer = timer;
        first->output_assigned = last->output_assigned = out;
        first->pulse_deadline = start;
        first->value_on = out->value_off;
        first->out_seq_next = last;
        last->pulse_deadline = end;
        last->value_on = value_on;
        last->out_seq_next = NULL;

        if(idle) {
          first->next = last;
          last->next = NULL;
          timer->entry_assigned = first;
        } else {
          queuedpulses_insert_pulse(timer, NULL, first, last, now);
        }

        out->queue_entry = first;
        out->queue_tail = last;
        out->timer = timer;
        queuedpulse_ctx.queue.entries_bitmap = ~(rest & (rest - 1u));
        queuedpulse_ctx.timers_bitmap |= 1u << timer->id;
        if(timer->entry_assigned == first) {
          if(!idle) {
            queuedpulses_internal_tim_disable(timer);
          }
          timer->output_assigned = out;
          queuedpulses_internal_tim_enable(timer, queuedpulses_remaining(start, time_now_us()),
              timer->prescaler_default);
        }

        ExitCritical(prim);
        break;
      }
    }

    err = queuedpulses_schedule_locked(out, start, end, value_on, keep_width, now, width);

    ExitCritical(prim);

  } while(0);

  return err;
}

ITCM_FUNC error_t queuedpulses_enqueue_ex( output_id_t output, time_us_t deadline, output_value_t value_on)
{
  error_t err = E_OK;
  queuedpulse_output_t *out;
  queuedpulse_entry_t *entry;
  queuedpulse_entry_t *current;
  queuedpulse_entry_t *previous;
  queuedpulse_entry_t *previous_seq;
  queuedpulse_timer_t *timer;
  uint32_t index, prim;
  time_us_t now;
  time_delta_us_t delta;
  time_delta_us_t remaining;
  const time_us_t time_mask_2 = time_mask_us() >> 1u;
  bool timer_immediate_change;

  do {
    BREAK_IF_ACTION(output < 0 || output >= OUTPUTS_CHS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(queuedpulse_ctx.timers_count == 0, err = E_NOTRDY);

    out = &queuedpulse_ctx.outputs[output];

    prim = EnterCritical();

    now = time_now_us();
    delta = time_diff(deadline, now);

    if(delta < QUEUEDPULSE_MINIMUM_PULSE_US || delta > time_mask_2) {
      err = output_set_value(out->id, out->value_off);
      out->value_cur = out->value_off;
      ExitCritical(prim);
      break;
    }

    BREAK_IF_ACTION(queuedpulse_ctx.queue.entries_bitmap == QUEUEDPULSE_ENTRIES_BITMAP_MAX, (ExitCritical(prim), err = E_OVERFLOW));

    index = POSITION_VAL(~queuedpulse_ctx.queue.entries_bitmap);
    BREAK_IF_ACTION(index >= QUEUEDPULSE_QUEUE_ENTRIES, (ExitCritical(prim), err = E_FAULT));

    entry = &queuedpulse_ctx.queue.entries[index];
    timer = out->timer;

    if(timer == NULL) {
      index = POSITION_VAL(~queuedpulse_ctx.timers_bitmap);

      if(index < queuedpulse_ctx.timers_count) {
        timer = &queuedpulse_ctx.timers[index];
      } else {
        index = queuedpulse_ctx.timer_index_next;
        timer = &queuedpulse_ctx.timers[index];

        if(++index >= queuedpulse_ctx.timers_count) {
          index = 0;
        }

        queuedpulse_ctx.timer_index_next = index;
      }
    }
    previous = NULL;
    previous_seq = NULL;
    current = timer->entry_assigned;

    while(current != NULL) {
      remaining = time_diff(current->pulse_deadline, now);

      if(remaining > time_mask_2) {
        remaining = 0u;
      }

      BREAK_IF(remaining > delta);

      if(current->output_assigned == out) {
        previous_seq = current;
      }

      previous = current;
      current = current->next;
    }

    entry->pulse_deadline = deadline;
    entry->timer = timer;
    entry->output_assigned = out;
    entry->value_on = value_on;
    entry->next = current;

    if(previous_seq != NULL) {
      entry->out_seq_next = previous_seq->out_seq_next;
      previous_seq->out_seq_next = entry;
    } else {
      entry->out_seq_next = out->queue_entry;
      out->queue_entry = entry;

      if(out->value_cur != value_on) {
        err = output_set_value(out->id, value_on);
        out->value_cur = value_on;
      }
    }

    if(entry->out_seq_next == NULL) {
      out->queue_tail = entry;
    }

    timer_immediate_change = previous == NULL;
    if(previous != NULL) {
      previous->next = entry;
    } else {
      timer->entry_assigned = entry;
    }

    out->timer = timer;

    queuedpulse_ctx.queue.entries_bitmap |= 1u << entry->id;
    queuedpulse_ctx.timers_bitmap |= 1u << timer->id;

    if(timer_immediate_change) {
      queuedpulses_internal_tim_disable(timer);

      timer->entry_assigned = entry;
      timer->output_assigned = out;

      now = time_now_us();
      remaining = time_diff(deadline, now);
      if(remaining > time_mask_2) {
        remaining = 0u;
      }

      queuedpulses_internal_tim_enable(timer, remaining, timer->prescaler_default);
    }

    ExitCritical(prim);

  } while(0);

  return err;
}

ITCM_FUNC error_t queuedpulses_reset_output(output_id_t output)
{
  error_t err;
  queuedpulse_output_t *out;
  queuedpulse_timer_t *timer;
  queuedpulse_entry_t *entry;
  queuedpulse_entry_t **link;
  uint32_t prim;
  bool head_removed = false;
  time_delta_us_t remaining;
  const time_us_t time_mask_2 = time_mask_us() >> 1u;

  if(output < 0 || output >= OUTPUTS_CHS_MAX) {
    return E_PARAM;
  }

  prim = EnterCritical();
  out = &queuedpulse_ctx.outputs[output];
  timer = out->timer;

  if(timer != NULL) {
    if(timer->entry_assigned->output_assigned == out) {
      head_removed = true;
      queuedpulses_internal_tim_disable(timer);
    }

    link = &timer->entry_assigned;
    while((entry = *link) != NULL) {
      if(entry->output_assigned == out) {
        *link = entry->next;

        queuedpulse_ctx.queue.entries_bitmap &= ~(1u << entry->id);
        entry->next = NULL;
        entry->out_seq_next = NULL;
        entry->output_assigned = NULL;
        entry->timer = NULL;
        entry->pulse_deadline = 0u;
        entry->value_on = 0u;
      } else {
        link = &entry->next;
      }
    }

    if(head_removed) {
      entry = timer->entry_assigned;
      if(entry != NULL) {
        timer->output_assigned = entry->output_assigned;

        remaining = time_diff(entry->pulse_deadline, time_now_us());
        if(remaining > time_mask_2) {
          remaining = 0u;
        }

        queuedpulses_internal_tim_enable(timer, remaining, timer->prescaler_default);
      } else {
        timer->output_assigned = NULL;
        queuedpulse_ctx.timers_bitmap &= ~(1u << timer->id);
      }
    }
  }

  out->queue_entry = NULL;
  out->queue_tail = NULL;
  out->timer = NULL;
  err = output_set_value(out->id, out->value_off);
  out->value_cur = out->value_off;

  ExitCritical(prim);

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
    out->queue_tail = NULL;
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
    entry->pulse_deadline = 0u;
    entry->value_on = 0u;
  }

  queuedpulse_ctx.queue.entries_bitmap = 0u;
  queuedpulse_ctx.timers_bitmap = 0u;

  ExitCritical(prim);

  return err;
}
