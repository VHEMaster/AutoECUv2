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
    .banked = {
      .global = {
          .sensor_ckp = {
              ECU_SENSOR_CKP_1, //ECU_CONFIG_IO_CKP_PRIMARY
          },
          .sensor_vss = {
              ECU_SENSOR_VSS_1, //ECU_CONFIG_IO_VSS_PRIMARY
          },
          .sensor_aps = {
              ECU_SENSOR_APS_1, //ECU_CONFIG_IO_APS_ACCELERATOR
          },
          .sensor_ect = {
              ECU_SENSOR_ECT_1, //ECU_CONFIG_IO_ECT_PRIMARY
              ECU_SENSOR_NONE, //ECU_CONFIG_IO_ECT_SECONDARY
          },
          .sensor_ots = {
              ECU_SENSOR_NONE, //ECU_CONFIG_IO_OTS_PRIMARY
          },
          .sensor_ops = {
              ECU_SENSOR_NONE, //ECU_CONFIG_IO_OPS_PRIMARY
          },

          .module_fuelpump = {
              ECU_MODULE_FUELPUMP_1, //ECU_CONFIG_IO_FUELPUMP_PRIMARY
          },
          .module_coolingfan = {
              ECU_MODULE_COOLINGFAN_1, //ECU_CONFIG_IO_COOLINGFAN_ONOFF_PRIMARY
              ECU_MODULE_COOLINGFAN_2, //ECU_CONFIG_IO_COOLINGFAN_ONOFF_SECONDARY
          },
          .module_ignpower = {
              ECU_MODULE_IGNPOWER_1, //ECU_CONFIG_IO_IGNPOWER_PRIMARY
          },
          .module_indication = {
              ECU_MODULE_INDICATION_1, //ECU_CONFIG_IO_INDICATION_CHECKENGINE
              ECU_MODULE_NONE, //ECU_CONFIG_IO_INDICATION_SHIFT_GREEN
              ECU_MODULE_NONE, //ECU_CONFIG_IO_INDICATION_SHIFT_RED
              ECU_MODULE_NONE, //ECU_CONFIG_IO_INDICATION_CUTOFF
              ECU_MODULE_NONE, //ECU_CONFIG_IO_INDICATION_RSVD1
              ECU_MODULE_NONE, //ECU_CONFIG_IO_INDICATION_RSVD2
          },
          .common = {
              .sensor_cmp = {
                  ECU_SENSOR_CMP_1, //ECU_CONFIG_IO_CMP_SOHC
                  ECU_SENSOR_NONE, //ECU_CONFIG_IO_CMP_DOHC_INTAKE
                  ECU_SENSOR_NONE, //ECU_CONFIG_IO_CMP_DOHC_EXHAUST
              },
              .sensor_egt = {
                  ECU_SENSOR_NONE, //ECU_CONFIG_IO_EGT_PRIMARY
              },
              .sensor_iat = {
                  ECU_SENSOR_IAT_1, //ECU_CONFIG_IO_IAT_MANIFOLD
                  ECU_SENSOR_NONE, //ECU_CONFIG_IO_IAT_THROTTLE
                  ECU_SENSOR_IAT_2, //ECU_CONFIG_IO_IAT_INLET
              },
              .sensor_maf = {
                  ECU_SENSOR_MAF_1, //ECU_CONFIG_IO_MAF_PRIMARY
              },
              .sensor_map = {
                  ECU_SENSOR_MAP_1, //ECU_CONFIG_IO_MAP_MANIFOLD
                  ECU_SENSOR_NONE, //ECU_CONFIG_IO_MAP_THROTTLE
              },
              .sensor_tps = {
                  ECU_SENSOR_TPS_1, //ECU_CONFIG_IO_TPS_PRIMARY
              },

              .device_stepper = {
                  ECU_DEVICE_STEPPER_1, //ECU_CONFIG_IO_STEPPER_IDLEVALVE
              },
              .device_wbls = {
                  ECU_DEVICE_WBLS_1, //ECU_CONFIG_IO_WBLS_PRIMARY
              },

              .module_etc = {
                  ECU_MODULE_ETC_1, //ECU_CONFIG_IO_ETC_PRIMARY
              },
              .module_vvt = {
                  ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_SOHC
                  ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_DOHC_INTAKE
                  ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_DOHC_EXHAUST
              },
              .module_wgcv = {
                  ECU_MODULE_NONE, //ECU_CONFIG_IO_WGCV_PRIMARY
              },
          },
      },

      .banks = {
          {
              .common = {
                  .sensor_cmp = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_CMP_SOHC
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_CMP_DOHC_INTAKE
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_CMP_DOHC_EXHAUST
                  },
                  .sensor_egt = {
                      ECU_SENSOR_EGT_1, //ECU_CONFIG_IO_EGT_PRIMARY
                  },
                  .sensor_iat = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_IAT_MANIFOLD
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_IAT_THROTTLE
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_IAT_INLET
                  },
                  .sensor_maf = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_MAF_PRIMARY
                  },
                  .sensor_map = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_MAP_MANIFOLD
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_MAP_THROTTLE
                  },
                  .sensor_tps = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_TPS_PRIMARY
                  },

                  .device_stepper = {
                      ECU_DEVICE_NONE, //ECU_CONFIG_IO_STEPPER_IDLEVALVE
                  },
                  .device_wbls = {
                      ECU_DEVICE_NONE, //ECU_CONFIG_IO_WBLS_PRIMARY
                  },

                  .module_etc = {
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_ETC_PRIMARY
                  },
                  .module_vvt = {
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_SOHC
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_DOHC_INTAKE
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_DOHC_EXHAUST
                  },
                  .module_wgcv = {
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_WGCV_PRIMARY
                  },
              },
          }, //ECU_BANK_1
          {
              .common = {
                  .sensor_cmp = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_CMP_SOHC
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_CMP_DOHC_INTAKE
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_CMP_DOHC_EXHAUST
                  },
                  .sensor_egt = {
                      ECU_SENSOR_EGT_2, //ECU_CONFIG_IO_EGT_PRIMARY
                  },
                  .sensor_iat = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_IAT_MANIFOLD
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_IAT_THROTTLE
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_IAT_INLET
                  },
                  .sensor_maf = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_MAF_PRIMARY
                  },
                  .sensor_map = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_MAP_MANIFOLD
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_MAP_THROTTLE
                  },
                  .sensor_tps = {
                      ECU_SENSOR_NONE, //ECU_CONFIG_IO_TPS_PRIMARY
                  },

                  .device_stepper = {
                      ECU_DEVICE_NONE, //ECU_CONFIG_IO_STEPPER_IDLEVALVE
                  },
                  .device_wbls = {
                      ECU_DEVICE_NONE, //ECU_CONFIG_IO_WBLS_PRIMARY
                  },

                  .module_etc = {
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_ETC_PRIMARY
                  },
                  .module_vvt = {
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_SOHC
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_DOHC_INTAKE
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_VVT_DOHC_EXHAUST
                  },
                  .module_wgcv = {
                      ECU_MODULE_NONE, //ECU_CONFIG_IO_WGCV_PRIMARY
                  },
              },
          }, //ECU_BANK_2
      },
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
