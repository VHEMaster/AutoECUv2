/*
 * config_loop.h
 *
 *  Created on: Apr 24, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_LOOP_H_
#define CONFIG_INC_CONFIG_LOOP_H_

#include "errors.h"
#include "time.h"

typedef void (*ecu_config_loop_cb_t)(void *usrdata);

void ecu_config_loop_main(void);
void ecu_config_loop_slow(void);
void ecu_config_loop_fast(void);

error_t ecu_config_loop_register_main(ecu_config_loop_cb_t func, void *usrdata, time_delta_us_t period);
error_t ecu_config_loop_register_slow(ecu_config_loop_cb_t func, void *usrdata, time_delta_us_t period);
error_t ecu_config_loop_register_fast(ecu_config_loop_cb_t func, void *usrdata, time_delta_us_t period);

#endif /* CONFIG_INC_CONFIG_LOOP_H_ */
