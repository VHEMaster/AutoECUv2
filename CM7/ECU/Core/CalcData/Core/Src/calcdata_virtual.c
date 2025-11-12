/*
 * calcdata_virtual.c
 *
 *  Created on: Nov 3, 2025
 *      Author: VHEMaster
 */

#include "config_global.h"
#include "calcdata_virtual.h"


#define CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, TYPEUPPERCASE, typelowercase, INSTUPPERCASE, instlowercase) \
{ \
  const ecu_core_runtime_value_ctx_t *value_src; \
  ecu_core_runtime_value_ctx_t *value_dest; \
  ecu_core_runtime_value_ctx_t *value_sim; \
  bool valid = false; \
  for(ecu_##typelowercase##_##instlowercase##_t i = 0; i < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_MAX; i++) {  \
    for(ecu_##typelowercase##_##instlowercase##_read_params_t param = 0; param < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_READ_PARAM_MAX; param++) { \
      valid = false;  \
      value_dest = &ctx->runtime.global.parameters.typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].read[param]; \
      value_sim = &ctx->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].read[param];  \
      for(ecu_core_runtime_parameters_virtual_source_t source = 0; source < ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX; source++) {  \
        value_src = &ctx->runtime.global.parameters_virtual[source].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].read[param];  \
        if(value_src->valid) {  \
          value_dest->value = value_src->value; \
          value_dest->valid = value_src->valid; \
          valid = true; \
          if(source != ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED) { \
            value_sim->value = value_src->value; \
          } \
          break;  \
        } \
      } \
      if(!valid) {  \
        value_sim->valid = false; \
        value_dest->valid = false; \
      } \
    } \
  } \
}

#define CALCDATA_VIRTUAL_TIMING_READ(ctx, TYPEUPPERCASE, typelowercase, INSTUPPERCASE, instlowercase) \
{ \
  const ecu_core_runtime_value_ctx_t *value_src; \
  ecu_core_runtime_value_ctx_t *value_dest; \
  ecu_core_runtime_value_ctx_t *value_sim; \
  bool valid = false; \
  for(ecu_##typelowercase##_##instlowercase##_read_params_t param = 0; param < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_READ_PARAM_MAX; param++) { \
    valid = false;  \
    value_dest = &ctx->runtime.global.parameters.typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE].read[param]; \
    value_sim = &ctx->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE].read[param];  \
    for(ecu_core_runtime_parameters_virtual_source_t source = 0; source < ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX; source++) {  \
      value_src = &ctx->runtime.global.parameters_virtual[source].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE].read[param];  \
      if(value_src->valid) {  \
        value_dest->value = value_src->value; \
        value_dest->valid = value_src->valid; \
        valid = true; \
        if(source != ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED) { \
          value_sim->value = value_src->value; \
        } \
        break;  \
      } \
    } \
    if(!valid) {  \
      value_sim->valid = false; \
      value_dest->valid = false; \
    } \
  } \
}

#define CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, TYPEUPPERCASE, typelowercase, INSTUPPERCASE, instlowercase) \
{ \
  const ecu_core_runtime_value_ctx_t *value_src; \
  ecu_core_runtime_value_ctx_t *value_dest; \
  ecu_core_runtime_value_ctx_t *value_sim; \
  bool valid = false; \
  for(ecu_##typelowercase##_##instlowercase##_t i = 0; i < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_MAX; i++) {  \
    for(ecu_##typelowercase##_##instlowercase##_write_params_t param = 0; param < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_WRITE_PARAM_MAX; param++) { \
      valid = false;  \
      value_dest = &ctx->runtime.global.parameters.typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].write[param]; \
      value_sim = &ctx->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].write[param];  \
      for(ecu_core_runtime_parameters_virtual_source_t source = 0; source < ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX; source++) {  \
        value_src = &ctx->runtime.global.parameters_virtual[source].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].write[param];  \
        if(value_src->valid) {  \
          value_dest->value = value_src->value; \
          value_dest->valid = value_src->valid; \
          valid = true; \
          if(source != ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED) { \
            value_sim->value = value_src->value; \
          } \
          break;  \
        } \
      } \
      if(!valid) {  \
        value_sim->valid = false; \
        value_dest->valid = false; \
      } \
    } \
  } \
}

