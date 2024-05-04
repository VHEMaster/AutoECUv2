/*
 * config_engine.h
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_
#define CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_

#include "versioned_sw.h"
#include "versioned_hw.h"

typedef struct {
    l9966_config_data_t flexio[ECU_DEVICE_FLEXIO_MAX];
    l9960_config_t motor[ECU_DEVICE_MOTOR_MAX];
    tle4729_config_t stepper[ECU_DEVICE_STEPPER_MAX];
    cj125_config_t wbls[ECU_DEVICE_WBLS_MAX];

}ecu_config_engine_hw_t;

typedef struct {
    ecu_config_data_identification_t id;

}ecu_config_engine_sw_t;

typedef struct {
    ecu_config_engine_hw_t hw;
    ecu_config_engine_sw_t sw;
}ecu_config_global_engine_t;

#endif /* CONFIG_GLOBAL_INC_CONFIG_ENGINE_H_ */
