/*
 * config_global_fsm.c
 *
 *  Created on: May 1, 2024
 *      Author: VHEMaster
 */

#include "config_global_fsm.h"
#include "compiler.h"

static error_t ecu_config_global_fsm_rst_cfg(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    switch(ctx->fsm_rst_cfg) {
      err = E_AGAIN;
      case ECU_CONFIG_FSM_RST_CFG_CONDITION:
        if(ctx->components_ready == true && ctx->process_trigger_type != ECU_CONFIG_RST_CFG_NONE && ctx->process_result == E_AGAIN) {
          switch(ctx->process_trigger_type) {
            case ECU_CONFIG_RST_CFG_RESET:
              ctx->components_initialized = false;
              break;
            case ECU_CONFIG_RST_CFG_CONFIGURE:
              ctx->components_configured = false;
              break;
            default:
              break;
          }
          ctx->process_comp_type = 0;
          ctx->process_instance = 0;
          ctx->fsm_rst_cfg = ECU_CONFIG_FSM_RST_CFG_PROCESS;
          err = E_AGAIN;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_RST_CFG_PROCESS:
        if(ctx->process_comp_type >= ctx->components_count) {
          switch(ctx->process_trigger_type) {
            case ECU_CONFIG_RST_CFG_RESET:
              ctx->components_initialized = true;
              break;
            case ECU_CONFIG_RST_CFG_CONFIGURE:
              ctx->components_configured = true;
              break;
            default:
              break;
          }
          ctx->fsm_rst_cfg = ECU_CONFIG_FSM_RST_CFG_CONDITION;
          err = E_OK;
          ctx->process_result = err;
        } else {
          if(ctx->process_instance >= ctx->components[ctx->process_comp_type].instances_count) {
            ctx->process_instance = 0;
            ctx->process_comp_type++;
            err = E_AGAIN;
            continue;
          } else {
            switch(ctx->process_trigger_type) {
              case ECU_CONFIG_RST_CFG_RESET:
                if(ctx->components[ctx->process_comp_type].reset_func != NULL) {
                  err = ctx->components[ctx->process_comp_type].reset_func(ctx->process_instance);
                } else {
                  err = E_OK;
                }
                ctx->components[ctx->process_comp_type].reset_errcode = err;
                //TODO: set DTC here
                break;
              case ECU_CONFIG_RST_CFG_CONFIGURE:
                if(ctx->components[ctx->process_comp_type].configure_func != NULL) {
                  err = ctx->components[ctx->process_comp_type].configure_func(ctx->process_instance, ctx->components[ctx->process_comp_type].data_ptr +
                      ctx->components[ctx->process_comp_type].data_size * ctx->process_instance);
                } else {
                  err = E_OK;
                }
                ctx->components[ctx->process_comp_type].config_errcode = err;
                //TODO: set DTC here
                break;
              default:
                err = E_INVALACT;
                break;
            }
            if(err != E_AGAIN) {
              ctx->process_instance++;
              err = E_AGAIN;
              continue;
            }
          }
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
