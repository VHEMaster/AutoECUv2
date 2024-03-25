/*
 * queuedpulses.h
 *
 *  Created on: Mar 25, 2024
 *      Author: VHEMaster
 */

#ifndef ECU_QUEUEDPULSES_INC_QUEUEDPULSES_H_
#define ECU_QUEUEDPULSES_INC_QUEUEDPULSES_H_

#include <limits.h>
#include "main.h"
#include "bool.h"
#include "time.h"
#include "gpio.h"
#include "outputs.h"

#define QUEUEDPULSE_TIMERS              4u
#define QUEUEDPULSE_QUEUE_ENTRIES       32u
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
    uint32_t id;
}queuedpulse_entry_t;

typedef struct queuedpulse_timer_tag {
    TIM_HandleTypeDef *htim;
    IRQn_Type irq;
    uint32_t prescaler_default;
    queuedpulse_entry_t *entry_assigned;
    queuedpulse_output_t *output_assigned;
}queuedpulse_timer_t;

typedef struct queuedpulse_output_tag {
    queuedpulse_entry_t *queue_entry;
    queuedpulse_timer_t *timer;
    output_value_t value_on;
    output_value_t value_off;
    output_id_t id;
}queuedpulse_output_t;

typedef struct {
    queuedpulse_entry_t entries[QUEUEDPULSE_QUEUE_ENTRIES];
    uint32_t entries_bitmap;
    uint8_t index_next;
}queuedpulse_queue_t;

typedef struct {
    queuedpulse_queue_t queue;
    queuedpulse_output_t outputs[OUTPUTS_CHS_MAX];
    queuedpulse_timer_t timers[QUEUEDPULSE_TIMERS];
    uint32_t timers_count;
    uint8_t timer_next;

}queuedpulse_ctx_t;

error_t queuedpulses_init(void);
void queuedpulses_loop_main(void);
void queuedpulses_loop_slow(void);
void queuedpulses_loop_fast(void);

void queuedpulses_tim_irq_handler(TIM_HandleTypeDef *htim);

error_t queuedpulses_timer_register(TIM_HandleTypeDef *htim, IRQn_Type irq);
error_t queuedpulses_output_configure(output_id_t output, output_value_t value_on, output_value_t value_off);
error_t queuedpulses_enqueue(output_id_t output, time_delta_us_t pulse);
error_t queuedpulses_reset(output_id_t output);
error_t queuedpulses_reset_all(void);


#endif /* ECU_QUEUEDPULSES_INC_QUEUEDPULSES_H_ */
