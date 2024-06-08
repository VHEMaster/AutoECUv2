/*
 * queuedpulses_internal.h
 *
 *  Created on: Mar 26, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_QUEUEDPULSES_INC_QUEUEDPULSES_INTERNAL_H_
#define DRIVERS_QUEUEDPULSES_INC_QUEUEDPULSES_INTERNAL_H_

#include "queuedpulses.h"

#define QUEUEDPULSE_ENTRIES_BITMAP_MAX  UINT_MAX

typedef struct queuedpulse_entry_tag queuedpulse_entry_t;
typedef struct queuedpulse_timer_tag queuedpulse_timer_t;
typedef struct queuedpulse_output_tag queuedpulse_output_t;

typedef struct queuedpulse_entry_tag {
    queuedpulse_timer_t *timer;
    queuedpulse_output_t *output_assigned;
    time_delta_us_t pulse;
    time_us_t time;
    queuedpulse_entry_t *next;
    queuedpulse_entry_t *out_seq_next;
    uint32_t id;
    output_value_t value_on;
}queuedpulse_entry_t;

typedef struct queuedpulse_timer_tag {
    TIM_HandleTypeDef *htim;
    IRQn_Type irq;
    uint32_t prescaler_default;
    queuedpulse_entry_t *entry_assigned;
    queuedpulse_output_t *output_assigned;
    uint32_t id;
}queuedpulse_timer_t;

typedef struct queuedpulse_output_tag {
    queuedpulse_entry_t *queue_entry;
    queuedpulse_timer_t *timer;
    output_value_t value_on;
    output_value_t value_off;
    output_value_t value_cur;
    output_id_t id;
}queuedpulse_output_t;

typedef struct {
    queuedpulse_entry_t entries[QUEUEDPULSE_QUEUE_ENTRIES];
    uint32_t entries_bitmap;
}queuedpulse_queue_t;

typedef struct {
    queuedpulse_queue_t queue;
    queuedpulse_output_t outputs[OUTPUTS_CHS_MAX];
    queuedpulse_timer_t timers[QUEUEDPULSE_TIMERS];
    uint32_t timers_bitmap;
    uint32_t timers_count;

}queuedpulse_ctx_t;

void queuedpulses_internal_tim_enable(queuedpulse_timer_t *timer, uint32_t prd, uint32_t psc);
void queuedpulses_internal_tim_disable(queuedpulse_timer_t *timer);

time_delta_us_t queuedpulses_internal_calculate_pulse_cplt_time(queuedpulse_entry_t *entry, time_us_t now);

#endif /* DRIVERS_QUEUEDPULSES_INC_QUEUEDPULSES_INTERNAL_H_ */
