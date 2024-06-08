/*
 * config_pulsedadc.h
 *
 *  Created on: Apr 28, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_PULSEDADC_H_
#define CONFIG_INC_CONFIG_PULSEDADC_H_

#include "config_devices.h"
#include "pulsedadc.h"

error_t ecu_devices_pulsedadc_init(ecu_device_pulsedadc_t instance, pulsedadc_ctx_t *ctx);

#endif /* CONFIG_INC_CONFIG_PULSEDADC_H_ */
