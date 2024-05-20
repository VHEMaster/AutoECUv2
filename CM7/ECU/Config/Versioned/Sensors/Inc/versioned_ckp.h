/*
 * versioned_ckp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CKP_H_
#define CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CKP_H_

#include "config_gpio.h"
#include "time.h"
#include "compiler.h"
#include "bool.h"
#include <stdint.h>

typedef enum {
  CKP_CONFIG_VERSION_V1 = 0,
  CKP_CONFIG_VERSION_MAX
}ckp_config_versions_t;

typedef enum {
  CKP_CONFIG_SIGNAL_REF_TYPE_REGULAR_60_2 = 0,
  CKP_CONFIG_SIGNAL_REF_TYPE_MAX,
}ckp_config_signal_ref_type_t;

typedef struct {
    bool enabled;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    ckp_config_signal_ref_type_t signal_ref_type;

    uint32_t align ALIGNED_CACHE;
}ckp_config_v1_t ALIGNED_CACHE;

typedef ckp_config_v1_t ckp_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CKP_H_ */
