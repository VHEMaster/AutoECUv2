/*
 * config_queuedpulses.c
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#include "config_queuedpulses.h"
#include "config_extern.h"
#include "compiler.h"

typedef struct {
    TIM_HandleTypeDef *htim;
    IRQn_Type irq;
} ecu_config_queuedpulses_timer_t;

typedef struct {
    ecu_config_queuedpulses_timer_t timers[ECU_QUEUEDPULSES_TIMERS_MAX];
} ecu_config_queuedpulses_t;

static ecu_config_queuedpulses_t ecu_config_queuedpulses = {
    .timers = {
        {
          .htim = &htim13,
          .irq = TIM8_UP_TIM13_IRQn,
        },
        {
          .htim = &htim14,
          .irq = TIM8_TRG_COM_TIM14_IRQn,
        },
        {
          .htim = &htim16,
          .irq = TIM16_IRQn,
        },
        {
          .htim = &htim17,
          .irq = TIM17_IRQn,
        },
    },
};

error_t ecu_config_queuedpulses_init(void)
{
  error_t err = E_OK;

  do {
    err = queuedpulses_init();
    BREAK_IF(err != E_OK);

    for(int i = 0; i < ITEMSOF(ecu_config_queuedpulses.timers); i++) {
      err = queuedpulses_timer_register(ecu_config_queuedpulses.timers[i].htim, ecu_config_queuedpulses.timers[i].irq);
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
