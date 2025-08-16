/*
 * core_powermoding.h
 *
 *  Created on: Aug 16, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_POWERMODING_INC_POWERMODING_H_
#define CORE_POWERMODING_INC_POWERMODING_H_

#include "core.h"

typedef enum {
  POWERMODING_USER_RSVD = 0,

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
  POWERMODING_MODE_RUNDOWN,
  POWERMODING_MODE_TURBOTIMER,
  POWERMODING_MODE_IGNITION,
  POWERMODING_MODE_RUNNING,
  POWERMODING_MODE_MAX
}ecu_core_powermoding_mode_t;

typedef enum {
  POWERMODING_MODE_REQ_STANDBY = 0,
  POWERMODING_MODE_REQ_SLEEP,
  POWERMODING_MODE_REQ_RUNDOWN,
  POWERMODING_MODE_REQ_RUNNING,
  POWERMODING_MODE_REQ_MAX
}ecu_core_powermoding_mode_request_t;

void core_powermoding_loop_slow(ecu_core_ctx_t *ctx);

error_t core_powermoding_user_mode_request(ecu_core_ctx_t *ctx, ecu_core_powermoding_user_t user, ecu_core_powermoding_mode_request_t mode_request);
error_t core_powermoding_user_mode_reset(ecu_core_ctx_t *ctx, ecu_core_powermoding_user_t user);
error_t core_powermoding_mode_get(ecu_core_ctx_t *ctx, ecu_core_powermoding_mode_t *mode);

#endif /* CORE_POWERMODING_INC_POWERMODING_H_ */
