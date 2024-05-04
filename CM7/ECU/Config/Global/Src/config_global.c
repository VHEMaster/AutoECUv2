/*
 * config_global.c
 *
 *  Created on: May 1, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "compiler.h"
#include "flash.h"
#include "config_global.h"
#include "config_global_fsm.h"

#include "versioned_hw.h"
#include "versioned_sw.h"
#include "config_engine.h"

static ecu_config_global_engine_t ecu_config_global_engine = {0};

static ecu_config_global_runtime_ctx_t ecu_config_global_runtime_ctx = {0};

static ecu_config_component_ctx_t ecu_config_global_component_ctx[ECU_CONFIG_COMP_TYPE_MAX] = {
      {
          .device_type = ECU_DEVICE_TYPE_FLASH,
          .instances_count = ECU_DEVICE_FLASH_MAX,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_flash_reset,
      }, //ECU_CONFIG_COMP_TYPE_FLASH
      {
          .device_type = ECU_DEVICE_TYPE_FLEXIO,
          .instances_count = ECU_DEVICE_FLEXIO_MAX,
          .flash_section_type = FLASH_SECTION_TYPE_HW_FLEXIO,
          .get_default_cfg_func = (ecu_config_get_default_comp_cfg_func_t)ecu_devices_flexio_get_default_config,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_flexio_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_flexio_reset,
          .data_ptr = &ecu_config_global_engine.hw.flexio[0],
          .data_size = sizeof(ecu_config_global_engine.hw.flexio[0]),
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
          .flash_section_type = FLASH_SECTION_TYPE_HW_MOTOR,
          .get_default_cfg_func = (ecu_config_get_default_comp_cfg_func_t)ecu_devices_motor_get_default_config,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_motor_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_motor_reset,
          .data_ptr = &ecu_config_global_engine.hw.motor[0],
          .data_size = sizeof(ecu_config_global_engine.hw.motor[0]),
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
          .flash_section_type = FLASH_SECTION_TYPE_HW_STEPPER,
          .get_default_cfg_func = (ecu_config_get_default_comp_cfg_func_t)ecu_devices_stepper_get_default_config,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_stepper_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_stepper_reset,
          .data_ptr = &ecu_config_global_engine.hw.stepper[0],
          .data_size = sizeof(ecu_config_global_engine.hw.stepper[0]),
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
          .flash_section_type = FLASH_SECTION_TYPE_HW_WBLS,
          .data_ptr = &ecu_config_global_engine.hw.wbls[0],
          .data_size = sizeof(ecu_config_global_engine.hw.wbls[0]),
          .get_default_cfg_func = (ecu_config_get_default_comp_cfg_func_t)ecu_devices_wbls_get_default_config,
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
    {
        .flash_section_type = FLASH_SECTION_TYPE_ID,
        .get_default_cfg_func = NULL,
        .data_ptr = &ecu_config_global_engine.sw.id,
        .data_size = sizeof(ecu_config_global_engine.sw.id),
        .versions_count = ECU_CONFIG_DATA_ID_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_DATA_ID_VERSION_V1,
                .size = sizeof(ecu_config_data_identification_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_ID
};

error_t ecu_config_global_init(void)
{
  error_t err = E_OK;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    memset(&ecu_config_global_runtime_ctx, 0u, sizeof(ecu_config_global_runtime_ctx));
    memset(&ecu_config_global_engine, 0u, sizeof(ecu_config_global_engine));

    err = flash_init();
    BREAK_IF(err != E_OK);

    ctx->components_count = ECU_CONFIG_COMP_TYPE_MAX;
    ctx->components = ecu_config_global_component_ctx;

    ctx->components_count = ECU_CONFIG_CALIB_TYPE_MAX;
    ctx->calibrations = ecu_config_global_calibration_ctx;

    for(int c = 0; c < ECU_CONFIG_COMP_TYPE_MAX; c++) {
      for(int i = 0; i < ctx->components[c].instances_count; i++) {
        if(ctx->components[c].get_default_cfg_func != NULL) {
          err = ctx->components[c].get_default_cfg_func(i, ctx->components[c].data_ptr + ctx->components[c].data_size * i);
        }
        BREAK_IF(err != E_OK);
      }
    }
    BREAK_IF(err != E_OK);

    for(int c = 0; c < ECU_CONFIG_CALIB_TYPE_MAX; c++) {
      if(ctx->calibrations[c].get_default_cfg_func != NULL) {
        err = ctx->calibrations[c].get_default_cfg_func(ctx->calibrations[c].data_ptr);
      }
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    ctx->components_ready = true;
  } while(0);

  return err;
}

void ecu_config_global_loop_main(void)
{

}

void ecu_config_global_loop_slow(void)
{

}

void ecu_config_global_loop_fast(void)
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

error_t ecu_config_global_components_reset(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->components_ready == false, err = E_NOTRDY);

    if(ctx->process_trigger_type == ECU_CONFIG_RST_CFG_NONE) {
      ctx->process_trigger_type = ECU_CONFIG_RST_CFG_RESET;
      ctx->process_result = E_AGAIN;
    }
    if(ctx->process_trigger_type == ECU_CONFIG_RST_CFG_RESET) {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_trigger_type = ECU_CONFIG_RST_CFG_NONE;
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

    if(ctx->process_trigger_type == ECU_CONFIG_RST_CFG_NONE) {
      ctx->process_trigger_type = ECU_CONFIG_RST_CFG_CONFIGURE;
      ctx->process_result = E_AGAIN;
    }
    if(ctx->process_trigger_type == ECU_CONFIG_RST_CFG_CONFIGURE) {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_trigger_type = ECU_CONFIG_RST_CFG_NONE;
      }
    }
  } while(0);

  return err;
}

