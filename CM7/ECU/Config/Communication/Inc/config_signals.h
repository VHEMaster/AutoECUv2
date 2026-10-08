/*
 * config_signals.h
 *
 *  Created for periodic CAN signals
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_SIGNALS_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_SIGNALS_H_

#include "signals.h"

error_t ecu_comm_signals_init(void);
void ecu_comm_signals_loop_comm(void);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_SIGNALS_H_ */
