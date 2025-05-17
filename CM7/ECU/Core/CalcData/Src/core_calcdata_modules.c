/*
 * core_calcdata_modules.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "core_calcdata.h"
#include "config_global.h"

static void calcdata_module_read_timing(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_read_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);

static void calcdata_module_write_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_coolingfan(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_indication(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_wgcv(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);

static const ecu_core_calcdata_modules_ctx_t ecu_core_calcdata_modules_ctx = {
    .modules = {
        {
            .max = ECU_MODULE_TIMING_MAX,
            .func_read = calcdata_module_read_timing,
            .func_write = NULL,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_TIMING
        {
            .max = ECU_MODULE_ETC_MAX,
            .func_read = NULL,
            .func_write = calcdata_module_write_etc,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_ETC
        {
            .max = ECU_MODULE_VVT_MAX,
            .func_read = NULL,
            .func_write = calcdata_module_write_vvt,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_VVT
        {
            .max = ECU_MODULE_FUELPUMP_MAX,
            .func_read = NULL,
            .func_write = NULL,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_FUELPUMP
        {
            .max = ECU_MODULE_COOLINGFAN_MAX,
            .func_read = NULL,
            .func_write = calcdata_module_write_coolingfan,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_COOLINGFAN
        {
            .max = ECU_MODULE_IGNPOWER_MAX,
            .func_read = calcdata_module_read_ignpower,
            .func_write = calcdata_module_write_ignpower,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_IGNPOWER
        {
            .max = ECU_MODULE_INDICATION_MAX,
            .func_read = NULL,
            .func_write = calcdata_module_write_indication,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_INDICATION
        {
            .max = ECU_MODULE_WGCV_MAX,
            .func_read = NULL,
            .func_write = calcdata_module_write_wgcv,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_WGCV
    },
};

void core_calcdata_modules_read(ecu_core_ctx_t *ctx)
{
  const ecu_core_calcdata_module_ctx_t *module_ctx;
  ecu_module_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_MODULE_TYPE_MAX; type++) {
    module_ctx = &ecu_core_calcdata_modules_ctx.modules[type];
    instance_max = MIN(module_ctx->max, ECU_MODULE_INSTANCE_MAX);

    if(module_ctx->func_read != NULL) {
      for(ecu_module_instance_t instance = 0; instance < instance_max; instance++) {
        module_ctx->func_read(ctx, instance, module_ctx->userdata);
      }
    }
  }
}

void core_calcdata_modules_write(ecu_core_ctx_t *ctx)
{
  const ecu_core_calcdata_module_ctx_t *module_ctx;
  ecu_module_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_MODULE_TYPE_MAX; type++) {
    module_ctx = &ecu_core_calcdata_modules_ctx.modules[type];
    instance_max = MIN(module_ctx->max, ECU_MODULE_INSTANCE_MAX);

    if(module_ctx->func_write != NULL) {
      for(ecu_module_instance_t instance = 0; instance < instance_max; instance++) {
        module_ctx->func_write(ctx, instance, module_ctx->userdata);
      }
    }
  }
}

static void calcdata_module_read_timing(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  timing_data_t data;
  ecu_core_runtime_global_parameters_module_timing_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.timing[instance];

  err = ecu_modules_timing_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read_valid = true;

  } else {
    module_ctx->read_valid = false;
  }
}

static void calcdata_module_read_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  ignpower_data_t data;
  ecu_core_runtime_global_parameters_module_ignpower_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.ignpower[instance];

  err = ecu_modules_ignpower_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read_valid = true;

  } else {
    module_ctx->read_valid = false;
  }
}

static void calcdata_module_write_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  etc_data_t data;
  ecu_core_runtime_global_parameters_module_etc_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.etc[instance];

  if(module_ctx->write_valid) {

  }
}

static void calcdata_module_write_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  vvt_data_t data;
  ecu_core_runtime_global_parameters_module_vvt_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.vvt[instance];

  if(module_ctx->write_valid) {

  }
}

static void calcdata_module_write_coolingfan(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  coolingfan_data_t data;
  ecu_core_runtime_global_parameters_module_coolingfan_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.coolingfan[instance];

  if(module_ctx->write_valid) {

  }
}

static void calcdata_module_write_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ignpower_data_t data;
  ecu_core_runtime_global_parameters_module_ignpower_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.ignpower[instance];

  if(module_ctx->write_valid) {

  }
}

static void calcdata_module_write_indication(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  indication_data_t data;
  ecu_core_runtime_global_parameters_module_indication_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.indication[instance];

  if(module_ctx->write_valid) {

  }
}

static void calcdata_module_write_wgcv(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  wgcv_data_t data;
  ecu_core_runtime_global_parameters_module_wgcv_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.wgcv[instance];

  if(module_ctx->write_valid) {

  }
}