#define CALCDATA_VIRTUAL_TIMING_WRITE(ctx, TYPEUPPERCASE, typelowercase, INSTUPPERCASE, instlowercase) \
{ \
  const ecu_core_runtime_value_ctx_t *value_src; \
  ecu_core_runtime_value_ctx_t *value_dest; \
  ecu_core_runtime_value_ctx_t *value_sim; \
  bool valid = false; \
  for(ecu_##typelowercase##_##instlowercase##_write_params_t param = 0; param < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_WRITE_PARAM_MAX; param++) { \
    valid = false;  \
    value_dest = &ctx->runtime.global.parameters.typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE].write[param]; \
    value_sim = &ctx->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE].write[param];  \
    for(ecu_core_runtime_parameters_virtual_source_t source = 0; source < ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX; source++) {  \
      value_src = &ctx->runtime.global.parameters_virtual[source].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE].write[param];  \
      if(value_src->valid) {  \
        value_dest->value = value_src->value; \
        value_dest->valid = value_src->valid; \
        valid = true; \
        if(source != ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED) { \
          value_sim->value = value_src->value; \
        } \
        break;  \
      } \
    } \
    if(!valid) {  \
      value_sim->valid = false; \
      value_dest->valid = false; \
    } \
  } \
}

static void calcdata_virtual_read_sensors(ecu_core_ctx_t *ctx);
static void calcdata_virtual_read_devices(ecu_core_ctx_t *ctx);
static void calcdata_virtual_read_modules(ecu_core_ctx_t *ctx);
static void calcdata_virtual_read_timings(ecu_core_ctx_t *ctx);

static void calcdata_virtual_write_devices(ecu_core_ctx_t *ctx);
static void calcdata_virtual_write_modules(ecu_core_ctx_t *ctx);
static void calcdata_virtual_write_timings(ecu_core_ctx_t *ctx);

void calcdata_virtual_read(ecu_core_ctx_t *ctx)
{
  calcdata_virtual_read_sensors(ctx);
  calcdata_virtual_read_devices(ctx);
  calcdata_virtual_read_modules(ctx);
  calcdata_virtual_read_timings(ctx);
}

void calcdata_virtual_write(ecu_core_ctx_t *ctx)
{
  calcdata_virtual_write_timings(ctx);
  calcdata_virtual_write_modules(ctx);
  calcdata_virtual_write_devices(ctx);
}

static void calcdata_virtual_read_sensors(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, APS, aps);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, CKP, ckp);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, ECT, ect);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, OPS, ops);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, OTS, ots);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, VSS, vss);

  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, CMP, cmp);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, EGT, egt);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, IAT, iat);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, MAF, maf);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, MAP, map);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, SENSOR, sensor, TPS, tps);
}

static void calcdata_virtual_read_devices(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, DEVICE, device, WBLS, wbls);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, DEVICE, device, STEPPER, stepper);
}

static void calcdata_virtual_read_modules(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, MODULE, module, TIMING, timing);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, MODULE, module, ETC, etc);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, MODULE, module, VVT, vvt);
  //CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, MODULE, module, COOLINGFAN, coolingfan);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, MODULE, module, IGNPOWER, ignpower);
  //CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, MODULE, module, INDICATION, indication);
  //CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_READ(ctx, MODULE, module, WGCV, wgcv);
}

static void calcdata_virtual_read_timings(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_TIMING_READ(ctx, TIMING, timing, IGNITION, ignition);
  CALCDATA_VIRTUAL_TIMING_READ(ctx, TIMING, timing, INJECTION, injection);
}

static void calcdata_virtual_write_devices(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, DEVICE, device, WBLS, wbls);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, DEVICE, device, STEPPER, stepper);
}

static void calcdata_virtual_write_modules(ecu_core_ctx_t *ctx)
{
  //CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, MODULE, module, TIMING, timing);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, MODULE, module, ETC, etc);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, MODULE, module, VVT, vvt);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, MODULE, module, COOLINGFAN, coolingfan);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, MODULE, module, IGNPOWER, ignpower);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, MODULE, module, INDICATION, indication);
  CALCDATA_VIRTUAL_SENSOR_DEVICE_MODULE_WRITE(ctx, MODULE, module, WGCV, wgcv);
}

static void calcdata_virtual_write_timings(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_TIMING_WRITE(ctx, TIMING, timing, IGNITION, ignition);
  CALCDATA_VIRTUAL_TIMING_WRITE(ctx, TIMING, timing, INJECTION, injection);
}
