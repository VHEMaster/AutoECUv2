/*
 * config_global.c
 *
 *  Created on: May 1, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "compiler.h"
#include "crc.h"
#include "flash.h"
#include "flash_memory_layout.h"
#include "config_global.h"
#include "config_global_fsm.h"

#include "versioned_hw.h"
#include "versioned_sw.h"
#include "config_engine.h"

static ALIGNED_CACHE BUFFER_DMA ecu_config_global_engine_t ecu_config_global_engine = {0};

static ecu_config_global_runtime_ctx_t ecu_config_global_runtime_ctx = {0};

static ecu_config_component_ctx_t ecu_config_global_flash_ctx = {
    .device_type = ECU_DEVICE_TYPE_FLASH,
    .instances_count = ECU_DEVICE_FLASH_MAX,
    .reset_func = (ecu_config_reset_func_t)ecu_devices_flash_reset,
};

static ecu_config_component_ctx_t ecu_config_global_component_ctx[ECU_CONFIG_COMP_TYPE_ALL] = {
      {
          .device_type = ECU_DEVICE_TYPE_FLEXIO,
          .instances_count = ECU_DEVICE_FLEXIO_MAX,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_flexio_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_flexio_reset,
          .generic = {
              .flash_section_type = FLASH_SECTION_TYPE_HW_FLEXIO,
              .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_flexio_get_default_config,
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
          },
      }, //ECU_CONFIG_COMP_TYPE_FLEXIO
      {
          .device_type = ECU_DEVICE_TYPE_MOTOR,
          .instances_count = ECU_DEVICE_MOTOR_MAX,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_motor_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_motor_reset,
          .generic = {
              .flash_section_type = FLASH_SECTION_TYPE_HW_MOTOR,
              .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_motor_get_default_config,
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
          },
      }, //ECU_CONFIG_COMP_TYPE_MOTOR
      {
          .device_type = ECU_DEVICE_TYPE_STEPPER,
          .instances_count = ECU_DEVICE_STEPPER_MAX,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_stepper_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_stepper_reset,
          .generic = {
              .flash_section_type = FLASH_SECTION_TYPE_HW_STEPPER,
              .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_stepper_get_default_config,
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
          },
      }, //ECU_CONFIG_COMP_TYPE_STEPPER
      {
          .device_type = ECU_DEVICE_TYPE_WBLS,
          .instances_count = ECU_DEVICE_WBLS_MAX,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_wbls_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_wbls_reset,
          .generic = {
              .flash_section_type = FLASH_SECTION_TYPE_HW_WBLS,
              .data_ptr = &ecu_config_global_engine.hw.wbls[0],
              .data_size = sizeof(ecu_config_global_engine.hw.wbls[0]),
              .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_wbls_get_default_config,
              .versions_count = WBLS_CONFIG_VERSION_MAX,
              .versions = {
                  {
                      .version = WBLS_CONFIG_VERSION_V1,
                      .size = sizeof(cj125_config_v1_t),
                      .translate_func = NULL,
                  }
              },
          },
      }, //ECU_CONFIG_COMP_TYPE_WBLS
};

static ecu_config_component_ctx_t ecu_config_global_sensor_ctx[ECU_CONFIG_SENS_TYPE_ALL] = {
    {
        .device_type = ECU_SENSOR_TYPE_CKP,
        .instances_count = ECU_SENSOR_CKP_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_ckp_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_ckp_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_CKP,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_ckp_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.ckp[0],
            .data_size = sizeof(ecu_config_global_engine.sens.ckp[0]),
            .versions_count = CKP_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = CKP_CONFIG_VERSION_V1,
                    .size = sizeof(ckp_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_CKP
    {
        .device_type = ECU_SENSOR_TYPE_CMP,
        .instances_count = ECU_SENSOR_CMP_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_cmp_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_cmp_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_CMP,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_cmp_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.cmp[0],
            .data_size = sizeof(ecu_config_global_engine.sens.cmp[0]),
            .versions_count = CMP_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = CMP_CONFIG_VERSION_V1,
                    .size = sizeof(cmp_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_CMP
    {
        .device_type = ECU_SENSOR_TYPE_ECT,
        .instances_count = ECU_SENSOR_ECT_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_ect_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_ect_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_ECT,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_ect_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.ect[0],
            .data_size = sizeof(ecu_config_global_engine.sens.ect[0]),
            .versions_count = ECT_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = ECT_CONFIG_VERSION_V1,
                    .size = sizeof(ect_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_ECT
    {
        .device_type = ECU_SENSOR_TYPE_EGT,
        .instances_count = ECU_SENSOR_EGT_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_egt_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_egt_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_EGT,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_egt_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.egt[0],
            .data_size = sizeof(ecu_config_global_engine.sens.egt[0]),
            .versions_count = EGT_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = EGT_CONFIG_VERSION_V1,
                    .size = sizeof(egt_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_EGT
    {
        .device_type = ECU_SENSOR_TYPE_IAT,
        .instances_count = ECU_SENSOR_IAT_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_iat_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_iat_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_IAT,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_iat_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.iat[0],
            .data_size = sizeof(ecu_config_global_engine.sens.iat[0]),
            .versions_count = IAT_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = IAT_CONFIG_VERSION_V1,
                    .size = sizeof(iat_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_IAT
    {
        .device_type = ECU_SENSOR_TYPE_MAF,
        .instances_count = ECU_SENSOR_MAF_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_maf_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_maf_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_MAF,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_maf_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.maf[0],
            .data_size = sizeof(ecu_config_global_engine.sens.maf[0]),
            .versions_count = MAF_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = MAF_CONFIG_VERSION_V1,
                    .size = sizeof(maf_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_MAF
    {
        .device_type = ECU_SENSOR_TYPE_MAP,
        .instances_count = ECU_SENSOR_MAP_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_map_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_map_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_MAP,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_map_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.map[0],
            .data_size = sizeof(ecu_config_global_engine.sens.map[0]),
            .versions_count = MAP_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = MAP_CONFIG_VERSION_V1,
                    .size = sizeof(map_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_MAP
    {
        .device_type = ECU_SENSOR_TYPE_TPS,
        .instances_count = ECU_SENSOR_TPS_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_tps_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_tps_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_TPS,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_tps_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.tps[0],
            .data_size = sizeof(ecu_config_global_engine.sens.tps[0]),
            .versions_count = TPS_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = TPS_CONFIG_VERSION_V1,
                    .size = sizeof(tps_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_TPS
    {
        .device_type = ECU_SENSOR_TYPE_APS,
        .instances_count = ECU_SENSOR_APS_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_aps_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_aps_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_APS,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_aps_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.aps[0],
            .data_size = sizeof(ecu_config_global_engine.sens.aps[0]),
            .versions_count = APS_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = APS_CONFIG_VERSION_V1,
                    .size = sizeof(aps_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_APS
};

static ecu_config_generic_ctx_t ecu_config_global_calibration_ctx[ECU_CONFIG_CALIB_TYPE_ALL] = {
    {
        .flash_section_type = FLASH_SECTION_TYPE_ID,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)NULL,
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

static ecu_config_generic_ctx_t ecu_config_global_runtimes_ctx[ECU_CONFIG_RUNTIME_TYPE_ALL] = {

};

error_t ecu_config_global_init(void)
{
  error_t err = E_OK;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;
  const flash_mem_layout_section_info_t *section_info;

  do {
    memset(&ecu_config_global_runtime_ctx, 0u, sizeof(ecu_config_global_runtime_ctx));
    memset(&ecu_config_global_engine, 0u, sizeof(ecu_config_global_engine));

    err = crc_init();
    BREAK_IF(err != E_OK);

    err = flash_init();
    BREAK_IF(err != E_OK);

    ctx->flash_ctx = &ecu_config_global_flash_ctx;

    ctx->components_count = ITEMSOF(ecu_config_global_component_ctx);
    ctx->components = ecu_config_global_component_ctx;

    ctx->sensors_count = ITEMSOF(ecu_config_global_sensor_ctx);
    ctx->sensors = ecu_config_global_sensor_ctx;

    ctx->calibrations_count = ITEMSOF(ecu_config_global_calibration_ctx);
    ctx->calibrations = ecu_config_global_calibration_ctx;

    ctx->runtimes_count = ITEMSOF(ecu_config_global_runtimes_ctx);
    ctx->runtimes = ecu_config_global_runtimes_ctx;

    for(int c = 0; c < ctx->components_count; c++) {
      for(int i = 0; i < ctx->components[c].instances_count; i++) {
        err = flash_mem_layout_get_section_info(&section_info, ctx->components[c].generic.flash_section_type, i);
        BREAK_IF(err != E_OK);
        BREAK_IF_ACTION(ctx->components[c].generic.versions_count == 0, err = E_INVALACT);
        BREAK_IF_ACTION((ctx->components[c].generic.data_size & 0x1F) != 0, err = E_INVALACT);
        BREAK_IF_ACTION(ctx->components[c].generic.data_size > section_info->section_length - ECU_FLASH_SECTION_HEADER_LENGTH, err = E_INVALACT);

        if(ctx->components[c].generic.get_default_cfg_func != NULL) {
          err = ctx->components[c].generic.get_default_cfg_func(i, ctx->components[c].generic.data_ptr + ctx->components[c].generic.data_size * i);
        }
        BREAK_IF(err != E_OK);
      }
    }
    BREAK_IF(err != E_OK);

    for(int c = 0; c < ctx->sensors_count; c++) {
      for(int i = 0; i < ctx->sensors[c].instances_count; i++) {
        err = flash_mem_layout_get_section_info(&section_info, ctx->sensors[c].generic.flash_section_type, i);
        BREAK_IF(err != E_OK);
        BREAK_IF_ACTION(ctx->sensors[c].generic.versions_count == 0, err = E_INVALACT);
        BREAK_IF_ACTION((ctx->sensors[c].generic.data_size & 0x1F) != 0, err = E_INVALACT);
        BREAK_IF_ACTION(ctx->sensors[c].generic.data_size > section_info->section_length - ECU_FLASH_SECTION_HEADER_LENGTH, err = E_INVALACT);

        if(ctx->sensors[c].generic.get_default_cfg_func != NULL) {
          err = ctx->sensors[c].generic.get_default_cfg_func(i, ctx->sensors[c].generic.data_ptr + ctx->sensors[c].generic.data_size * i);
        }
        BREAK_IF(err != E_OK);
      }
    }
    BREAK_IF(err != E_OK);


    for(int c = 0; c < ctx->calibrations_count; c++) {
      err = flash_mem_layout_get_section_info(&section_info, ctx->calibrations[c].flash_section_type, 0);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(ctx->calibrations[c].versions_count == 0, err = E_INVALACT);
      BREAK_IF_ACTION((ctx->calibrations[c].data_size & 0x1F) != 0, err = E_INVALACT);
      BREAK_IF_ACTION(ctx->calibrations[c].data_size > section_info->section_length - ECU_FLASH_SECTION_HEADER_LENGTH, err = E_INVALACT);

      if(ctx->calibrations[c].get_default_cfg_func != NULL) {
        err = ctx->calibrations[c].get_default_cfg_func(0, ctx->calibrations[c].data_ptr);
      }
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    for(int c = 0; c < ctx->runtimes_count; c++) {
      err = flash_mem_layout_get_section_info(&section_info, ctx->runtimes[c].flash_section_type, 0);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(ctx->runtimes[c].versions_count == 0, err = E_INVALACT);
      BREAK_IF_ACTION((ctx->runtimes[c].data_size & 0x1F) != 0, err = E_INVALACT);
      BREAK_IF_ACTION(ctx->runtimes[c].data_size > section_info->section_length - ECU_FLASH_SECTION_HEADER_LENGTH, err = E_INVALACT);

      if(ctx->runtimes[c].get_default_cfg_func != NULL) {
        err = ctx->runtimes[c].get_default_cfg_func(0, ctx->runtimes[c].data_ptr);
      }
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);

    ctx->op_req_type = ECU_CONFIG_TYPE_MAX;

    ctx->global_ready = true;
  } while(0);

  return err;
}

void ecu_config_global_loop_main(void)
{

}

void ecu_config_global_loop_slow(void)
{

}

ITCM_FUNC void ecu_config_global_loop_fast(void)
{
  error_t err = E_OK;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  if(ctx->global_ready) {
    err = ecu_config_global_fsm(ctx);
    if(err != E_OK && err != E_AGAIN) {
      //TODO: set error in future
    }
  }

  if(ctx->flash_initialized) {
    flash_loop_fast();
  }
}

error_t ecu_config_global_flash_initialize(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->global_ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->process_comps_init != false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_sens_init != false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);

    if(ctx->process_flash_init == false) {
      ctx->process_flash_init = true;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_flash_init = false;
      }
    }
  } while(0);

  return err;
}

error_t ecu_config_global_components_initialize(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->global_ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->process_flash_init != false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_sens_init != false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);

    if(ctx->process_comps_init == false) {
      ctx->process_comps_init = true;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_comps_init = false;
      }
    }
  } while(0);

  return err;
}

error_t ecu_config_global_sensors_initialize(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->global_ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->process_flash_init != false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_comps_init != false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);

    if(ctx->process_sens_init == false) {
      ctx->process_sens_init = true;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_sens_init = false;
      }
    }
  } while(0);

  return err;
}

error_t ecu_config_global_operation(ecu_config_op_t op, ecu_config_type_t type, ecu_index_type_t index, ecu_instance_t instance)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(op >= ECU_CONFIG_OP_MAX || op <= ECU_CONFIG_OP_NONE, err = E_PARAM);
    BREAK_IF_ACTION(type >= ECU_CONFIG_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(ctx->global_ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->process_flash_init != false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_sens_init != false, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_comps_init != false, err = E_INVALACT);

    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE && ctx->op_request != op, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE && (ctx->op_req_type != type || ctx->op_req_index != index || ctx->op_req_instance != instance), err = E_INVALACT);

    if(ctx->op_request == ECU_CONFIG_OP_NONE) {
      ctx->op_req_type = type;
      ctx->op_req_index = index;
      ctx->op_req_instance = instance;
      ctx->op_request = op;
      ctx->op_req_errcode = E_AGAIN;
    } else {
      if(ctx->op_req_errcode != E_AGAIN) {
        err = ctx->op_req_errcode;
        ctx->op_request = ECU_CONFIG_OP_NONE;
      }
    }
  } while(0);

  return err;
}
