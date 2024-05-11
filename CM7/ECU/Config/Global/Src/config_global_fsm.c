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

static void config_global_internal_calculate_index_max(ecu_config_global_runtime_ctx_t *ctx)
{
  switch(ctx->op_type_index) {
    case ECU_CONFIG_TYPE_COMPONENT:
      ctx->op_index_max = ctx->components_count;
      break;
    case ECU_CONFIG_TYPE_CALIBRATION:
      ctx->op_index_max = ctx->calibrations_count;
      break;
    case ECU_CONFIG_TYPE_RUNTIME:
      ctx->op_index_max = ctx->runtimes_count;
      break;
    default:
      ctx->op_index_max = 0;
      break;
  }
}

static void config_global_internal_calculate_instance_max(ecu_config_global_runtime_ctx_t *ctx)
{
  switch(ctx->op_type_index) {
    case ECU_CONFIG_TYPE_COMPONENT:
      ctx->op_instance_max = ctx->components[ctx->op_index].instances_count;
      break;
    case ECU_CONFIG_TYPE_CALIBRATION:
      ctx->op_instance_max = 1;
      break;
    case ECU_CONFIG_TYPE_RUNTIME:
      ctx->op_instance_max = 1;
      break;
    default:
      ctx->op_instance_max = 0;
      break;
  }
}

