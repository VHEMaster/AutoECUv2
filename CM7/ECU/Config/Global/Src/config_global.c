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
#include "config_extern.h"
#include "config_global.h"
#include "config_global_fsm.h"

#include "versioned_devices.h"
#include "versioned_modules.h"
#include "versioned_comm.h"
#include "versioned_calibration.h"
#include "versioned_runtime.h"
#include "config_engine.h"

extern ecu_config_global_engine_t ecu_config_global_engine;
extern ecu_config_global_engine_double_t ecu_config_global_engine_double;

static ecu_config_global_runtime_ctx_t ecu_config_global_runtime_ctx = {0};

static ecu_config_device_ctx_t ecu_config_global_flash_ctx = {
    .device_type = ECU_DEVICE_TYPE_FLASH,
    .instances_count = ECU_DEVICE_FLASH_MAX,
    .reset_func = (ecu_config_reset_func_t)ecu_devices_flash_reset,
};

static ecu_config_device_ctx_t ecu_config_global_device_ctx[ECU_CONFIG_DEV_TYPE_ALL] = {
      {
          .device_type = ECU_DEVICE_TYPE_FLEXIO,
          .instances_count = ECU_DEVICE_FLEXIO_MAX,
          .configure_func = (ecu_config_configure_func_t)ecu_devices_flexio_configure,
          .reset_func = (ecu_config_reset_func_t)ecu_devices_flexio_reset,
          .generic = {
              .flash_section_type = FLASH_SECTION_TYPE_HW_FLEXIO,
              .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_devices_flexio_get_default_config,
              .data_ptr = &ecu_config_global_engine.devs.flexio[0],
              .data_size = sizeof(ecu_config_global_engine.devs.flexio[0]),
              .versions_count = FLEXIO_CONFIG_VERSION_MAX,
              .versions = {
                  {
                      .version = FLEXIO_CONFIG_VERSION_V1,
                      .size = sizeof(l9966_config_v1_t),
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
              .data_ptr = &ecu_config_global_engine.devs.motor[0],
              .data_size = sizeof(ecu_config_global_engine.devs.motor[0]),
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
              .data_ptr = &ecu_config_global_engine.devs.stepper[0],
              .data_size = sizeof(ecu_config_global_engine.devs.stepper[0]),
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
              .data_ptr = &ecu_config_global_engine.devs.wbls[0],
              .data_size = sizeof(ecu_config_global_engine.devs.wbls[0]),
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

static ecu_config_device_ctx_t ecu_config_global_sensor_ctx[ECU_CONFIG_SENS_TYPE_ALL] = {
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
        .device_type = ECU_SENSOR_TYPE_VSS,
        .instances_count = ECU_SENSOR_VSS_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_vss_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_vss_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_VSS,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_vss_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.vss[0],
            .data_size = sizeof(ecu_config_global_engine.sens.vss[0]),
            .versions_count = VSS_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = VSS_CONFIG_VERSION_V1,
                    .size = sizeof(vss_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_VSS
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
    {
        .device_type = ECU_SENSOR_TYPE_OTS,
        .instances_count = ECU_SENSOR_OTS_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_ots_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_ots_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_OTS,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_ots_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.ots[0],
            .data_size = sizeof(ecu_config_global_engine.sens.ots[0]),
            .versions_count = OTS_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = OTS_CONFIG_VERSION_V1,
                    .size = sizeof(ots_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_OTS
    {
        .device_type = ECU_SENSOR_TYPE_OPS,
        .instances_count = ECU_SENSOR_OPS_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_sensors_ops_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_sensors_ops_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_SENS_OPS,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_sensors_ops_get_default_config,
            .data_ptr = &ecu_config_global_engine.sens.ops[0],
            .data_size = sizeof(ecu_config_global_engine.sens.ops[0]),
            .versions_count = OPS_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = OPS_CONFIG_VERSION_V1,
                    .size = sizeof(ops_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_SENS_TYPE_OPS
};

static ecu_config_device_ctx_t ecu_config_global_module_ctx[ECU_CONFIG_MODULE_TYPE_ALL] = {
    {
        .device_type = ECU_MODULE_TYPE_TIMING,
        .instances_count = ECU_MODULE_TIMING_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_modules_timing_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_modules_timing_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_MODULE_TIMING,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_modules_timing_get_default_config,
            .data_ptr = &ecu_config_global_engine.modules.timing[0],
            .data_size = sizeof(ecu_config_global_engine.modules.timing[0]),
            .versions_count = TIMING_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = TIMING_CONFIG_VERSION_V1,
                    .size = sizeof(timing_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_MODULE_TYPE_TIMING
    {
        .device_type = ECU_MODULE_TYPE_ETC,
        .instances_count = ECU_MODULE_ETC_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_modules_etc_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_modules_etc_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_MODULE_ETC,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_modules_etc_get_default_config,
            .data_ptr = &ecu_config_global_engine.modules.etc[0],
            .data_size = sizeof(ecu_config_global_engine.modules.etc[0]),
            .versions_count = ETC_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = ETC_CONFIG_VERSION_V1,
                    .size = sizeof(etc_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_MODULE_TYPE_ETC
    {
        .device_type = ECU_MODULE_TYPE_VVT,
        .instances_count = ECU_MODULE_VVT_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_modules_vvt_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_modules_vvt_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_MODULE_VVT,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_modules_vvt_get_default_config,
            .data_ptr = &ecu_config_global_engine.modules.vvt[0],
            .data_size = sizeof(ecu_config_global_engine.modules.vvt[0]),
            .versions_count = VVT_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = VVT_CONFIG_VERSION_V1,
                    .size = sizeof(vvt_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_MODULE_TYPE_VVT
    {
        .device_type = ECU_MODULE_TYPE_FUELPUMP,
        .instances_count = ECU_MODULE_FUELPUMP_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_modules_fuelpump_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_modules_fuelpump_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_MODULE_FUELPUMP,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_modules_fuelpump_get_default_config,
            .data_ptr = &ecu_config_global_engine.modules.fuelpump[0],
            .data_size = sizeof(ecu_config_global_engine.modules.fuelpump[0]),
            .versions_count = FUELPUMP_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = FUELPUMP_CONFIG_VERSION_V1,
                    .size = sizeof(fuelpump_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_MODULE_TYPE_FUELPUMP
    {
        .device_type = ECU_MODULE_TYPE_COOLINGFAN,
        .instances_count = ECU_MODULE_COOLINGFAN_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_modules_coolingfan_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_modules_coolingfan_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_MODULE_COOLINGFAN,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_modules_coolingfan_get_default_config,
            .data_ptr = &ecu_config_global_engine.modules.coolingfan[0],
            .data_size = sizeof(ecu_config_global_engine.modules.coolingfan[0]),
            .versions_count = COOLINGFAN_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = COOLINGFAN_CONFIG_VERSION_V1,
                    .size = sizeof(coolingfan_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_MODULE_TYPE_COOLINGFAN
    {
        .device_type = ECU_MODULE_TYPE_IGNPOWER,
        .instances_count = ECU_MODULE_IGNPOWER_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_modules_ignpower_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_modules_ignpower_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_MODULE_IGNPOWER,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_modules_ignpower_get_default_config,
            .data_ptr = &ecu_config_global_engine.modules.ignpower[0],
            .data_size = sizeof(ecu_config_global_engine.modules.ignpower[0]),
            .versions_count = IGNPOWER_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = IGNPOWER_CONFIG_VERSION_V1,
                    .size = sizeof(ignpower_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_MODULE_TYPE_IGNPOWER
    {
        .device_type = ECU_MODULE_TYPE_INDICATION,
        .instances_count = ECU_MODULE_INDICATION_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_modules_indication_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_modules_indication_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_MODULE_INDICATION,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_modules_indication_get_default_config,
            .data_ptr = &ecu_config_global_engine.modules.indication[0],
            .data_size = sizeof(ecu_config_global_engine.modules.indication[0]),
            .versions_count = INDICATION_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = INDICATION_CONFIG_VERSION_V1,
                    .size = sizeof(indication_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_MODULE_TYPE_INDICATION
    {
        .device_type = ECU_MODULE_TYPE_WGCV,
        .instances_count = ECU_MODULE_WGCV_MAX,
        .configure_func = (ecu_config_configure_func_t)ecu_modules_wgcv_configure,
        .reset_func = (ecu_config_reset_func_t)ecu_modules_wgcv_reset,
        .generic = {
            .flash_section_type = FLASH_SECTION_TYPE_MODULE_WGCV,
            .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_modules_wgcv_get_default_config,
            .data_ptr = &ecu_config_global_engine.modules.wgcv[0],
            .data_size = sizeof(ecu_config_global_engine.modules.wgcv[0]),
            .versions_count = WGCV_CONFIG_VERSION_MAX,
            .versions = {
                {
                    .version = WGCV_CONFIG_VERSION_V1,
                    .size = sizeof(wgcv_config_v1_t),
                    .translate_func = NULL,
                }
            },
        },
    }, //ECU_CONFIG_MODULE_TYPE_WGCV
};

static ecu_config_generic_ctx_t ecu_config_global_calibration_ctx[ECU_CONFIG_CALIB_TYPE_ALL] = {
    {
        .flash_section_type = FLASH_SECTION_TYPE_ID,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_calibration_id_get_default_config,
        .data_ptr = &ecu_config_global_engine.calibration.id,
        .data_size = sizeof(ecu_config_global_engine.calibration.id),
        .versions_count = ECU_CONFIG_DATA_ID_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_DATA_ID_VERSION_V1,
                .size = sizeof(ecu_config_data_identification_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_ID
    {
        .flash_section_type = FLASH_SECTION_TYPE_CALIBRATION_IO,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_calibration_io_get_default_config,
        .data_ptr = &ecu_config_global_engine.calibration.io,
        .data_size = sizeof(ecu_config_global_engine.calibration.io),
        .versions_count = ECU_CONFIG_IO_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_IO_VERSION_V1,
                .size = sizeof(ecu_config_io_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_IO
    {
        .flash_section_type = FLASH_SECTION_TYPE_CALIBRATION_POWERMODING,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_calibration_powermoding_get_default_config,
        .data_ptr = &ecu_config_global_engine.calibration.powermoding,
        .data_size = sizeof(ecu_config_global_engine.calibration.powermoding),
        .versions_count = ECU_CONFIG_POWERMODING_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_POWERMODING_VERSION_V1,
                .size = sizeof(ecu_config_powermoding_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_POWERMODING
    {
        .flash_section_type = FLASH_SECTION_TYPE_CALIBRATION_CYLINDERS,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_calibration_cylinders_get_default_config,
        .data_ptr = &ecu_config_global_engine.calibration.cylinders,
        .data_size = sizeof(ecu_config_global_engine.calibration.cylinders),
        .versions_count = ECU_CONFIG_CYLINDERS_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_CYLINDERS_VERSION_V1,
                .size = sizeof(ecu_config_cylinders_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_CYLINDERS
    {
        .flash_section_type = FLASH_SECTION_TYPE_CALIBRATION_INJECTION,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_calibration_injection_get_default_config,
        .data_ptr = &ecu_config_global_engine.calibration.injection,
        .data_size = sizeof(ecu_config_global_engine.calibration.injection),
        .versions_count = ECU_CONFIG_INJECTION_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_INJECTION_VERSION_V1,
                .size = sizeof(ecu_config_injection_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_INJECTION
    {
        .flash_section_type = FLASH_SECTION_TYPE_CALIBRATION_IGNITION,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_calibration_ignition_get_default_config,
        .data_ptr = &ecu_config_global_engine.calibration.ignition,
        .data_size = sizeof(ecu_config_global_engine.calibration.ignition),
        .versions_count = ECU_CONFIG_IGNITION_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_IGNITION_VERSION_V1,
                .size = sizeof(ecu_config_ignition_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_IGNITION
    {
        .flash_section_type = FLASH_SECTION_TYPE_CALIBRATION_CALCDATA,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_calibration_calcdata_get_default_config,
        .data_ptr = &ecu_config_global_engine.calibration.calcdata,
        .data_size = sizeof(ecu_config_global_engine.calibration.calcdata),
        .versions_count = ECU_CONFIG_CALCDATA_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_CALCDATA_VERSION_V1,
                .size = sizeof(ecu_config_calcdata_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_CALCDATA
    {
        .flash_section_type = FLASH_SECTION_TYPE_CALIBRATION_TABLES,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_calibration_tables_get_default_config,
        .data_ptr = &ecu_config_global_engine.calibration.tables,
        .data_size = sizeof(ecu_config_global_engine.calibration.tables),
        .versions_count = ECU_CONFIG_TABLES_VERSION_MAX,
        .versions = {
            {
                .version = ECU_CONFIG_TABLES_VERSION_V1,
                .size = sizeof(ecu_config_tables_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_CALIB_TYPE_TABLES
};

static ecu_config_generic_ctx_t ecu_config_global_runtimes_ctx[ECU_CONFIG_RUNTIME_TYPE_ALL] = {
    {
        .flash_section_type = FLASH_SECTION_TYPE_RUNTIME_CORRECTIONS,
        .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)NULL,
        .data_ptr = &ecu_config_global_engine.runtime.corrections,
        .double_data_ptr = &ecu_config_global_engine_double.runtime.corrections,
        .data_size = sizeof(ecu_config_global_engine.runtime.corrections),
        .versions_count = CORRECTIONS_CONFIG_VERSION_MAX,
        .versions = {
            {
                .version = CORRECTIONS_CONFIG_VERSION_V1,
                .size = sizeof(ecu_config_corrections_v1_t),
                .translate_func = NULL,
            }
        },
    }, //ECU_CONFIG_RUNTIME_TYPE_CORRECTIONS
};

static ecu_config_device_ctx_t ecu_config_global_comm_ctx[ECU_CONFIG_COMM_TYPE_ALL] = {
    {
      .device_type = ECU_COMM_TYPE_CAN,
      .instances_count = ECU_COMM_CAN_MAX,
      .configure_func = (ecu_config_configure_func_t)ecu_comm_can_configure,
      .reset_func = (ecu_config_reset_func_t)ecu_comm_can_reset,
      .generic = {
          .flash_section_type = FLASH_SECTION_TYPE_COMM_CAN,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_comm_can_get_default_config,
          .data_ptr = &ecu_config_global_engine.comm.can[0],
          .data_size = sizeof(ecu_config_global_engine.comm.can[0]),
          .versions_count = CAN_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = ISOTP_CONFIG_VERSION_V1,
                  .size = sizeof(can_config_v1_t),
                  .translate_func = NULL,
              }
          },
      },
    }, //ECU_CONFIG_COMM_TYPE_CAN
    {
      .device_type = ECU_COMM_TYPE_KWP,
      .instances_count = ECU_COMM_KWP_MAX,
      .configure_func = (ecu_config_configure_func_t)ecu_comm_kwp_configure,
      .reset_func = (ecu_config_reset_func_t)ecu_comm_kwp_reset,
      .generic = {
          .flash_section_type = FLASH_SECTION_TYPE_COMM_KWP,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_comm_kwp_get_default_config,
          .data_ptr = &ecu_config_global_engine.comm.kwp[0],
          .data_size = sizeof(ecu_config_global_engine.comm.kwp[0]),
          .versions_count = KWP_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = KWP_CONFIG_VERSION_V1,
                  .size = sizeof(kwp_config_v1_t),
                  .translate_func = NULL,
              }
          },
      }
    }, //ECU_CONFIG_COMM_TYPE_KWP
    {
      .device_type = ECU_COMM_TYPE_ISOTP,
      .instances_count = ECU_COMM_ISOTP_MAX,
      .configure_func = (ecu_config_configure_func_t)ecu_comm_isotp_configure,
      .reset_func = (ecu_config_reset_func_t)ecu_comm_isotp_reset,
      .generic = {
          .flash_section_type = FLASH_SECTION_TYPE_COMM_ISOTP,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_comm_isotp_get_default_config,
          .data_ptr = &ecu_config_global_engine.comm.isotp[0],
          .data_size = sizeof(ecu_config_global_engine.comm.isotp[0]),
          .versions_count = ISOTP_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = ISOTP_CONFIG_VERSION_V1,
                  .size = sizeof(isotp_config_v1_t),
                  .translate_func = NULL,
              }
          },
      }
    }, //ECU_CONFIG_COMM_TYPE_ISOTP
    {
      .device_type = ECU_COMM_TYPE_UDS,
      .instances_count = ECU_COMM_UDS_MAX,
      .configure_func = (ecu_config_configure_func_t)ecu_comm_uds_configure,
      .reset_func = (ecu_config_reset_func_t)ecu_comm_uds_reset,
      .generic = {
          .flash_section_type = FLASH_SECTION_TYPE_COMM_UDS,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_comm_uds_get_default_config,
          .data_ptr = &ecu_config_global_engine.comm.uds[0],
          .data_size = sizeof(ecu_config_global_engine.comm.uds[0]),
          .versions_count = UDS_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = UDS_CONFIG_VERSION_V1,
                  .size = sizeof(uds_config_v1_t),
                  .translate_func = NULL,
              }
          },
      }
    }, //ECU_CONFIG_COMM_TYPE_UDS
    {
      .device_type = ECU_COMM_TYPE_OBD2,
      .instances_count = ECU_COMM_OBD2_MAX,
      .configure_func = (ecu_config_configure_func_t)ecu_comm_obd2_configure,
      .reset_func = (ecu_config_reset_func_t)ecu_comm_obd2_reset,
      .generic = {
          .flash_section_type = FLASH_SECTION_TYPE_COMM_OBD2,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_comm_obd2_get_default_config,
          .data_ptr = &ecu_config_global_engine.comm.obd2[0],
          .data_size = sizeof(ecu_config_global_engine.comm.obd2[0]),
          .versions_count = OBD2_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = OBD2_CONFIG_VERSION_V1,
                  .size = sizeof(obd2_config_v1_t),
                  .translate_func = NULL,
              }
          },
      }
    }, //ECU_CONFIG_COMM_TYPE_OBD2
    {
      .device_type = ECU_COMM_TYPE_ROUTER,
      .instances_count = ECU_COMM_ROUTER_MAX,
      .configure_func = (ecu_config_configure_func_t)ecu_comm_router_configure,
      .reset_func = (ecu_config_reset_func_t)ecu_comm_router_reset,
      .generic = {
          .flash_section_type = FLASH_SECTION_TYPE_COMM_ROUTER,
          .get_default_cfg_func = (ecu_config_get_default_cfg_func_t)ecu_comm_router_get_default_config,
          .data_ptr = &ecu_config_global_engine.comm.router[0],
          .data_size = sizeof(ecu_config_global_engine.comm.router[0]),
          .versions_count = ROUTER_CONFIG_VERSION_MAX,
          .versions = {
              {
                  .version = ROUTER_CONFIG_VERSION_V1,
                  .size = sizeof(router_config_v1_t),
                  .translate_func = NULL,
              }
          },
      }
    }, //ECU_CONFIG_COMM_TYPE_ROUTER
};

static void ecu_config_dma_clpt_cb(DMA_HandleTypeDef *hdma)
{
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  if(ctx->hdma == hdma) {
    ctx->dma_errode = E_OK;
  }
}

static void ecu_config_dma_err_cb(DMA_HandleTypeDef *hdma)
{
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  if(ctx->hdma == hdma) {
    ctx->dma_errode = E_IO;
  }
}

error_t ecu_config_global_init(void)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
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

    ctx->devices_count = ITEMSOF(ecu_config_global_device_ctx);
    ctx->devices = ecu_config_global_device_ctx;

    ctx->sensors_count = ITEMSOF(ecu_config_global_sensor_ctx);
    ctx->sensors = ecu_config_global_sensor_ctx;

    ctx->modules_count = ITEMSOF(ecu_config_global_module_ctx);
    ctx->modules = ecu_config_global_module_ctx;

    ctx->calibrations_count = ITEMSOF(ecu_config_global_calibration_ctx);
    ctx->calibrations = ecu_config_global_calibration_ctx;

    ctx->runtimes_count = ITEMSOF(ecu_config_global_runtimes_ctx);
    ctx->runtimes = ecu_config_global_runtimes_ctx;

    ctx->comm_count = ITEMSOF(ecu_config_global_comm_ctx);
    ctx->comm = ecu_config_global_comm_ctx;

    for(int c = 0; c < ctx->devices_count; c++) {
      for(int i = 0; i < ctx->devices[c].instances_count; i++) {
        err = flash_mem_layout_get_section_info(&section_info, ctx->devices[c].generic.flash_section_type, i);
        BREAK_IF(err != E_OK);
        BREAK_IF_ACTION(ctx->devices[c].generic.versions_count == 0, err = E_INVALACT);
        BREAK_IF_ACTION((ctx->devices[c].generic.data_size & 0x1F) != 0, err = E_INVALACT);
        BREAK_IF_ACTION(ctx->devices[c].generic.data_size > section_info->section_length - ECU_FLASH_SECTION_HEADER_LENGTH, err = E_INVALACT);

        if(ctx->devices[c].generic.get_default_cfg_func != NULL) {
          err = ctx->devices[c].generic.get_default_cfg_func(i, ctx->devices[c].generic.data_ptr + ctx->devices[c].generic.data_size * i);
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

    for(int c = 0; c < ctx->modules_count; c++) {
      for(int i = 0; i < ctx->modules[c].instances_count; i++) {
        err = flash_mem_layout_get_section_info(&section_info, ctx->modules[c].generic.flash_section_type, i);
        BREAK_IF(err != E_OK);
        BREAK_IF_ACTION(ctx->modules[c].generic.versions_count == 0, err = E_INVALACT);
        BREAK_IF_ACTION((ctx->modules[c].generic.data_size & 0x1F) != 0, err = E_INVALACT);
        BREAK_IF_ACTION(ctx->modules[c].generic.data_size > section_info->section_length - ECU_FLASH_SECTION_HEADER_LENGTH, err = E_INVALACT);

        if(ctx->modules[c].generic.get_default_cfg_func != NULL) {
          err = ctx->modules[c].generic.get_default_cfg_func(i, ctx->modules[c].generic.data_ptr + ctx->modules[c].generic.data_size * i);
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

    for(int c = 0; c < ctx->comm_count; c++) {
      for(int i = 0; i < ctx->comm[c].instances_count; i++) {
        err = flash_mem_layout_get_section_info(&section_info, ctx->comm[c].generic.flash_section_type, i);
        BREAK_IF(err != E_OK);
        BREAK_IF_ACTION(ctx->comm[c].generic.versions_count == 0, err = E_INVALACT);
        BREAK_IF_ACTION((ctx->comm[c].generic.data_size & 0x1F) != 0, err = E_INVALACT);
        BREAK_IF_ACTION(ctx->comm[c].generic.data_size > section_info->section_length - ECU_FLASH_SECTION_HEADER_LENGTH, err = E_INVALACT);

        if(ctx->comm[c].generic.get_default_cfg_func != NULL) {
          err = ctx->comm[c].generic.get_default_cfg_func(i, ctx->comm[c].generic.data_ptr + ctx->comm[c].generic.data_size * i);
        }
        BREAK_IF(err != E_OK);
      }
    }
    BREAK_IF(err != E_OK);


    ctx->op_req_type = ECU_CONFIG_TYPE_MAX;

    ctx->hdma = &hdma_memtomem_dma2_stream2;
    ctx->hdma->Init.Request = DMA_REQUEST_MEM2MEM;
    ctx->hdma->Init.Direction = DMA_MEMORY_TO_MEMORY;
    ctx->hdma->Init.PeriphInc = DMA_PINC_ENABLE;
    ctx->hdma->Init.MemInc = DMA_MINC_ENABLE;
    ctx->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    ctx->hdma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    status = HAL_DMA_Init(ctx->hdma);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

    status = HAL_DMA_RegisterCallback(ctx->hdma, HAL_DMA_XFER_CPLT_CB_ID, ecu_config_dma_clpt_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
    status = HAL_DMA_RegisterCallback(ctx->hdma, HAL_DMA_XFER_ERROR_CB_ID, ecu_config_dma_err_cb);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

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
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_type != ECU_CONFIG_PROCESS_TYPE_NONE && ctx->process_type != ECU_CONFIG_PROCESS_TYPE_FLASH_INIT, err = E_INVALACT);

    if(ctx->process_type == ECU_CONFIG_PROCESS_TYPE_NONE) {
      ctx->process_type = ECU_CONFIG_PROCESS_TYPE_FLASH_INIT;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_type = ECU_CONFIG_PROCESS_TYPE_NONE;
      }
    }
  } while(0);

  return err;
}

error_t ecu_config_global_devices_initialize(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->global_ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_type != ECU_CONFIG_PROCESS_TYPE_NONE && ctx->process_type != ECU_CONFIG_PROCESS_TYPE_DEVS_INIT, err = E_INVALACT);

    if(ctx->process_type == ECU_CONFIG_PROCESS_TYPE_NONE) {
      ctx->process_type = ECU_CONFIG_PROCESS_TYPE_DEVS_INIT;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_type = ECU_CONFIG_PROCESS_TYPE_NONE;
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
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_type != ECU_CONFIG_PROCESS_TYPE_NONE && ctx->process_type != ECU_CONFIG_PROCESS_TYPE_SENS_INIT, err = E_INVALACT);

    if(ctx->process_type == ECU_CONFIG_PROCESS_TYPE_NONE) {
      ctx->process_type = ECU_CONFIG_PROCESS_TYPE_SENS_INIT;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_type = ECU_CONFIG_PROCESS_TYPE_NONE;
      }
    }
  } while(0);

  return err;
}

error_t ecu_config_global_modules_initialize(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->global_ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_type != ECU_CONFIG_PROCESS_TYPE_NONE && ctx->process_type != ECU_CONFIG_PROCESS_TYPE_MODULES_INIT, err = E_INVALACT);

    if(ctx->process_type == ECU_CONFIG_PROCESS_TYPE_NONE) {
      ctx->process_type = ECU_CONFIG_PROCESS_TYPE_MODULES_INIT;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_type = ECU_CONFIG_PROCESS_TYPE_NONE;
      }
    }
  } while(0);

  return err;
}

error_t ecu_config_global_comm_initialize(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->global_ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_type != ECU_CONFIG_PROCESS_TYPE_NONE && ctx->process_type != ECU_CONFIG_PROCESS_TYPE_COMM_INIT, err = E_INVALACT);

    if(ctx->process_type == ECU_CONFIG_PROCESS_TYPE_NONE) {
      ctx->process_type = ECU_CONFIG_PROCESS_TYPE_COMM_INIT;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_type = ECU_CONFIG_PROCESS_TYPE_NONE;
      }
    }
  } while(0);

  return err;
}

error_t ecu_config_global_flash_erase(void)
{
  error_t err = E_AGAIN;
  ecu_config_global_runtime_ctx_t *ctx = &ecu_config_global_runtime_ctx;

  do {
    BREAK_IF_ACTION(ctx->global_ready == false, err = E_NOTRDY);
    BREAK_IF_ACTION(ctx->op_request != ECU_CONFIG_OP_NONE, err = E_INVALACT);
    BREAK_IF_ACTION(ctx->process_type != ECU_CONFIG_PROCESS_TYPE_NONE && ctx->process_type != ECU_CONFIG_PROCESS_TYPE_FLASH_ERASE, err = E_INVALACT);

    if(ctx->process_type == ECU_CONFIG_PROCESS_TYPE_NONE) {
      ctx->process_type = ECU_CONFIG_PROCESS_TYPE_FLASH_ERASE;
      ctx->process_result = E_AGAIN;
    } else {
      if(ctx->process_result != E_AGAIN) {
        err = ctx->process_result;
        ctx->process_type = ECU_CONFIG_PROCESS_TYPE_NONE;
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
    BREAK_IF_ACTION(ctx->process_type != ECU_CONFIG_PROCESS_TYPE_NONE, err = E_INVALACT);
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
