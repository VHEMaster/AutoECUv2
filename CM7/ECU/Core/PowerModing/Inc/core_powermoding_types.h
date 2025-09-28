/*
 * core_powermoding_types.h
 *
 *  Created on: Aug 17, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_POWERMODING_INC_POWERMODING_TYPES_H_
#define CORE_POWERMODING_INC_POWERMODING_TYPES_H_

#include "common.h"
#include "config_modules_types.h"
#include "config_devices.h"

typedef enum {
  POWERMODING_USER_RSVD = 0,

  POWERMODING_USER_CAN,
  POWERMODING_USER_KLINE,
  POWERMODING_USER_IGNPOWER,

  POWERMODING_USER_FLASH,
  POWERMODING_USER_TURBOTIMER,

  POWERMODING_USER_ETCx,
  POWERMODING_USER_ETCx_LAST = POWERMODING_USER_ETCx + ECU_MODULE_ETC_MAX - 1,

  POWERMODING_USER_STEPPERx,
  POWERMODING_USER_STEPPERx_LAST = POWERMODING_USER_STEPPERx + ECU_DEVICE_STEPPER_MAX - 1,

  POWERMODING_USER_COOLINGFANx,
  POWERMODING_USER_COOLINGFANx_LAST = POWERMODING_USER_COOLINGFANx + ECU_MODULE_COOLINGFAN_MAX - 1,

  POWERMODING_USER_MAX
}ecu_core_powermoding_user_t;

typedef enum {
  POWERMODING_MODE_STANDBY = 0,
  POWERMODING_MODE_SLEEP,
  POWERMODING_MODE_COLDBOOT,
  POWERMODING_MODE_RUNDOWN,
  POWERMODING_MODE_TURBOTIMER,
  POWERMODING_MODE_IGNITION,
  POWERMODING_MODE_STARTING,
  POWERMODING_MODE_RUNNING,
  POWERMODING_MODE_MAX
}ecu_core_powermoding_mode_t;

typedef enum {
  POWERMODING_MODE_REQ_STANDBY = 0,
  POWERMODING_MODE_REQ_SLEEP,
  POWERMODING_MODE_REQ_COLDBOOT,
  POWERMODING_MODE_REQ_RUNDOWN,
  POWERMODING_MODE_REQ_RUNNING,
  POWERMODING_MODE_REQ_MAX
}ecu_core_powermoding_mode_request_t;

typedef struct {
    time_us_t mode_switch_timestamp;
    ecu_core_powermoding_mode_t mode_current;
    ecu_core_powermoding_mode_request_t mode_requested;
    ecu_core_powermoding_mode_request_t modes_requested[POWERMODING_USER_MAX];

    time_delta_us_t turbotimer_requested_time;
    time_delta_us_t turbotimer_passed_time;
}ecu_core_powermoding_ctx_t;

#endif /* CORE_POWERMODING_INC_POWERMODING_TYPES_H_ */
