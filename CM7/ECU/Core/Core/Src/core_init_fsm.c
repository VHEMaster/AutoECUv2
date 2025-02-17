/*
 * core_init_fsm.c
 *
 *  Created on: Jun 23, 2024
 *      Author: VHEMaster
 */

#include "core_init_fsm.h"
#include "config_global.h"

void core_init_fsm(ecu_core_ctx_t *ctx)
{
  error_t err;

  while(true) {
    switch(ctx->fsm_core_init) {
      case ECU_CORE_INIT_FSM_INITIAL:
        if(ctx->core_init_request == true && ctx->core_init_errcode == E_AGAIN) {
          ctx->initialized = false;
          ctx->fsm_core_init++;
          continue;
        }
        break;
      case ECU_CORE_INIT_FSM_INIT_FLASH:
        err = ecu_config_global_flash_initialize();
        if(err == E_OK) {
          ctx->fsm_core_init++;
          continue;
        } else if(err != E_AGAIN) {
          ctx->core_init_errcode = err;
          ctx->fsm_core_init = ECU_CORE_INIT_FSM_INITIAL;
        }
        break;
      case ECU_CORE_INIT_FSM_READ_FLASH:
        err = ecu_config_global_operation(ECU_CONFIG_OP_READ, ECU_CONFIG_TYPE_ALL, 0, 0);
        if(err == E_OK) {
          ctx->fsm_core_init++;
          continue;
        } else if(err != E_AGAIN) {
          ctx->core_init_errcode = err;
          ctx->fsm_core_init = ECU_CORE_INIT_FSM_INITIAL;
        }
        break;
      case ECU_CORE_INIT_FSM_INIT_DEVS:
        err = ecu_config_global_devices_initialize();
        if(err == E_OK) {
          ctx->fsm_core_init++;
          continue;
        } else if(err != E_AGAIN) {
          ctx->core_init_errcode = err;
          ctx->fsm_core_init = ECU_CORE_INIT_FSM_INITIAL;
        }
        break;
      case ECU_CORE_INIT_FSM_INIT_SENS:
        err = ecu_config_global_sensors_initialize();
        if(err == E_OK) {
          ctx->fsm_core_init++;
          continue;
        } else if(err != E_AGAIN) {
          ctx->core_init_errcode = err;
          ctx->fsm_core_init = ECU_CORE_INIT_FSM_INITIAL;
        }
        break;
      case ECU_CORE_INIT_FSM_INIT_MODULES:
        err = ecu_config_global_modules_initialize();
        if(err == E_OK) {
          ctx->fsm_core_init++;
          continue;
        } else if(err != E_AGAIN) {
          ctx->core_init_errcode = err;
          ctx->fsm_core_init = ECU_CORE_INIT_FSM_INITIAL;
        }
        break;
      case ECU_CORE_INIT_FSM_COMPLETED:
        err = E_OK;
        ctx->core_init_errcode = err;
        ctx->fsm_core_init = ECU_CORE_INIT_FSM_INITIAL;
        ctx->initialized = true;
        break;
      default:
        break;
    }
    break;
  }
}