static error_t ecu_config_global_fsm_operation(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;
  const ecu_config_generic_ctx_t *req_ctx;
  uint32_t addr;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_operation) {
      case ECU_CONFIG_FSM_OPERATION_CONDITION:
        if(ctx->global_ready == true && ctx->op_request > ECU_CONFIG_OP_NONE && ctx->op_req_errcode == E_AGAIN) {
          if(ctx->op_req_type >= ECU_CONFIG_TYPE_ALL) {
            ctx->op_type_index = 0;
            ctx->op_index = 0;
            ctx->op_instance = 0;
            config_global_internal_calculate_index_max(ctx);
            config_global_internal_calculate_instance_max(ctx);
          } else {
            ctx->op_type_index = ctx->op_req_type;
            config_global_internal_calculate_index_max(ctx);
            if(ctx->op_req_index >= ctx->op_index_max) {
              ctx->op_index = 0;
              ctx->op_instance = 0;
            } else {
              ctx->op_index = ctx->op_req_index;
              config_global_internal_calculate_instance_max(ctx);
              if(ctx->op_req_instance >= ctx->op_instance_max) {
                ctx->op_instance = 0;
              } else {
                ctx->op_instance = ctx->op_req_instance;
              }
            }
          }

          ctx->op_req_errcode_internal = E_AGAIN;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_LOCK;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_LOCK:
        err = flash_lock();
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_DEFINE;
          continue;
        } else if(err != E_AGAIN) {
          ctx->op_req_errcode = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_CONDITION;
          break;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_DEFINE:
        if(ctx->op_type_index >= ECU_CONFIG_TYPE_ALL) {
          err = E_OK;
          ctx->op_req_errcode_internal = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
        } else {
          err = E_AGAIN;

          if(ctx->op_index >= ctx->op_index_max) {
            if(ctx->op_req_type >= ECU_CONFIG_TYPE_ALL) {
              ctx->op_type_index++;
              ctx->op_index = 0;
              ctx->op_instance = 0;
              config_global_internal_calculate_index_max(ctx);
            } else {
              err = E_OK;
              ctx->op_req_errcode_internal = err;
              ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
            }
          } else {
            switch(ctx->op_type_index) {
              case ECU_CONFIG_TYPE_COMPONENT:
                req_ctx = &ctx->components[ctx->op_index].generic;
                break;
              case ECU_CONFIG_TYPE_CALIBRATION:
                req_ctx = &ctx->calibrations[ctx->op_index];
                break;
              case ECU_CONFIG_TYPE_RUNTIME:
                req_ctx = &ctx->runtimes[ctx->op_index];
                break;
              default:
                req_ctx = NULL;
                break;
            }

            if(ctx->op_instance >= ctx->op_instance_max) {
              if(ctx->op_req_index >= ctx->op_index_max || ctx->op_req_type >= ECU_CONFIG_TYPE_ALL) {
                ctx->op_index++;
                ctx->op_instance = 0;
                config_global_internal_calculate_instance_max(ctx);
              } else {
                err = E_OK;
                ctx->op_req_errcode_internal = err;
                ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
              }
            } else {
              if(req_ctx != NULL) {
                ctx->op_req_ctx.flash_section_type = req_ctx->flash_section_type;
                ctx->op_req_ctx.get_default_cfg_func = req_ctx->get_default_cfg_func;
                ctx->op_req_ctx.version_current = req_ctx->versions_count - 1;
                ctx->op_req_ctx.versions = req_ctx->versions;
                ctx->op_version = ctx->op_req_ctx.version_current;
                ctx->op_req_ctx.data_ptr = req_ctx->data_ptr;
                ctx->op_req_ctx.data_size = req_ctx->data_size;

                ctx->op_req_ctx.data_ptr += req_ctx->data_size * ctx->op_instance;
              } else {
                err = E_PARAM;
                ctx->op_req_errcode_internal = err;
                ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
                continue;
              }

              ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_REQUEST;
            }
          }
        }
        continue;
      case ECU_CONFIG_FSM_OPERATION_REQUEST:
        if(ctx->op_request == ECU_CONFIG_OP_READ) {
          err = flash_section_read(ctx->op_req_ctx.flash_section_type, ctx->op_instance, &ctx->op_version, ctx->op_req_ctx.data_ptr, ctx->op_req_ctx.data_size);
        } else if(ctx->op_request == ECU_CONFIG_OP_WRITE) {
          err = flash_section_write(ctx->op_req_ctx.flash_section_type, ctx->op_instance, ctx->op_version, ctx->op_req_ctx.data_ptr, ctx->op_req_ctx.data_size);
        } else {
          err = E_INVALACT;
        }
        if(err != E_AGAIN) {
          if(ctx->op_request == ECU_CONFIG_OP_READ) {
            if(err == E_BADVALUE) {
              // TODO: set DTC here?
              if(ctx->op_req_ctx.get_default_cfg_func != NULL) {
                err = ctx->op_req_ctx.get_default_cfg_func(ctx->op_instance, ctx->op_req_ctx.data_ptr);
              } else {
                // TODO: set DTC here?
                err = E_OK;
              }
            } else if(ctx->op_version != ctx->op_req_ctx.version_current) {
              if(ctx->op_version > ctx->op_req_ctx.version_current) {
                if(ctx->op_req_ctx.get_default_cfg_func != NULL) {
                  err = ctx->op_req_ctx.get_default_cfg_func(ctx->op_instance, ctx->op_req_ctx.data_ptr);
                } else {
                  // TODO: set DTC here?
                  err = E_OK;
                }

                err = E_BADVALUE;
                // TODO: set DTC here?
                err = E_OK;
              } else {
                do {
                  addr = 0;
                  err = flash_mem_base_address(&addr);
                  BREAK_IF(err != E_OK);
                  ctx->op_translate_ptr = addr;
                  err = flash_mem_section_address(&addr, ctx->op_req_ctx.flash_section_type, ctx->op_instance);
                  BREAK_IF(err != E_OK);
                  ctx->op_translate_ptr += addr;
                  err = flash_mem_header_len(&addr);
                  BREAK_IF(err != E_OK);
                  ctx->op_translate_ptr += addr;
                } while(0);

                if(err != E_OK) {
                  err = E_FAULT;
                  ctx->op_req_errcode_internal = err;
                  ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
                  continue;
                }

                err = E_AGAIN;
                ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_ACQUIRE;
                continue;
              }
            }
          }

          if(err == E_OK) {
            if(ctx->op_req_instance >= ctx->op_instance_max || ctx->op_req_index >= ctx->op_index_max || ctx->op_req_type >= ECU_CONFIG_TYPE_ALL) {
              err = E_AGAIN;
              ctx->op_instance++;
              ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_DEFINE;
            } else {
              err = E_OK;
              ctx->op_req_errcode_internal = err;
              ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
            }
          } else {
            err = E_PARAM;
            ctx->op_req_errcode_internal = err;
            ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
          }
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_TRANSLATE_ACQUIRE:
        err = ecu_devices_get_flash_ctx(ECU_DEVICE_FLASH_1, &ctx->qspi_ctx);
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_CHECK;
          continue;
        } else if(err != E_AGAIN) {
          ctx->op_req_errcode_internal = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_TRANSLATE_CHECK:
        if(ctx->op_version == ctx->op_req_ctx.version_current) {
          err = E_AGAIN;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_REQUEST;
          continue;
        } else if(ctx->op_version > ctx->op_req_ctx.version_current) {
          err = E_FAULT;
        } else if(ctx->op_req_ctx.versions[ctx->op_version + 1].size < ctx->op_req_ctx.data_size) {
          err = E_FAULT;
        } else {
          err = E_OK;
        }

        if(err == E_OK) {
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_LOCK;
          continue;
        } else if(err != E_AGAIN) {
          ctx->op_req_errcode_internal = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
          continue;
        }
        continue;
      case ECU_CONFIG_FSM_OPERATION_TRANSLATE_LOCK:
        err = qspi_lock(ctx->qspi_ctx);
        if(err == E_OK) {
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_MAPPING_ENABLE;
          continue;
        } else if(err != E_AGAIN) {
          ctx->op_req_errcode_internal = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_TRANSLATE_MAPPING_ENABLE:
        err = qspi_memory_mapping_set(ctx->qspi_ctx, true);
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE;
          continue;
        } else if(err != E_AGAIN) {
          ctx->op_req_errcode_internal = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_UNLOCK;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_TRANSLATE:
        if(ctx->op_req_ctx.versions[ctx->op_version + 1].translate_func != NULL) {
          err = ctx->op_req_ctx.versions[ctx->op_version + 1].translate_func((void*)ctx->op_translate_ptr, ctx->op_req_ctx.data_ptr, ctx->op_req_ctx.versions[ctx->op_version + 1].size);
        } else {
          err = E_OK;
        }

        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_MAPPING_DISABLE;
          continue;
        } else if(err != E_AGAIN) {
          if(ctx->op_req_errcode_internal == E_AGAIN) {
            ctx->op_req_errcode_internal = err;
          }
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_MAPPING_DISABLE;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_TRANSLATE_MAPPING_DISABLE:
        err = qspi_memory_mapping_set(ctx->qspi_ctx, false);
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_UNLOCK;
          continue;
        } else if(err != E_AGAIN) {
          if(ctx->op_req_errcode_internal == E_AGAIN) {
            ctx->op_req_errcode_internal = err;
          }
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_UNLOCK;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_TRANSLATE_UNLOCK:
        err = qspi_unlock(ctx->qspi_ctx);
        if(err == E_OK) {
          if(ctx->op_req_errcode_internal != E_AGAIN) {
            err = ctx->op_req_errcode_internal;
            ctx->op_req_errcode = err;
            ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
          } else {
            ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_WRITE;
          }
          continue;
        } else if(err != E_AGAIN) {
          ctx->op_req_errcode_internal = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_TRANSLATE_WRITE:
        err = flash_section_write(ctx->op_req_ctx.flash_section_type, ctx->op_instance, ctx->op_version + 1, ctx->op_req_ctx.data_ptr, ctx->op_req_ctx.versions[ctx->op_version + 1].size);
        if(err == E_OK) {
          ctx->op_version++;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_TRANSLATE_CHECK;
          continue;
        } else if(err != E_AGAIN) {
          ctx->op_req_errcode_internal = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_UNLOCK:
        err = flash_unlock();
        if(err != E_AGAIN) {
          if(ctx->op_req_errcode_internal != E_AGAIN) {
            err = ctx->op_req_errcode_internal;
          }
          ctx->op_req_errcode = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_CONDITION;
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
      case ECU_CONFIG_FSM_PROCESS_OPERATION:
        err = ecu_config_global_fsm_operation(ctx);
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
