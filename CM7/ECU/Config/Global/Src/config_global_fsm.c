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
        if(ctx->global_ready == true && ctx->process_type == ECU_CONFIG_PROCESS_TYPE_FLASH_INIT && ctx->process_result == E_AGAIN) {
          ctx->process_instance = 0;
          ctx->flash_initialized = false;
          ctx->fsm_flash = ECU_CONFIG_FSM_FLASH_RESET;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_DEFINE:
        if(ctx->process_instance >= ctx->flash_config->instances_count) {
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
        if(ctx->flash_config->reset_func != NULL) {
          err = ctx->flash_config->reset_func(ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_flash = ECU_CONFIG_FSM_FLASH_DEFINE;
          ctx->process_instance++;
          continue;
        } if(err != E_AGAIN) {
          ctx->process_result = err;
          ctx->fsm_flash = ECU_CONFIG_FSM_FLASH_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t ecu_config_global_fsm_flash_erase(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_flash_erase) {
      case ECU_CONFIG_FSM_FLASH_ERASE_CONDITION:
        if(ctx->global_ready == true && ctx->process_type == ECU_CONFIG_PROCESS_TYPE_FLASH_ERASE && ctx->process_result == E_AGAIN) {
          ctx->op_req_errcode_internal = E_AGAIN;
          ctx->process_instance = 0;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_DEFINE;
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_ERASE_DEFINE:
        if(ctx->process_instance >= ctx->flash_config->instances_count) {
          err = E_OK;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_CONDITION;
          ctx->process_instance = 0;
          ctx->process_result = err;
        } else {
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_ACQUIRE;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_ERASE_ACQUIRE:
        err = ecu_devices_get_flash_ctx(ctx->process_instance, &ctx->qspi_ctx);
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_LOCK;
          continue;
        } else if(err != E_AGAIN) {
          ctx->process_result = err;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_CONDITION;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_ERASE_LOCK:
        err = qspi_lock(ctx->qspi_ctx);
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_PERFORM;
          continue;
        } if(err != E_AGAIN) {
          ctx->process_result = err;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_CONDITION;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_ERASE_PERFORM:
        err = qspi_chip_erase(ctx->qspi_ctx);
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_SYNC;
          continue;
        } if(err != E_AGAIN) {
          ctx->process_result = err;
          ctx->op_req_errcode_internal = err;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_UNLOCK;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_ERASE_SYNC:
        err = qspi_sync(ctx->qspi_ctx);
        if(err == E_OK) {
          err = E_AGAIN;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_UNLOCK;
          continue;
        } if(err != E_AGAIN) {
          ctx->process_result = err;
          ctx->op_req_errcode_internal = err;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_UNLOCK;
        }
        break;
      case ECU_CONFIG_FSM_FLASH_ERASE_UNLOCK:
        err = qspi_unlock(ctx->qspi_ctx);
        if(err == E_OK) {
          err = ctx->op_req_errcode_internal;
          if(err == E_AGAIN) {
            ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_DEFINE;
            ctx->process_instance++;
          } else {
            ctx->process_result = err;
            ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_CONDITION;
          }
          continue;
        } if(err != E_AGAIN) {
          ctx->process_result = err;
          ctx->fsm_flash_erase = ECU_CONFIG_FSM_FLASH_ERASE_CONDITION;
        }
        break;
      default:
        break;
    }
    break;
  }

  return err;
}

static error_t ecu_config_global_fsm_device_cfg(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_device_cfg) {
      case ECU_CONFIG_FSM_DEVICE_CFG_CONDITION:
        if(ctx->global_ready == true && ctx->process_type == ECU_CONFIG_PROCESS_TYPE_DEVICE_INIT && ctx->process_result == E_AGAIN) {
          ctx->devices_initialized = false;
          ctx->process_sens_type = 0;
          ctx->process_instance = 0;
          ctx->fsm_device_cfg = ECU_CONFIG_FSM_DEVICE_CFG_DEFINE;
          err = E_AGAIN;
          for(int c = 0; c < ctx->devices_count; c++) {
            ctx->devices_ctx[c].reset_errcode = err;
            ctx->devices_ctx[c].config_errcode = err;
          }
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_DEVICE_CFG_DEFINE:
        if(ctx->process_sens_type >= ctx->devices_count) {
          ctx->devices_initialized = true;
          ctx->fsm_device_cfg = ECU_CONFIG_FSM_DEVICE_CFG_CONDITION;
          err = E_OK;
          ctx->process_result = err;
        } else {
          err = E_AGAIN;
          if(ctx->process_instance >= ctx->devices_config[ctx->process_sens_type].instances_count) {
            ctx->process_instance = 0;
            ctx->process_sens_type++;
          } else {
            ctx->fsm_device_cfg = ECU_CONFIG_FSM_DEVICE_CFG_RESET;
          }
          continue;
        }

        break;
      case ECU_CONFIG_FSM_DEVICE_CFG_RESET:
        if(ctx->devices_config[ctx->process_sens_type].reset_func != NULL) {
          err = ctx->devices_config[ctx->process_sens_type].reset_func(ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->devices_ctx[ctx->process_sens_type].reset_errcode == E_AGAIN ||
              ctx->devices_ctx[ctx->process_sens_type].reset_errcode == E_OK) {
            ctx->devices_ctx[ctx->process_sens_type].reset_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_device_cfg = ECU_CONFIG_FSM_DEVICE_CFG_CONFIG;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_DEVICE_CFG_CONFIG:
        if(ctx->devices_config[ctx->process_sens_type].configure_func != NULL) {
          err = ctx->devices_config[ctx->process_sens_type].configure_func(ctx->process_instance, ctx->devices_config[ctx->process_sens_type].generic.data_ptr +
              ctx->devices_config[ctx->process_sens_type].generic.data_size * ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->devices_ctx[ctx->process_sens_type].config_errcode == E_AGAIN ||
              ctx->devices_ctx[ctx->process_sens_type].config_errcode == E_OK) {
            ctx->devices_ctx[ctx->process_sens_type].config_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_device_cfg = ECU_CONFIG_FSM_DEVICE_CFG_DEFINE;
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

static error_t ecu_config_global_fsm_sensor_cfg(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_sensor_cfg) {
      case ECU_CONFIG_FSM_SENSOR_CFG_CONDITION:
        if(ctx->global_ready == true && ctx->process_type == ECU_CONFIG_PROCESS_TYPE_SENSOR_INIT && ctx->process_result == E_AGAIN) {
          ctx->sensors_initialized = false;
          ctx->process_sens_type = 0;
          ctx->process_instance = 0;
          ctx->fsm_sensor_cfg = ECU_CONFIG_FSM_SENSOR_CFG_DEFINE;
          err = E_AGAIN;
          for(int c = 0; c < ctx->sensors_count; c++) {
            ctx->sensors_ctx[c].reset_errcode = err;
            ctx->sensors_ctx[c].config_errcode = err;
          }
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_SENSOR_CFG_DEFINE:
        if(ctx->process_sens_type >= ctx->sensors_count) {
          ctx->sensors_initialized = true;
          ctx->fsm_sensor_cfg = ECU_CONFIG_FSM_SENSOR_CFG_CONDITION;
          err = E_OK;
          ctx->process_result = err;
        } else {
          err = E_AGAIN;
          if(ctx->process_instance >= ctx->sensors_config[ctx->process_sens_type].instances_count) {
            ctx->process_instance = 0;
            ctx->process_sens_type++;
          } else {
            ctx->fsm_sensor_cfg = ECU_CONFIG_FSM_SENSOR_CFG_RESET;
          }
          continue;
        }

        break;
      case ECU_CONFIG_FSM_SENSOR_CFG_RESET:
        if(ctx->sensors_config[ctx->process_sens_type].reset_func != NULL) {
          err = ctx->sensors_config[ctx->process_sens_type].reset_func(ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->sensors_ctx[ctx->process_sens_type].reset_errcode == E_AGAIN ||
              ctx->sensors_ctx[ctx->process_sens_type].reset_errcode == E_OK) {
            ctx->sensors_ctx[ctx->process_sens_type].reset_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_sensor_cfg = ECU_CONFIG_FSM_SENSOR_CFG_CONFIG;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_SENSOR_CFG_CONFIG:
        if(ctx->sensors_config[ctx->process_sens_type].configure_func != NULL) {
          err = ctx->sensors_config[ctx->process_sens_type].configure_func(ctx->process_instance, ctx->sensors_config[ctx->process_sens_type].generic.data_ptr +
              ctx->sensors_config[ctx->process_sens_type].generic.data_size * ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->sensors_ctx[ctx->process_sens_type].config_errcode == E_AGAIN ||
              ctx->sensors_ctx[ctx->process_sens_type].config_errcode == E_OK) {
            ctx->sensors_ctx[ctx->process_sens_type].config_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_sensor_cfg = ECU_CONFIG_FSM_SENSOR_CFG_DEFINE;
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

static error_t ecu_config_global_fsm_module_cfg(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_module_cfg) {
      case ECU_CONFIG_FSM_MODULE_CFG_CONDITION:
        if(ctx->global_ready == true && ctx->process_type == ECU_CONFIG_PROCESS_TYPE_MODULE_INIT && ctx->process_result == E_AGAIN) {
          ctx->modules_initialized = false;
          ctx->process_sens_type = 0;
          ctx->process_instance = 0;
          ctx->fsm_module_cfg = ECU_CONFIG_FSM_MODULE_CFG_DEFINE;
          err = E_AGAIN;
          for(int c = 0; c < ctx->modules_count; c++) {
            ctx->modules_ctx[c].reset_errcode = err;
            ctx->modules_ctx[c].config_errcode = err;
          }
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_MODULE_CFG_DEFINE:
        if(ctx->process_sens_type >= ctx->modules_count) {
          ctx->modules_initialized = true;
          ctx->fsm_module_cfg = ECU_CONFIG_FSM_MODULE_CFG_CONDITION;
          err = E_OK;
          ctx->process_result = err;
        } else {
          err = E_AGAIN;
          if(ctx->process_instance >= ctx->modules_config[ctx->process_sens_type].instances_count) {
            ctx->process_instance = 0;
            ctx->process_sens_type++;
          } else {
            ctx->fsm_module_cfg = ECU_CONFIG_FSM_MODULE_CFG_RESET;
          }
          continue;
        }

        break;
      case ECU_CONFIG_FSM_MODULE_CFG_RESET:
        if(ctx->modules_config[ctx->process_sens_type].reset_func != NULL) {
          err = ctx->modules_config[ctx->process_sens_type].reset_func(ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->modules_ctx[ctx->process_sens_type].reset_errcode == E_AGAIN ||
              ctx->modules_ctx[ctx->process_sens_type].reset_errcode == E_OK) {
            ctx->modules_ctx[ctx->process_sens_type].reset_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_module_cfg = ECU_CONFIG_FSM_MODULE_CFG_CONFIG;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_MODULE_CFG_CONFIG:
        if(ctx->modules_config[ctx->process_sens_type].configure_func != NULL) {
          err = ctx->modules_config[ctx->process_sens_type].configure_func(ctx->process_instance, ctx->modules_config[ctx->process_sens_type].generic.data_ptr +
              ctx->modules_config[ctx->process_sens_type].generic.data_size * ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->modules_ctx[ctx->process_sens_type].config_errcode == E_AGAIN ||
              ctx->modules_ctx[ctx->process_sens_type].config_errcode == E_OK) {
            ctx->modules_ctx[ctx->process_sens_type].config_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_module_cfg = ECU_CONFIG_FSM_MODULE_CFG_DEFINE;
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
static error_t ecu_config_global_fsm_timing_cfg(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_timing_cfg) {
      case ECU_CONFIG_FSM_TIMING_CFG_CONDITION:
        if(ctx->global_ready == true && ctx->process_type == ECU_CONFIG_PROCESS_TYPE_TIMING_INIT && ctx->process_result == E_AGAIN) {
          ctx->timings_initialized = false;
          ctx->process_sens_type = 0;
          ctx->process_instance = 0;
          ctx->fsm_timing_cfg = ECU_CONFIG_FSM_TIMING_CFG_DEFINE;
          err = E_AGAIN;
          for(int c = 0; c < ctx->timings_count; c++) {
            ctx->timings_ctx[c].reset_errcode = err;
            ctx->timings_ctx[c].config_errcode = err;
          }
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_TIMING_CFG_DEFINE:
        if(ctx->process_sens_type >= ctx->timings_count) {
          ctx->timings_initialized = true;
          ctx->fsm_timing_cfg = ECU_CONFIG_FSM_TIMING_CFG_CONDITION;
          err = E_OK;
          ctx->process_result = err;
        } else {
          err = E_AGAIN;
          if(ctx->process_instance >= ctx->timings_config[ctx->process_sens_type].instances_count) {
            ctx->process_instance = 0;
            ctx->process_sens_type++;
          } else {
            ctx->fsm_timing_cfg = ECU_CONFIG_FSM_TIMING_CFG_RESET;
          }
          continue;
        }

        break;
      case ECU_CONFIG_FSM_TIMING_CFG_RESET:
        if(ctx->timings_config[ctx->process_sens_type].reset_func != NULL) {
          err = ctx->timings_config[ctx->process_sens_type].reset_func(ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->timings_ctx[ctx->process_sens_type].reset_errcode == E_AGAIN ||
              ctx->timings_ctx[ctx->process_sens_type].reset_errcode == E_OK) {
            ctx->timings_ctx[ctx->process_sens_type].reset_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_timing_cfg = ECU_CONFIG_FSM_TIMING_CFG_CONFIG;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_TIMING_CFG_CONFIG:
        if(ctx->timings_config[ctx->process_sens_type].configure_func != NULL) {
          err = ctx->timings_config[ctx->process_sens_type].configure_func(ctx->process_instance, ctx->timings_config[ctx->process_sens_type].generic.data_ptr +
              ctx->timings_config[ctx->process_sens_type].generic.data_size * ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->timings_ctx[ctx->process_sens_type].config_errcode == E_AGAIN ||
              ctx->timings_ctx[ctx->process_sens_type].config_errcode == E_OK) {
            ctx->timings_ctx[ctx->process_sens_type].config_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_timing_cfg = ECU_CONFIG_FSM_TIMING_CFG_DEFINE;
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

static error_t ecu_config_global_fsm_comms_cfg(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;

  while(true) {
    err = E_AGAIN;

    switch(ctx->fsm_comm_cfg) {
      case ECU_CONFIG_FSM_COMM_CFG_CONDITION:
        if(ctx->global_ready == true && ctx->process_type == ECU_CONFIG_PROCESS_TYPE_COMM_INIT && ctx->process_result == E_AGAIN) {
          ctx->comms_initialized = false;
          ctx->process_sens_type = 0;
          ctx->process_instance = 0;
          ctx->fsm_comm_cfg = ECU_CONFIG_FSM_COMM_CFG_DEFINE;
          err = E_AGAIN;
          for(int c = 0; c < ctx->comms_count; c++) {
            ctx->comms_ctx[c].reset_errcode = err;
            ctx->comms_ctx[c].config_errcode = err;
          }
          continue;
        } else {
          err = E_OK;
        }
        break;
      case ECU_CONFIG_FSM_COMM_CFG_DEFINE:
        if(ctx->process_sens_type >= ctx->comms_count) {
          ctx->comms_initialized = true;
          ctx->fsm_comm_cfg = ECU_CONFIG_FSM_COMM_CFG_CONDITION;
          err = E_OK;
          ctx->process_result = err;
        } else {
          err = E_AGAIN;
          if(ctx->process_instance >= ctx->comms_config[ctx->process_sens_type].instances_count) {
            ctx->process_instance = 0;
            ctx->process_sens_type++;
          } else {
            ctx->fsm_comm_cfg = ECU_CONFIG_FSM_COMM_CFG_RESET;
          }
          continue;
        }

        break;
      case ECU_CONFIG_FSM_COMM_CFG_RESET:
        if(ctx->comms_config[ctx->process_sens_type].reset_func != NULL) {
          err = ctx->comms_config[ctx->process_sens_type].reset_func(ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->comms_ctx[ctx->process_sens_type].reset_errcode == E_AGAIN ||
              ctx->comms_ctx[ctx->process_sens_type].reset_errcode == E_OK) {
            ctx->comms_ctx[ctx->process_sens_type].reset_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_comm_cfg = ECU_CONFIG_FSM_COMM_CFG_CONFIG;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_COMM_CFG_CONFIG:
        if(ctx->comms_config[ctx->process_sens_type].configure_func != NULL) {
          err = ctx->comms_config[ctx->process_sens_type].configure_func(ctx->process_instance, ctx->comms_config[ctx->process_sens_type].generic.data_ptr +
              ctx->comms_config[ctx->process_sens_type].generic.data_size * ctx->process_instance);
        } else {
          err = E_OK;
        }
        if(err != E_AGAIN) {
          if(ctx->comms_ctx[ctx->process_sens_type].config_errcode == E_AGAIN ||
              ctx->comms_ctx[ctx->process_sens_type].config_errcode == E_OK) {
            ctx->comms_ctx[ctx->process_sens_type].config_errcode = err;
          }
          err = E_AGAIN;
          ctx->fsm_comm_cfg = ECU_CONFIG_FSM_COMM_CFG_DEFINE;
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
    case ECU_CONFIG_TYPE_DEVICE:
      ctx->op_index_max = ctx->devices_count;
      break;
    case ECU_CONFIG_TYPE_SENSOR:
      ctx->op_index_max = ctx->sensors_count;
      break;
    case ECU_CONFIG_TYPE_MODULE:
      ctx->op_index_max = ctx->modules_count;
      break;
    case ECU_CONFIG_TYPE_TIMING:
      ctx->op_index_max = ctx->timings_count;
      break;
    case ECU_CONFIG_TYPE_CALIBRATION:
      ctx->op_index_max = ctx->calibrations_count;
      break;
    case ECU_CONFIG_TYPE_RUNTIME:
      ctx->op_index_max = ctx->runtimes_count;
      break;
    case ECU_CONFIG_TYPE_COMM:
      ctx->op_index_max = ctx->comms_count;
      break;
    default:
      ctx->op_index_max = 0;
      break;
  }
}

static void config_global_internal_calculate_instance_max(ecu_config_global_runtime_ctx_t *ctx)
{
  switch(ctx->op_type_index) {
    case ECU_CONFIG_TYPE_DEVICE:
      ctx->op_instance_max = ctx->devices_config[ctx->op_index].instances_count;
      break;
    case ECU_CONFIG_TYPE_SENSOR:
      ctx->op_instance_max = ctx->sensors_config[ctx->op_index].instances_count;
      break;
    case ECU_CONFIG_TYPE_MODULE:
      ctx->op_instance_max = ctx->modules_config[ctx->op_index].instances_count;
      break;
    case ECU_CONFIG_TYPE_TIMING:
      ctx->op_instance_max = ctx->timings_config[ctx->op_index].instances_count;
      break;
    case ECU_CONFIG_TYPE_CALIBRATION:
      ctx->op_instance_max = 1;
      break;
    case ECU_CONFIG_TYPE_RUNTIME:
      ctx->op_instance_max = 1;
      break;
    case ECU_CONFIG_TYPE_COMM:
      ctx->op_instance_max = ctx->comms_config[ctx->op_index].instances_count;
      break;
    default:
      ctx->op_index_max = 0;
      break;
  }
}

static error_t ecu_config_global_fsm_operation(ecu_config_global_runtime_ctx_t *ctx)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  const ecu_config_generic_config_t *req_config;
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
              config_global_internal_calculate_instance_max(ctx);
            } else {
              err = E_OK;
              ctx->op_req_errcode_internal = err;
              ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
            }
          } else {
            switch(ctx->op_type_index) {
              case ECU_CONFIG_TYPE_DEVICE:
                req_config = &ctx->devices_config[ctx->op_index].generic;
                break;
              case ECU_CONFIG_TYPE_SENSOR:
                req_config = &ctx->sensors_config[ctx->op_index].generic;
                break;
              case ECU_CONFIG_TYPE_MODULE:
                req_config = &ctx->modules_config[ctx->op_index].generic;
                break;
              case ECU_CONFIG_TYPE_TIMING:
                req_config = &ctx->timings_config[ctx->op_index].generic;
                break;
              case ECU_CONFIG_TYPE_CALIBRATION:
                req_config = &ctx->calibrations_config[ctx->op_index];
                break;
              case ECU_CONFIG_TYPE_RUNTIME:
                req_config = &ctx->runtimes_config[ctx->op_index];
                break;
              case ECU_CONFIG_TYPE_COMM:
                req_config = &ctx->comms_config[ctx->op_index].generic;
                break;
              default:
                req_config = NULL;
                break;
            }

            if(ctx->op_instance >= ctx->op_instance_max) {
              if(ctx->op_req_index >= ctx->op_index_max || ctx->op_req_type >= ECU_CONFIG_TYPE_ALL) {
                ctx->op_index++;
                ctx->op_instance = 0;
                if(ctx->op_index < ctx->op_index_max) {
                  config_global_internal_calculate_instance_max(ctx);
                }
              } else {
                err = E_OK;
                ctx->op_req_errcode_internal = err;
                ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
              }
            } else {
              if(req_config != NULL) {
                ctx->op_req_ctx.flash_section_type = req_config->flash_section_type;
                ctx->op_req_ctx.get_default_cfg_func = req_config->get_default_cfg_func;
                ctx->op_req_ctx.version_current = req_config->versions_count - 1;
                ctx->op_req_ctx.versions = req_config->versions;
                ctx->op_version = ctx->op_req_ctx.version_current;
                ctx->op_req_ctx.data_ptr = req_config->data_ptr;
                ctx->op_req_ctx.final_data_ptr = req_config->double_data_ptr;
                ctx->op_req_ctx.data_size = req_config->data_size;

                ctx->op_req_ctx.data_ptr += req_config->data_size * ctx->op_instance;
              } else {
                err = E_PARAM;
                ctx->op_req_errcode_internal = err;
                ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
                continue;
              }

              if(ctx->op_request == ECU_CONFIG_OP_WRITE) {
                if(ctx->op_req_ctx.final_data_ptr == NULL) {
                  ctx->op_req_ctx.final_data_ptr = ctx->op_req_ctx.data_ptr;
                  ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_REQUEST;
                } else {
                  ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_DOUBLEBUFFER_REQ;
                  switch(ctx->hdma->Init.MemDataAlignment) {
                    default:
                    case DMA_MDATAALIGN_BYTE:
                      ctx->op_req_ctx.data_dma_datasize = sizeof(uint8_t);
                      break;
                    case DMA_MDATAALIGN_HALFWORD:
                      ctx->op_req_ctx.data_dma_datasize = sizeof(uint16_t);
                      break;
                    case DMA_MDATAALIGN_WORD:
                      ctx->op_req_ctx.data_dma_datasize = sizeof(uint32_t);
                      break;
                  }
                  ctx->op_req_ctx.data_dma_limit = 0x10000 * ctx->op_req_ctx.data_dma_datasize - 0x1000;
                  ctx->op_req_ctx.data_dma_left = req_config->data_size;
                  ctx->op_req_ctx.data_dma_length = ctx->op_req_ctx.data_dma_left;
                  ctx->op_req_ctx.data_dma_pos = 0;
                  if(ctx->op_req_ctx.data_dma_length > ctx->op_req_ctx.data_dma_limit) {
                    ctx->op_req_ctx.data_dma_length = ctx->op_req_ctx.data_dma_limit;
                  }
                }
              } else {
                ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_REQUEST;
              }
            }
          }
        }
        continue;
      case ECU_CONFIG_FSM_OPERATION_DOUBLEBUFFER_REQ:
        ctx->dma_errode = E_AGAIN;
        CacheClean((const void *)ctx->op_req_ctx.data_ptr + ctx->op_req_ctx.data_dma_pos, ctx->op_req_ctx.data_dma_length);
        CacheClean((const void *)ctx->op_req_ctx.final_data_ptr + ctx->op_req_ctx.data_dma_pos, ctx->op_req_ctx.data_dma_length);
        status = HAL_DMA_Start_IT(ctx->hdma, (uint32_t)ctx->op_req_ctx.data_ptr + ctx->op_req_ctx.data_dma_pos,
            (uint32_t)ctx->op_req_ctx.final_data_ptr + ctx->op_req_ctx.data_dma_pos,
            ctx->op_req_ctx.data_dma_length / ctx->op_req_ctx.data_dma_datasize);
        if(status == HAL_OK) {
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_REQUEST;
          continue;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_DOUBLEBUFFER_POLL:
        err = ctx->dma_errode;
        if(err == E_OK) {
          CacheInvalidate((const void *)ctx->op_req_ctx.final_data_ptr + ctx->op_req_ctx.data_dma_pos, ctx->op_req_ctx.data_dma_length);

          ctx->op_req_ctx.data_dma_left -= ctx->op_req_ctx.data_dma_length;
          ctx->op_req_ctx.data_dma_pos += ctx->op_req_ctx.data_dma_length;
          ctx->op_req_ctx.data_dma_length = ctx->op_req_ctx.data_dma_left;
          if(ctx->op_req_ctx.data_dma_length > ctx->op_req_ctx.data_dma_limit) {
            ctx->op_req_ctx.data_dma_length = ctx->op_req_ctx.data_dma_limit;
          } else if(ctx->op_req_ctx.data_dma_length == 0) {
            ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_REQUEST;
          } else {
            ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_DOUBLEBUFFER_REQ;
          }
          continue;
        } else if(err != E_AGAIN) {
          ctx->op_req_errcode_internal = err;
          ctx->fsm_operation = ECU_CONFIG_FSM_OPERATION_UNLOCK;
        }
        break;
      case ECU_CONFIG_FSM_OPERATION_REQUEST:
        if(ctx->op_request == ECU_CONFIG_OP_READ) {
          err = flash_section_read(ctx->op_req_ctx.flash_section_type, ctx->op_instance, &ctx->op_version, ctx->op_req_ctx.data_ptr, ctx->op_req_ctx.data_size);
        } else if(ctx->op_request == ECU_CONFIG_OP_WRITE) {
          err = flash_section_write(ctx->op_req_ctx.flash_section_type, ctx->op_instance, ctx->op_version, ctx->op_req_ctx.final_data_ptr, ctx->op_req_ctx.data_size);
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
      case ECU_CONFIG_FSM_PROCESS_FLASH_ERASE:
        err = ecu_config_global_fsm_flash_erase(ctx);
        break;
      case ECU_CONFIG_FSM_PROCESS_DEVICE_CFG:
        err = ecu_config_global_fsm_device_cfg(ctx);
        break;
      case ECU_CONFIG_FSM_PROCESS_SENSOR_CFG:
        err = ecu_config_global_fsm_sensor_cfg(ctx);
        break;
      case ECU_CONFIG_FSM_PROCESS_MODULE_CFG:
        err = ecu_config_global_fsm_module_cfg(ctx);
        break;
      case ECU_CONFIG_FSM_PROCESS_TIMING_CFG:
        err = ecu_config_global_fsm_timing_cfg(ctx);
        break;
      case ECU_CONFIG_FSM_PROCESS_COMM_CFG:
        err = ecu_config_global_fsm_comms_cfg(ctx);
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
