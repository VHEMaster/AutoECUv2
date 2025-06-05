/*
 * core_calcdata_banked.c
 *
 *  Created on: Jun 2, 2025
 *      Author: VHEMaster
 */

#include "core_calcdata.h"
#include "config_global.h"

#define CORE_CALCDATA_GET_SENSOR_VALUE_PTR(TYPEUPPER, typelower, INSTUPPER, instlower, instance) &ctx->runtime.global.parameters.typelower##s[ECU_##TYPEUPPER##_TYPE_##INSTUPPER][instance]
#define CORE_CALCDATA_GET_DEVICE_VALUE_PTR(TYPEUPPER, typelower, INSTUPPER, instlower, instance) &ctx->runtime.global.parameters.typelower##s.instlower[instance]
#define CORE_CALCDATA_GET_MODULE_VALUE_PTR(TYPEUPPER, typelower, INSTUPPER, instlower, instance) &ctx->runtime.global.parameters.typelower##s.instlower[instance]

#define CORE_CALCDATA_BANKED_APPLY_GLOBAL(TYPEUPPER, typelower, INSTUPPER, instlower) \
  for(ecu_##typelower##_##instlower##_t i = 0; i < ECU_##TYPEUPPER##_##INSTUPPER##_MAX; i++) { \
    ecu_##typelower##_instance_t instance = ctx->engine_config->calibration.io.banked.global.typelower##_##instlower[i]; \
    if(instance != ECU_##TYPEUPPER##_NONE) { \
      ctx->runtime.banked.global.typelower##s_##instlower[i] = CORE_CALCDATA_GET_##TYPEUPPER##_VALUE_PTR(TYPEUPPER, typelower, INSTUPPER, instlower, instance); \
    } else { \
      ctx->runtime.banked.global.typelower##s_##instlower[i] = NULL; \
    } \
  }

#define CORE_CALCDATA_BANKED_APPLY_BANKED(TYPEUPPER, typelower, INSTUPPER, instlower) \
  for(ecu_##typelower##_##instlower##_t i = 0; i < ECU_##TYPEUPPER##_##INSTUPPER##_MAX; i++) { \
    ecu_##typelower##_instance_t instance = ctx->engine_config->calibration.io.banked.global.common.typelower##_##instlower[i]; \
    if(instance == ECU_##TYPEUPPER##_NONE) { \
      for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) { \
        instance = ctx->engine_config->calibration.io.banked.banks[b].common.typelower##_##instlower[i]; \
        if(instance != ECU_##TYPEUPPER##_NONE) { \
          ctx->runtime.banked.banks[b].typelower##s_##instlower[i] = CORE_CALCDATA_GET_##TYPEUPPER##_VALUE_PTR(TYPEUPPER, typelower, INSTUPPER, instlower, instance); \
        } else { \
          ctx->runtime.banked.banks[b].typelower##s_##instlower[i] = NULL; \
        } \
      } \
    } else { \
      for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) { \
        ctx->runtime.banked.banks[b].typelower##s_##instlower[i] = CORE_CALCDATA_GET_##TYPEUPPER##_VALUE_PTR(TYPEUPPER, typelower, INSTUPPER, instlower, instance); \
      } \
    } \
  }

static void core_calcdata_banked_apply_sensors(ecu_core_ctx_t *ctx);
static void core_calcdata_banked_apply_devices(ecu_core_ctx_t *ctx);
static void core_calcdata_banked_apply_modules(ecu_core_ctx_t *ctx);

void core_calcdata_banked_apply(ecu_core_ctx_t *ctx)
{
  core_calcdata_banked_apply_sensors(ctx);
  core_calcdata_banked_apply_devices(ctx);
  core_calcdata_banked_apply_modules(ctx);
}

STATIC_INLINE void core_calcdata_banked_apply_sensors(ecu_core_ctx_t *ctx)
{
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(SENSOR, sensor, APS, aps);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(SENSOR, sensor, CKP, ckp);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(SENSOR, sensor, ECT, ect);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(SENSOR, sensor, OPS, ops);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(SENSOR, sensor, OTS, ots);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(SENSOR, sensor, VSS, vss);

  CORE_CALCDATA_BANKED_APPLY_BANKED(SENSOR, sensor, CMP, cmp);
  CORE_CALCDATA_BANKED_APPLY_BANKED(SENSOR, sensor, EGT, egt);
  CORE_CALCDATA_BANKED_APPLY_BANKED(SENSOR, sensor, IAT, iat);
  CORE_CALCDATA_BANKED_APPLY_BANKED(SENSOR, sensor, MAF, maf);
  CORE_CALCDATA_BANKED_APPLY_BANKED(SENSOR, sensor, MAP, map);
  CORE_CALCDATA_BANKED_APPLY_BANKED(SENSOR, sensor, TPS, tps);
}

STATIC_INLINE void core_calcdata_banked_apply_devices(ecu_core_ctx_t *ctx)
{
  CORE_CALCDATA_BANKED_APPLY_BANKED(DEVICE, device, STEPPER, stepper);
  CORE_CALCDATA_BANKED_APPLY_BANKED(DEVICE, device, WBLS, wbls);
}

STATIC_INLINE void core_calcdata_banked_apply_modules(ecu_core_ctx_t *ctx)
{
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(MODULE, module, COOLINGFAN, coolingfan);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(MODULE, module, IGNPOWER, ignpower);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(MODULE, module, INDICATION, indication);
  CORE_CALCDATA_BANKED_APPLY_GLOBAL(MODULE, module, TIMING, timing);

  CORE_CALCDATA_BANKED_APPLY_BANKED(MODULE, module, ETC, etc);
  CORE_CALCDATA_BANKED_APPLY_BANKED(MODULE, module, VVT, vvt);
  CORE_CALCDATA_BANKED_APPLY_BANKED(MODULE, module, WGCV, wgcv);
}
