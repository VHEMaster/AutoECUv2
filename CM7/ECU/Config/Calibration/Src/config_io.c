/*
 * config_io.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_io_t ecu_calibration_io_config_default = {

    .global = {
        .sensor_ckp = {
            ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_CKP_PRIMARY
        },
        .sensor_vss = {
            ECU_SENSOR_NONE, //ECU_CONFIG_IO_VSS_PRIMARY
        },
        .sensor_aps = {
            ECU_SENSOR_NONE, //ECU_CONFIG_IO_APS_ACCELERATOR
        },
        .common = {
            .sensor_cmp = {
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_CMP_INTAKE
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_CMP_EXHAUST
            },
            .sensor_ect = {
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_ECT_PRIMARY
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_ECT_SECONDARY
            },
            .sensor_egt = {
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_EGT_PRIMARY
            },
            .sensor_iat = {
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_INLET
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_THROTTLE
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_MANIFOLD
            },
            .sensor_maf = {
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAF_PRIMARY
            },
            .sensor_map = {
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_INLET
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_THROTTLE
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_MANIFOLD
            },
            .sensor_tps = {
                ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_TPS_PRIMARY
            },

            .device_stepper = {
                ECU_DEVICE_NONE, //ECU_CONFIG_BANKS_STEPPER_IDLEVALVE
            },
            .device_wbls = {
                ECU_DEVICE_NONE, //ECU_CONFIG_BANKS_WBLS_PRIMARY
            },

            .module_etc = {
                ECU_MODULE_NONE, //ECU_CONFIG_BANKS_ETC_PRIMARY
            },
            .module_vvt = {
                ECU_MODULE_NONE, //ECU_CONFIG_BANKS_VVT_INTAKE
                ECU_MODULE_NONE, //ECU_CONFIG_BANKS_VVT_EXHAUST
            },
        },
    },

    .banks = {
        {
            .common = {
                .sensor_cmp = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_CMP_INTAKE
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_CMP_EXHAUST
                },
                .sensor_ect = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_ECT_PRIMARY
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_ECT_SECONDARY
                },
                .sensor_egt = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_EGT_PRIMARY
                },
                .sensor_iat = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_INLET
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_THROTTLE
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_MANIFOLD
                },
                .sensor_maf = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAF_PRIMARY
                },
                .sensor_map = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_INLET
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_THROTTLE
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_MANIFOLD
                },
                .sensor_tps = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_TPS_PRIMARY
                },

                .device_stepper = {
                    ECU_DEVICE_NONE, //ECU_CONFIG_BANKS_STEPPER_IDLEVALVE
                },
                .device_wbls = {
                    ECU_DEVICE_NONE, //ECU_CONFIG_BANKS_WBLS_PRIMARY
                },

                .module_etc = {
                    ECU_MODULE_NONE, //ECU_CONFIG_BANKS_ETC_PRIMARY
                },
                .module_vvt = {
                    ECU_MODULE_NONE, //ECU_CONFIG_BANKS_VVT_INTAKE
                    ECU_MODULE_NONE, //ECU_CONFIG_BANKS_VVT_EXHAUST
                },
            },
        }, //ECU_BANK_1
        {
            .common = {
                .sensor_cmp = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_CMP_INTAKE
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_CMP_EXHAUST
                },
                .sensor_ect = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_ECT_PRIMARY
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_ECT_SECONDARY
                },
                .sensor_egt = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_EGT_PRIMARY
                },
                .sensor_iat = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_INLET
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_THROTTLE
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_IAT_MANIFOLD
                },
                .sensor_maf = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAF_PRIMARY
                },
                .sensor_map = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_INLET
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_THROTTLE
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_MAP_MANIFOLD
                },
                .sensor_tps = {
                    ECU_SENSOR_NONE, //ECU_CONFIG_BANKS_TPS_PRIMARY
                },

                .device_stepper = {
                    ECU_DEVICE_NONE, //ECU_CONFIG_BANKS_STEPPER_IDLEVALVE
                },
                .device_wbls = {
                    ECU_DEVICE_NONE, //ECU_CONFIG_BANKS_WBLS_PRIMARY
                },

                .module_etc = {
                    ECU_MODULE_NONE, //ECU_CONFIG_BANKS_ETC_PRIMARY
                },
                .module_vvt = {
                    ECU_MODULE_NONE, //ECU_CONFIG_BANKS_VVT_INTAKE
                    ECU_MODULE_NONE, //ECU_CONFIG_BANKS_VVT_EXHAUST
                },
            },
        }, //ECU_BANK_2
    }
};

error_t ecu_calibration_io_get_default_config(ecu_calibration_instance_t instance, ecu_config_io_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_calibration_io_config_default, sizeof(ecu_config_io_t));

  } while(0);

  return err;
}
