/*
 * calcdata_virtual.c
 *
 *  Created on: Nov 3, 2025
 *      Author: VHEMaster
 */

#include "config_global.h"
#include "calcdata_virtual.h"


#define CALCDATA_VIRTUAL_READ(ctx, TYPEUPPERCASE, typelowercase, INSTUPPERCASE, instlowercase) \
{ \
  const ecu_core_runtime_value_ctx_t *value_src; \
  ecu_core_runtime_value_ctx_t *value_dest; \
  ecu_core_runtime_value_ctx_t *value_sim; \
  bool valid = false; \
  for(ecu_##typelowercase##_##instlowercase##_t i = 0; i < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_MAX; i++) {  \
    for(ecu_##typelowercase##_##instlowercase##_read_params_t param = 0; param < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_READ_PARAM_MAX; param++) { \
      valid = false;  \
      value_dest = &ctx->runtime.global.parameters.typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].params[ECU_COMMON_READ][param]; \
      value_sim = &ctx->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].params[ECU_COMMON_READ][param];  \
      for(ecu_core_runtime_parameters_virtual_source_t source = 0; source < ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX; source++) {  \
        value_src = &ctx->runtime.global.parameters_virtual[source].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].params[ECU_COMMON_READ][param];  \
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

#define CALCDATA_VIRTUAL_WRITE(ctx, TYPEUPPERCASE, typelowercase, INSTUPPERCASE, instlowercase) \
{ \
  const ecu_core_runtime_value_ctx_t *value_src; \
  ecu_core_runtime_value_ctx_t *value_dest; \
  ecu_core_runtime_value_ctx_t *value_sim; \
  bool valid = false; \
  for(ecu_##typelowercase##_##instlowercase##_t i = 0; i < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_MAX; i++) {  \
    for(ecu_##typelowercase##_##instlowercase##_write_params_t param = 0; param < ECU_##TYPEUPPERCASE##_##INSTUPPERCASE##_WRITE_PARAM_MAX; param++) { \
      valid = false;  \
      value_dest = &ctx->runtime.global.parameters.typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].params[ECU_COMMON_WRITE][param]; \
      value_sim = &ctx->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_SIMULATED].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].params[ECU_COMMON_WRITE][param];  \
      for(ecu_core_runtime_parameters_virtual_source_t source = 0; source < ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX; source++) {  \
        value_src = &ctx->runtime.global.parameters_virtual[source].typelowercase##s[ECU_##TYPEUPPERCASE##_TYPE_##INSTUPPERCASE][i].params[ECU_COMMON_WRITE][param];  \
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
  error_t err;
  bool valid = false;

  ecu_config_common_entity_type_t type_max;
  ecu_config_common_entity_instance_t instance_max;
  const ecu_core_runtime_global_instance_parameters_ctx_t *param_ctx_src;
  const ecu_core_runtime_value_ctx_t *param_value_src;

  ecu_core_runtime_global_instance_parameters_ctx_t *param_ctx_dst;
  ecu_core_runtime_value_ctx_t *param_value_dst;

  do {
    err = ecu_config_common_get_entity_type_max(ECU_COMMON_ENTITY_SENSOR, &type_max);
    BREAK_IF(err != E_OK);

    for(ecu_config_common_entity_type_t t = 0; t < type_max; t++) {
      err = ecu_config_common_get_entity_type_instance_max(ECU_COMMON_ENTITY_SENSOR, t, &instance_max);
      BREAK_IF(err != E_OK);
      for(ecu_config_common_entity_instance_t i = 0; i < instance_max; i++) {
        param_ctx_dst = &ctx->runtime.global.parameters.sensors[t][i];
        for(ecu_runtime_param_index_t p = 0; p < param_ctx_dst->count[ECU_COMMON_READ]; p++) {
          valid = false;
          param_value_dst = &param_ctx_dst->params[ECU_COMMON_READ][p];
          for(ecu_core_runtime_parameters_virtual_source_t s = 0; s < ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_MAX; s++) {
            param_ctx_src = &ctx->runtime.global.parameters_virtual[s].sensors[t][i];
            param_value_src = &param_ctx_src->params[ECU_COMMON_READ][p];
            if(param_value_src->valid) {
              param_value_dst->value = param_value_src->value;
              param_value_dst->valid = param_value_src->valid;
              valid = true;
              break;
            }
          }
          if(valid != true) {
            param_value_dst->valid = false;
          }
        }
      }
    }
  } while(0);
}

static void calcdata_virtual_read_devices(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_READ(ctx, DEVICE, device, WBLS, wbls);
  CALCDATA_VIRTUAL_READ(ctx, DEVICE, device, STEPPER, stepper);
}

static void calcdata_virtual_read_modules(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_READ(ctx, MODULE, module, ETC, etc);
  CALCDATA_VIRTUAL_READ(ctx, MODULE, module, VVT, vvt);
  CALCDATA_VIRTUAL_READ(ctx, MODULE, module, COOLINGFAN, coolingfan);
  CALCDATA_VIRTUAL_READ(ctx, MODULE, module, IGNPOWER, ignpower);
  CALCDATA_VIRTUAL_READ(ctx, MODULE, module, INDICATION, indication);
  CALCDATA_VIRTUAL_READ(ctx, MODULE, module, WGCV, wgcv);
}

static void calcdata_virtual_read_timings(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_READ(ctx, TIMING, timing, IGNITION, ignition);
  CALCDATA_VIRTUAL_READ(ctx, TIMING, timing, INJECTION, injection);
}

static void calcdata_virtual_write_devices(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_WRITE(ctx, DEVICE, device, WBLS, wbls);
  CALCDATA_VIRTUAL_WRITE(ctx, DEVICE, device, STEPPER, stepper);
}

static void calcdata_virtual_write_modules(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_WRITE(ctx, MODULE, module, ETC, etc);
  CALCDATA_VIRTUAL_WRITE(ctx, MODULE, module, VVT, vvt);
  CALCDATA_VIRTUAL_WRITE(ctx, MODULE, module, COOLINGFAN, coolingfan);
  CALCDATA_VIRTUAL_WRITE(ctx, MODULE, module, IGNPOWER, ignpower);
  CALCDATA_VIRTUAL_WRITE(ctx, MODULE, module, INDICATION, indication);
  CALCDATA_VIRTUAL_WRITE(ctx, MODULE, module, WGCV, wgcv);
}

static void calcdata_virtual_write_timings(ecu_core_ctx_t *ctx)
{
  CALCDATA_VIRTUAL_WRITE(ctx, TIMING, timing, IGNITION, ignition);
  CALCDATA_VIRTUAL_WRITE(ctx, TIMING, timing, INJECTION, injection);
}
