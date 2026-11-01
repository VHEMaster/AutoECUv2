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

#define ECU_DEVICES_PULSEDADC_CALLBACKS_MAX   8

typedef struct {
    pulsedadc_sampling_cplt_cb_t sampling_cplt_cb;
    pulsedadc_sampling_error_cb_t sampling_error_cb;
    void *usrdata;
}ecu_devices_pulsedadc_cb_t;

error_t ecu_devices_pulsedadc_init(ecu_device_pulsedadc_t instance, pulsedadc_ctx_t *ctx);
error_t ecu_devices_pulsedadc_register_cb(ecu_device_pulsedadc_t instance, pulsedadc_sampling_cplt_cb_t cplt_callback, pulsedadc_sampling_error_cb_t error_callback, void *usrdata);


#endif /* CONFIG_INC_CONFIG_PULSEDADC_H_ */
