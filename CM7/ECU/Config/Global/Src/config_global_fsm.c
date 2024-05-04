/*
 * config_global_fsm.c
 *
 *  Created on: May 1, 2024
 *      Author: VHEMaster
 */

#include "config_global_fsm.h"
#include "compiler.h"

error_t ecu_config_global_fsm(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    BREAK_IF_ACTION(ctx->components_ready != true, err = E_NOTRDY);

    if(ctx->process_trigger_type != ECU_CONFIG_PROC_TRIG_TYPE_NONE) {
      if(ctx->process_comp_type >= ctx->components_count) {

        switch(ctx->process_trigger_type) {
          case ECU_CONFIG_PROC_TRIG_TYPE_RESET:
            ctx->components_initialized = true;
            break;
          case ECU_CONFIG_PROC_TRIG_TYPE_CONFIGURE:
            ctx->components_configured = true;
            break;
          default:
            break;
        }
        ctx->process_result = E_OK;
      } else {
        if(ctx->process_instance >= ctx->components[ctx->process_comp_type].instances_count) {
          ctx->process_instance = 0;
          ctx->process_comp_type++;
          continue;
        } else {
          switch(ctx->process_trigger_type) {
            case ECU_CONFIG_PROC_TRIG_TYPE_RESET:
              if(ctx->components[ctx->process_comp_type].reset_func != NULL) {
                err = ctx->components[ctx->process_comp_type].reset_func(ctx->process_instance);
              } else {
                err = E_OK;
              }
              ctx->components[ctx->process_comp_type].reset_errcode = err;
              //TODO: set DTC here
              break;
            case ECU_CONFIG_PROC_TRIG_TYPE_CONFIGURE:
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
            continue;
          }
        }
      }
    }
    break;
  }

  return err;
}
