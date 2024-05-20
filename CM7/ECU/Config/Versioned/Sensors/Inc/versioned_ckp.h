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


typedef struct {
    bool enabled;

    time_delta_us_t boot_time;

    ecu_gpio_input_pin_t input_pin;

    uint32_t align ALIGNED_CACHE;
}ckp_config_v1_t ALIGNED_CACHE;

typedef ckp_config_v1_t ckp_config_t;

#endif /* CONFIG_VERSIONED_SENSORS_INC_VERSIONED_CKP_H_ */
