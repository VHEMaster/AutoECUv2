/*
 * config_global.c
 *
 *  Created on: May 1, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "compiler.h"
#include "config_global.h"
#include "config_global_fsm.h"

static ecu_config_global_runtime_ctx_t ecu_config_global_runtime_ctx = {0};

static ecu_config_component_ctx_t ecu_config_global_component_ctx[ECU_CONFIG_COMP_TYPE_MAX] = {
      {
          .device_type = ECU_DEVICE_TYPE_FLEXIO,
          .instances_count = ECU_DEVICE_FLEXIO_MAX,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_flexio_get_default_config,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_flexio_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_flexio_reset,
          .versions_count = FLEXIO_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = FLEXIO_CONFIG_VERSION_V1,
                  .size = sizeof(l9966_config_data_v1_t),
                  .translate_func = NULL,
              }
          },
      }, //ECU_CONFIG_COMP_TYPE_FLEXIO
      {
          .device_type = ECU_DEVICE_TYPE_MOTOR,
          .instances_count = ECU_DEVICE_MOTOR_MAX,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_motor_get_default_config,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_motor_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_motor_reset,
          .versions_count = MOTOR_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = MOTOR_CONFIG_VERSION_V1,
                  .size = sizeof(l9960_config_v1_t),
                  .translate_func = NULL,
              }
          },
      }, //ECU_CONFIG_COMP_TYPE_MOTOR
      {
          .device_type = ECU_DEVICE_TYPE_STEPPER,
          .instances_count = ECU_DEVICE_STEPPER_MAX,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_stepper_get_default_config,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_stepper_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_stepper_reset,
          .versions_count = STEPPER_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = STEPPER_CONFIG_VERSION_V1,
                  .size = sizeof(tle4729_config_v1_t),
                  .translate_func = NULL,
              }
          },
      }, //ECU_CONFIG_COMP_TYPE_STEPPER
      {
          .device_type = ECU_DEVICE_TYPE_WBLS,
          .instances_count = ECU_DEVICE_WBLS_MAX,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_wbls_get_default_config,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_wbls_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_wbls_reset,
          .versions_count = WBLS_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = WBLS_CONFIG_VERSION_V1,
                  .size = sizeof(cj125_config_v1_t),
                  .translate_func = NULL,
              }
          },
      }, //ECU_CONFIG_COMP_TYPE_WBLS
};

static ecu_config_calibration_ctx_t ecu_config_global_calibration_ctx[ECU_CONFIG_CALIB_TYPE_MAX] = {

};

error_t ecu_config_global_init(void)
{
  error_t err = E_OK;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  memset(&ecu_config_global_runtime_ctx, 0u, sizeof(ecu_config_global_runtime_ctx));

  ctx->components_count = ECU_CONFIG_COMP_TYPE_MAX;
  ctx->components = ecu_config_global_component_ctx;

  ctx->components_count = ECU_CONFIG_CALIB_TYPE_MAX;
  ctx->calibrations = ecu_config_global_calibration_ctx;

  ctx->components_ready = true;

  return err;
}

void ecu_config_global_loop_main(void)
{

}

void ecu_config_global_loop_slow(void)
{
  error_t err = E_OK;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  if(ctx->components_ready) {
    err = ecu_config_global_fsm(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set error in future
    }
  }
}

void ecu_config_global_loop_fast(void)
{

}

error_t ecu_config_global_components_reset(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->components_ready == false, err = E_NOTRDY);

    if(ctx->process_trigger_type == ECU_CONFIG_PROC_TRIG_TYPE_NONE) {
      ctx->components_initialized = false;
      ctx->process_comp_type = 0;
      ctx->process_instance = 0;
      ctx->process_result = E_AGAIN;
      ctx->process_trigger_type = ECU_CONFIG_PROC_TRIG_TYPE_RESET;
    }
    if(ctx->process_trigger_type == ECU_CONFIG_PROC_TRIG_TYPE_RESET) {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_trigger_type = ECU_CONFIG_PROC_TRIG_TYPE_NONE;
      }
    }
  } while(0);

  return err;
}

error_t ecu_config_global_components_configure(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->components_ready == false, err = E_NOTRDY);

    if(ctx->process_trigger_type == ECU_CONFIG_PROC_TRIG_TYPE_NONE) {
      ctx->components_configured = false;
      ctx->process_comp_type = 0;
      ctx->process_instance = 0;
      ctx->process_result = E_AGAIN;
      ctx->process_trigger_type = ECU_CONFIG_PROC_TRIG_TYPE_CONFIGURE;
    }
    if(ctx->process_trigger_type == ECU_CONFIG_PROC_TRIG_TYPE_CONFIGURE) {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_trigger_type = ECU_CONFIG_PROC_TRIG_TYPE_NONE;
      }
    }
  } while(0);

  return err;
}

