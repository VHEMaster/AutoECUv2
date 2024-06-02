/*
 * config.h
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_H_
#define CONFIG_INC_CONFIG_H_

#include "bool.h"
#include "config_spi.h"
#include "config_gpio.h"

#define ECU_TICKBASE_MASK   0x03FFFFFF
#define ECU_TIMEBASE_MASK   0x03FFFFFF

void ecu_config_ll_init(void);
void ecu_config_init_counter(void);
void ecu_config_start_counter(void);
void ecu_config_start_periodic_timers(pTIM_CallbackTypeDef func_tim_slow_irq_cb, pTIM_CallbackTypeDef func_tim_fast_irq_cb);

void ecu_config_set_ignition_enabled(bool enabled);
void ecu_config_iwgd_refresh(void);

#endif /* CONFIG_INC_CONFIG_H_ */
