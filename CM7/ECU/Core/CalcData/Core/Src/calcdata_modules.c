/*
 * calcdata_modules.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "calcdata_modules.h"
#include "config_global.h"

#define CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx) ((ctx)->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_INTERNAL])

static void calcdata_module_read_timing(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_read_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_read_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_read_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);

static void calcdata_module_write_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_coolingfan(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_indication(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_write_wgcv(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);

static void calcdata_module_invalidate_timing(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_invalidate_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_invalidate_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_invalidate_coolingfan(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_invalidate_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_invalidate_indication(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);
static void calcdata_module_invalidate_wgcv(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata);

static const ecu_core_calcdata_modules_ctx_t ecu_core_calcdata_modules_ctx = {
    .modules = {
        {
            .max = ECU_MODULE_TIMING_MAX,
            .func_read = calcdata_module_read_timing,
            .func_write = NULL,
            .func_invalidate = calcdata_module_invalidate_timing,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_TIMING
        {
            .max = ECU_MODULE_ETC_MAX,
            .func_read = calcdata_module_read_etc,
            .func_write = calcdata_module_write_etc,
            .func_invalidate = calcdata_module_invalidate_etc,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_ETC
        {
            .max = ECU_MODULE_VVT_MAX,
            .func_read = calcdata_module_read_vvt,
            .func_write = calcdata_module_write_vvt,
            .func_invalidate = calcdata_module_invalidate_vvt,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_VVT
        {
            .max = ECU_MODULE_FUELPUMP_MAX,
            .func_read = NULL,
            .func_write = NULL,
            .func_invalidate = NULL,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_FUELPUMP
        {
            .max = ECU_MODULE_COOLINGFAN_MAX,
            .func_read = NULL,
            .func_write = calcdata_module_write_coolingfan,
            .func_invalidate = calcdata_module_invalidate_coolingfan,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_COOLINGFAN
        {
            .max = ECU_MODULE_IGNPOWER_MAX,
            .func_read = calcdata_module_read_ignpower,
            .func_write = calcdata_module_write_ignpower,
            .func_invalidate = calcdata_module_invalidate_ignpower,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_IGNPOWER
        {
            .max = ECU_MODULE_INDICATION_MAX,
            .func_read = NULL,
            .func_write = calcdata_module_write_indication,
            .func_invalidate = calcdata_module_invalidate_indication,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_INDICATION
        {
            .max = ECU_MODULE_WGCV_MAX,
            .func_read = NULL,
            .func_write = calcdata_module_write_wgcv,
            .func_invalidate = calcdata_module_invalidate_wgcv,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_WGCV
    },
};

void core_calcdata_modules_read(ecu_core_ctx_t *ctx)
{
  error_t err;
  bool enabled;
  const ecu_core_calcdata_module_ctx_t *module_ctx;
  ecu_module_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_MODULE_TYPE_MAX; type++) {
    module_ctx = &ecu_core_calcdata_modules_ctx.modules[type];
    instance_max = MIN(module_ctx->max, ECU_MODULE_INSTANCE_MAX);

    if(module_ctx->func_read != NULL) {
      for(ecu_module_instance_t instance = 0; instance < instance_max; instance++) {
        err = ecu_modules_get_module_enabled(type, instance, &enabled);
        if(err == E_OK && enabled) {
          module_ctx->func_read(ctx, instance, module_ctx->userdata);
        } else if(module_ctx->func_invalidate != NULL) {
          module_ctx->func_invalidate(ctx, instance, module_ctx->userdata);
        }
      }
    }
  }
}

void core_calcdata_modules_write(ecu_core_ctx_t *ctx)
{
  error_t err;
  bool enabled;
  const ecu_core_calcdata_module_ctx_t *module_ctx;
  ecu_module_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_MODULE_TYPE_MAX; type++) {
    module_ctx = &ecu_core_calcdata_modules_ctx.modules[type];
    instance_max = MIN(module_ctx->max, ECU_MODULE_INSTANCE_MAX);

    if(module_ctx->func_write != NULL) {
      for(ecu_module_instance_t instance = 0; instance < instance_max; instance++) {
        err = ecu_modules_get_module_enabled(type, instance, &enabled);
        if(err == E_OK && enabled) {
          module_ctx->func_write(ctx, instance, module_ctx->userdata);
        } else if(module_ctx->func_invalidate != NULL) {
          module_ctx->func_invalidate(ctx, instance, module_ctx->userdata);
        }
      }
    }
  }
}

static void calcdata_module_read_timing(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  timing_data_t data;
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_TIMING][instance];

  err = ecu_modules_timing_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read[ECU_MODULE_TIMING_READ_PARAM_MODE].value = data.crankshaft.mode;
    module_ctx->read[ECU_MODULE_TIMING_READ_PARAM_MODE].valid = true;
    module_ctx->read[ECU_MODULE_TIMING_READ_PARAM_RPM].value = data.crankshaft.sensor_data.period;
    module_ctx->read[ECU_MODULE_TIMING_READ_PARAM_RPM].valid = true;
    module_ctx->read[ECU_MODULE_TIMING_READ_PARAM_PERIOD].value = data.crankshaft.sensor_data.rpm;
    module_ctx->read[ECU_MODULE_TIMING_READ_PARAM_PERIOD].valid = true;
    module_ctx->read[ECU_MODULE_TIMING_READ_PARAM_REVS_COUNT].value = data.crankshaft.sensor_data.revs_count;
    module_ctx->read[ECU_MODULE_TIMING_READ_PARAM_REVS_COUNT].valid = true;
  } else {
    for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_TIMING_READ_PARAM_MAX; i++) {
      module_ctx->read[i].valid = false;
    }
  }
}

static void calcdata_module_read_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  etc_data_t data;
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_ETC][instance];

  err = ecu_modules_etc_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read[ECU_MODULE_ETC_READ_PARAM_ENABLED].value = data.enabled;
    module_ctx->read[ECU_MODULE_ETC_READ_PARAM_ENABLED].valid = true;
    module_ctx->read[ECU_MODULE_ETC_READ_PARAM_POS_CURRENT].value = data.current_position;
    module_ctx->read[ECU_MODULE_ETC_READ_PARAM_POS_CURRENT].valid = true;
    module_ctx->read[ECU_MODULE_ETC_READ_PARAM_POS_TARGET].value = data.target_position;
    module_ctx->read[ECU_MODULE_ETC_READ_PARAM_POS_TARGET].valid = true;
  } else {
    for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_ETC_READ_PARAM_MAX; i++) {
      module_ctx->read[i].valid = false;
    }
  }
}

static void calcdata_module_read_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  vvt_data_t data;
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_VVT][instance];

  err = ecu_modules_vvt_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_ENABLED].value = data.enabled;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_ENABLED].valid = true;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_POS_CURRENT].value = data.pos_current;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_POS_CURRENT].valid = true;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_POS_TARGET].value = data.pos_target;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_POS_TARGET].valid = true;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_DUTYCYCLE_CURRENT].value = data.dutycycle_current;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_DUTYCYCLE_CURRENT].valid = true;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_DUTYCYCLE_TARGET].value = data.dutycycle_target;
    module_ctx->read[ECU_MODULE_VVT_READ_PARAM_DUTYCYCLE_TARGET].valid = true;
  } else {
    for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_VVT_READ_PARAM_MAX; i++) {
      module_ctx->read[i].valid = false;
    }
  }
}

static void calcdata_module_read_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  ignpower_data_t data;
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_IGNPOWER][instance];

  err = ecu_modules_ignpower_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read[ECU_MODULE_IGNPOWER_READ_PARAM_OPERATING].value = (data.crankshaft_operating || data.components_operating) ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    module_ctx->read[ECU_MODULE_IGNPOWER_READ_PARAM_OPERATING].valid = true;
  } else {
    for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_IGNPOWER_READ_PARAM_MAX; i++) {
      module_ctx->read[i].valid = false;
    }
  }
}

static void calcdata_module_write_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_ETC][instance];

  if(module_ctx->write[ECU_MODULE_ETC_WRITE_PARAM_ENABLED].valid) {
      (void)ecu_modules_etc_set_enabled(instance, module_ctx->write[ECU_MODULE_ETC_WRITE_PARAM_ENABLED].value > ECU_RUNTIME_PARAMETER_FALSE ? true : false);
      module_ctx->write[ECU_MODULE_ETC_WRITE_PARAM_ENABLED].valid = false;
  }
  if(module_ctx->write[ECU_MODULE_ETC_WRITE_PARAM_POS_TARGET].valid) {
      (void)ecu_modules_etc_set_target_position(instance, module_ctx->write[ECU_MODULE_ETC_WRITE_PARAM_POS_TARGET].value);
      module_ctx->write[ECU_MODULE_ETC_WRITE_PARAM_POS_TARGET].valid = false;
  }
}

static void calcdata_module_write_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_VVT][instance];

  if(module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_ENABLED].valid) {
    (void)ecu_modules_vvt_set_enabled(instance, module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_ENABLED].value > ECU_RUNTIME_PARAMETER_FALSE ? true : false);
    module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_ENABLED].valid = false;
  }
  if(module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_POS_TARGET].valid) {
    (void)ecu_modules_vvt_set_target_position(instance, module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_POS_TARGET].value);
    module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_POS_TARGET].valid = false;
  }
  if(module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_DUTYCYCLE_TARGET].valid) {
    (void)ecu_modules_vvt_set_target_dutycycle(instance, module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_DUTYCYCLE_TARGET].value);
    module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_DUTYCYCLE_TARGET].valid = false;
  }
}

static void calcdata_module_write_coolingfan(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_COOLINGFAN][instance];

  if(module_ctx->write[ECU_MODULE_COOLINGFAN_WRITE_PARAM_ACTIVATE_TRIGGER].valid) {
    (void)ecu_modules_coolingfan_activate_trigger(instance, module_ctx->write[ECU_MODULE_COOLINGFAN_WRITE_PARAM_ACTIVATE_TRIGGER].value > ECU_RUNTIME_PARAMETER_FALSE ? true : false);
    module_ctx->write[ECU_MODULE_COOLINGFAN_WRITE_PARAM_ACTIVATE_TRIGGER].valid = false;
  }
  if(module_ctx->write[ECU_MODULE_COOLINGFAN_WRITE_PARAM_EMERGENCY_TRIGGER].valid) {
    (void)ecu_modules_coolingfan_emergency_trigger(instance, module_ctx->write[ECU_MODULE_COOLINGFAN_WRITE_PARAM_EMERGENCY_TRIGGER].value > ECU_RUNTIME_PARAMETER_FALSE ? true : false);
        module_ctx->write[ECU_MODULE_COOLINGFAN_WRITE_PARAM_EMERGENCY_TRIGGER].valid = false;
  }
}

static void calcdata_module_write_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_IGNPOWER][instance];

  if(module_ctx->write[ECU_MODULE_IGNPOWER_WRITE_PARAM_OPERATING].valid) {
    (void)ecu_modules_ignpower_set_operating(instance, module_ctx->write[ECU_MODULE_IGNPOWER_WRITE_PARAM_OPERATING].value > ECU_RUNTIME_PARAMETER_FALSE ? true : false);
    module_ctx->write[ECU_MODULE_IGNPOWER_WRITE_PARAM_OPERATING].valid = false;
  }
  if(module_ctx->write[ECU_MODULE_IGNPOWER_WRITE_PARAM_OPERATING_TRIGGER_SIGNAL].valid) {
    (void)ecu_modules_ignpower_trigger_operating_signal(instance);
    module_ctx->write[ECU_MODULE_VVT_WRITE_PARAM_POS_TARGET].valid = false;
  }
}

static void calcdata_module_write_indication(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_INDICATION][instance];

  if(module_ctx->write[ECU_MODULE_INDICATION_WRITE_PARAM_SET_MANUAL_ENGAGED].valid) {
    (void)ecu_modules_indication_manual_set(instance, module_ctx->write[ECU_MODULE_INDICATION_WRITE_PARAM_SET_MANUAL_ENGAGED].value > ECU_RUNTIME_PARAMETER_FALSE ? true : false);
    module_ctx->write[ECU_MODULE_INDICATION_WRITE_PARAM_SET_MANUAL_ENGAGED].valid = false;
  }
}

static void calcdata_module_write_wgcv(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_WGCV][instance];

  if(module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_ENABLED].valid) {
    (void)ecu_modules_wgcv_set_enabled(instance, module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_ENABLED].value > ECU_RUNTIME_PARAMETER_FALSE ? true : false);
    module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_ENABLED].valid = false;
  }
  if(module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_TARGET_BOOST].valid) {
    (void)ecu_modules_wgcv_set_target_boost(instance, module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_TARGET_BOOST].value);
    module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_TARGET_BOOST].valid = false;
  }
  if(module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_TARGET_DUTYCYCLE].valid) {
    (void)ecu_modules_wgcv_set_dutycycle(instance, module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_TARGET_DUTYCYCLE].value);
    module_ctx->write[ECU_MODULE_WGCV_WRITE_PARAM_TARGET_DUTYCYCLE].valid = false;
  }
}


static void calcdata_module_invalidate_timing(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_TIMING][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_TIMING_READ_PARAM_MAX; i++) {
    module_ctx->read[i].valid = false;
  }
}

static void calcdata_module_invalidate_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_ETC][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_ETC_READ_PARAM_MAX; i++) {
    module_ctx->read[i].valid = false;
  }
  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_ETC_WRITE_PARAM_MAX; i++) {
    module_ctx->write[i].valid = false;
  }
}

static void calcdata_module_invalidate_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_VVT][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_VVT_READ_PARAM_MAX; i++) {
    module_ctx->read[i].valid = false;
  }
  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_VVT_WRITE_PARAM_MAX; i++) {
    module_ctx->write[i].valid = false;
  }
}

static void calcdata_module_invalidate_coolingfan(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_COOLINGFAN][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_COOLINGFAN_WRITE_PARAM_MAX; i++) {
    module_ctx->write[i].valid = false;
  }
}

static void calcdata_module_invalidate_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_IGNPOWER][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_IGNPOWER_READ_PARAM_MAX; i++) {
    module_ctx->read[i].valid = false;
  }
  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_IGNPOWER_WRITE_PARAM_MAX; i++) {
    module_ctx->write[i].valid = false;
  }
}

static void calcdata_module_invalidate_indication(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_INDICATION][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_INDICATION_WRITE_PARAM_MAX; i++) {
    module_ctx->write[i].valid = false;
  }
}

static void calcdata_module_invalidate_wgcv(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *module_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).modules[ECU_MODULE_TYPE_WGCV][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_MODULE_WGCV_WRITE_PARAM_MAX; i++) {
    module_ctx->write[i].valid = false;
  }
}
