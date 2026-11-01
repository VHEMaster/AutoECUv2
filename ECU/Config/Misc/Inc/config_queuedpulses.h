/*
 * config_queuedpulses.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_QUEUEDPULSES_H_
#define CONFIG_INC_CONFIG_QUEUEDPULSES_H_

#include "common.h"
#include "queuedpulses.h"

#define ECU_QUEUEDPULSES_TIMERS_MAX   4u

error_t ecu_config_queuedpulses_init(void);

#endif /* CONFIG_INC_CONFIG_QUEUEDPULSES_H_ */
