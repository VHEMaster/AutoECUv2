/*
 * core_calcdata_banked.c
 *
 *  Created on: Jun 2, 2025
 *      Author: VHEMaster
 */

#include "core_calcdata.h"
#include "config_global.h"

#define CORE_CALCDATA_GET_SENSOR_VALUE_PTR(upper, instance) &ctx->runtime.global.parameters.sensors[ECU_SENSOR_TYPE_##upper][instance]

#define CORE_CALCDATA_BANKED_APPLY_GLOBAL_SENSOR(upper, lower) \
  for(ecu_sensor_##lower##_t i = 0; i < ECU_SENSOR_##upper##_MAX; i++) { \
    ecu_sensor_instance_t instance = ctx->engine_config->calibration.io.banked.global.sensor_##lower[i]; \
    if(instance != ECU_SENSOR_NONE) { \
      ctx->runtime.banked.global.sensors_##lower[i] = CORE_CALCDATA_GET_SENSOR_VALUE_PTR(upper, instance); \
    } else { \
      ctx->runtime.banked.global.sensors_##lower[i] = NULL; \
    } \
  }

#define CORE_CALCDATA_BANKED_APPLY_BANKED_SENSOR(upper, lower) \
  for(ecu_sensor_##lower##_t i = 0; i < ECU_SENSOR_##upper##_MAX; i++) { \
    ecu_sensor_instance_t instance = ctx->engine_config->calibration.io.banked.global.common.sensor_##lower[i]; \
    if(instance == ECU_SENSOR_NONE) { \
      for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) { \
        instance = ctx->engine_config->calibration.io.banked.banks[b].common.sensor_##lower[i]; \
        if(instance != ECU_SENSOR_NONE) { \
          ctx->runtime.banked.banks[b].sensors_##lower[i] = CORE_CALCDATA_GET_SENSOR_VALUE_PTR(upper, instance); \
        } else { \
          ctx->runtime.banked.banks[b].sensors_##lower[i] = NULL; \
        } \
      } \
    } else { \
      for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) { \
        ctx->runtime.banked.banks[b].sensors_##lower[i] = CORE_CALCDATA_GET_SENSOR_VALUE_PTR(upper, instance); \
      } \
    } \
  }

void core_calcdata_banked_apply_sensors(ecu_core_ctx_t *ctx)
{
  CORE_CALCDATA_BANKED_APPLY_GLOBAL_SENSOR(APS, aps);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL_SENSOR(CKP, ckp);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL_SENSOR(ECT, ect);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL_SENSOR(OPS, ops);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL_SENSOR(OTS, ots);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL_SENSOR(VSS, vss);

  CORE_CALCDATA_BANKED_APPLY_BANKED_SENSOR(CMP, cmp);
  CORE_CALCDATA_BANKED_APPLY_BANKED_SENSOR(EGT, egt);
  CORE_CALCDATA_BANKED_APPLY_BANKED_SENSOR(IAT, iat);
  CORE_CALCDATA_BANKED_APPLY_BANKED_SENSOR(MAF, maf);
  CORE_CALCDATA_BANKED_APPLY_BANKED_SENSOR(MAP, map);
  CORE_CALCDATA_BANKED_APPLY_BANKED_SENSOR(TPS, tps);
}
