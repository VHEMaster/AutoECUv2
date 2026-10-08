/*
 * versioned_knockwindow.h
 *
 *  Created on: 5 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_TIMINGS_INC_VERSIONED_KNOCKWINDOW_H_
#define CONFIG_VERSIONED_TIMINGS_INC_VERSIONED_KNOCKWINDOW_H_

#include "common.h"
#include "config_pulsedadc.h"

typedef enum {
  KNOCKWINDOW_CONFIG_VERSION_V1 = 0,
  KNOCKWINDOW_CONFIG_VERSION_MAX
}ecu_config_knockwindow_versions_t;

typedef enum {
  KNOCKWINDOW_CONFIG_SETUP_1 = 0,
  KNOCKWINDOW_CONFIG_SETUP_2,
  KNOCKWINDOW_CONFIG_SETUP_MAX
}knockwindow_config_setup_t;

typedef struct {
    bool enabled;
    ecu_cylinder_t cy;
}knockwindow_config_setup_cylinder_t;

typedef struct {
    bool enabled;
    ecu_device_pulsedadc_t pulsedadc_instance;

    knockwindow_config_setup_cylinder_t cylinders_supported[ECU_CYLINDER_MAX];

}knockwindow_config_setup_ctx_t;

typedef struct {
    bool enabled;

    float window_prepare_advance;
    float window_start_advance;
    float window_overflow_threshold;

    knockwindow_config_setup_ctx_t setups[KNOCKWINDOW_CONFIG_SETUP_MAX];

    uint32_t align ALIGNED_CACHE;
}knockwindow_config_v1_t ALIGNED_CACHE;

typedef knockwindow_config_v1_t knockwindow_config_t;




#endif /* CONFIG_VERSIONED_TIMINGS_INC_VERSIONED_KNOCKWINDOW_H_ */
