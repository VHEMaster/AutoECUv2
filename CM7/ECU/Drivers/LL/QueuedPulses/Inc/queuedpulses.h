/*
 * queuedpulses.h
 *
 *  Created on: Mar 25, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_LL_QUEUEDPULSES_INC_QUEUEDPULSES_H_
#define DRIVERS_LL_QUEUEDPULSES_INC_QUEUEDPULSES_H_

#include <limits.h>
#include "main.h"
#include "bool.h"
#include "time.h"
#include "gpio.h"
#include "outputs.h"

#define QUEUEDPULSE_TIMERS              4u
#define QUEUEDPULSE_QUEUE_ENTRIES       32u

error_t queuedpulses_init(void);
void queuedpulses_loop_main(void);
void queuedpulses_loop_slow(void);
void queuedpulses_loop_fast(void);

error_t queuedpulses_timer_register(TIM_HandleTypeDef *htim, IRQn_Type irq);
error_t queuedpulses_output_configure(output_id_t output, output_value_t value_on, output_value_t value_off);
error_t queuedpulses_enqueue(output_id_t output, time_delta_us_t pulse);
error_t queuedpulses_reset_all(void);


#endif /* DRIVERS_LL_QUEUEDPULSES_INC_QUEUEDPULSES_H_ */
