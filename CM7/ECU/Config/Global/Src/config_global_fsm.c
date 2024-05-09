/*
 * config_global_fsm.c
 *
 *  Created on: May 1, 2024
 *      Author: VHEMaster
 */

#include "config_global_fsm.h"
#include "compiler.h"

static error_t ecu_config_global_fsm_flash_init(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_flash) {
      case ECU_CONFIG_FSM_FLASH_CONDITION:
        if(ctx->global_ready == true && ctx->process_flash_init == true && ctx->process_result == E_AGAIN) {
          ctx->process_instance = 0;
          ctx->flash_initialized = false;
          ctx->fsm_flash = ECU_CONFIG_FSM_FLASH_RESET;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_DEFINE:
        if(ctx->process_instance >= ctx->flash_ctx->instances_count) {
          err = E_OK;
          ctx->fsm_flash = ECU_CONFIG_FSM_FLASH_CONDITION;
          ctx->process_instance = 0;
          ctx->flash_initialized = true;
          ctx->process_result = err;
        } else {
          ctx->fsm_flash = ECU_CONFIG_FSM_FLASH_RESET;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_RESET:
        if(ctx->flash_ctx->reset_func != NULL) {
          err = ctx->flash_ctx->reset_func(ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->flash_ctx->reset_errcode == E_AGAIN ||
              ctx->flash_ctx->reset_errcode == E_OK) {
            ctx->flash_ctx->reset_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_flash = ECU_CONFIG_FSM_FLASH_DEFINE;
          ctx->process_instance++;
          continue;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t ecu_config_global_fsm_rst_cfg(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_rst_cfg) {
      case ECU_CONFIG_FSM_RST_CFG_CONDITION:
        if(ctx->global_ready == true && ctx->process_comps_init == true && ctx->process_result == E_AGAIN) {
          ctx->components_initialized = false;
          ctx->process_comp_type = 0;
          ctx->process_instance = 0;
          ctx->fsm_rst_cfg = ECU_CONFIG_FSM_RST_CFG_DEFINE;
          err = E_AGAIN;
          for(int c = 0; c < ctx->components_count; c++) {
            ctx->components[c].reset_errcode = err;
            ctx->components[c].config_errcode = err;
          }
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_RST_CFG_DEFINE:
        if(ctx->process_comp_type >= ctx->components_count) {
          ctx->components_initialized = true;
          ctx->fsm_rst_cfg = ECU_CONFIG_FSM_RST_CFG_CONDITION;
          err = E_OK;
          ctx->process_result = err;
        } else {
          err = E_AGAIN;
          if(ctx->process_instance >= ctx->components[ctx->process_comp_type].instances_count) {
            ctx->process_instance = 0;
            ctx->process_comp_type++;
          } else {
            ctx->fsm_rst_cfg = ECU_CONFIG_FSM_RST_CFG_RESET;
          }
          continue;
        }

        break;
      case ECU_CONFIG_FSM_RST_CFG_RESET:
        if(ctx->components[ctx->process_comp_type].reset_func != NULL) {
          err = ctx->components[ctx->process_comp_type].reset_func(ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->components[ctx->process_comp_type].reset_errcode == E_AGAIN ||
              ctx->components[ctx->process_comp_type].reset_errcode == E_OK) {
            ctx->components[ctx->process_comp_type].reset_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_rst_cfg = ECU_CONFIG_FSM_RST_CFG_CONFIG;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_RST_CFG_CONFIG:
        if(ctx->components[ctx->process_comp_type].configure_func != NULL) {
          err = ctx->components[ctx->process_comp_type].configure_func(ctx->process_instance, ctx->components[ctx->process_comp_type].generic.data_ptr +
              ctx->components[ctx->process_comp_type].generic.data_size * ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->components[ctx->process_comp_type].config_errcode == E_AGAIN ||
              ctx->components[ctx->process_comp_type].config_errcode == E_OK) {
            ctx->components[ctx->process_comp_type].config_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_rst_cfg = ECU_CONFIG_FSM_RST_CFG_DEFINE;
          ctx->process_instance++;
          continue;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

error_t ecu_config_global_fsm(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    switch(ctx->fsm_process) {
      case ECU_CONFIG_FSM_PROCESS_FLASH_INIT:
        err = ecu_config_global_fsm_flash_init(ctx);
        break;
      case ECU_CONFIG_FSM_PROCESS_CFG_RST:
        err = ecu_config_global_fsm_rst_cfg(ctx);
        break;
      default:
        break;
    }

    if(err == E_OK) {
      if(++ctx->fsm_process >= ECU_CONFIG_FSM_PROCESS_MAX) {
        ctx->fsm_process = 0;
        break;
      }
    } else {
      break;
    }
  }

  return err;
}
