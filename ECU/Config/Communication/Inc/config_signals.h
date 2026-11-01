/*
 * config_signals.h
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_COMMUNICATION_INC_CONFIG_SIGNALS_H_
#define CONFIG_COMMUNICATION_INC_CONFIG_SIGNALS_H_

#include "config_comm.h"
#include "signals.h"

error_t ecu_comm_signals_init(ecu_comm_signals_t instance, signals_ctx_t *ctx);
error_t ecu_comm_signals_get_default_config(ecu_comm_signals_t instance, signals_config_t *config);
error_t ecu_comm_signals_configure(ecu_comm_signals_t instance, const signals_config_t *config);
error_t ecu_comm_signals_reset(ecu_comm_signals_t instance);

#endif /* CONFIG_COMMUNICATION_INC_CONFIG_SIGNALS_H_ */
