/*
 * core_calcdata_modules.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "core_calcdata.h"
#include "config_global.h"

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
            .func_read = calcdata_module_read_etc,
            .func_write = calcdata_module_write_etc,
            .userdata = NULL,
        }, //ECU_MODULE_TYPE_ETC
        {
            .max = ECU_MODULE_VVT_MAX,
            .func_read = calcdata_module_read_vvt,
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
  ecu_core_runtime_global_parameters_module_timing_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.timing[instance];

  err = ecu_modules_timing_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read.mode = data.crankshaft.mode;
    module_ctx->read.period = data.crankshaft.sensor_data.period;
    module_ctx->read.rpm = data.crankshaft.sensor_data.rpm;
    module_ctx->read.revs_count = data.crankshaft.sensor_data.revs_count;
    module_ctx->read_valid = true;
  } else {
    module_ctx->read_valid = false;
  }

  if(simulated_ctx->read_valid) {
    module_ctx->read = simulated_ctx->read;
  } else {
    simulated_ctx->read = module_ctx->read;
  }
}

static void calcdata_module_read_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  etc_data_t data;
  ecu_core_runtime_global_parameters_module_etc_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.etc[instance];
  ecu_core_runtime_global_parameters_module_etc_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.etc[instance];

  err = ecu_modules_etc_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read.enabled = data.enabled;
    module_ctx->read.pos_current = data.current_position;
    module_ctx->read.pos_target = data.target_position;
    module_ctx->read_valid = true;
  } else {
    module_ctx->read_valid = false;
  }

  if(simulated_ctx->read_valid) {
    module_ctx->read = simulated_ctx->read;
  } else {
    simulated_ctx->read = module_ctx->read;
  }
}

static void calcdata_module_read_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  vvt_data_t data;
  ecu_core_runtime_global_parameters_module_vvt_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.vvt[instance];
  ecu_core_runtime_global_parameters_module_vvt_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.vvt[instance];

  err = ecu_modules_vvt_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read.enabled = data.enabled;
    module_ctx->read.pos_current = data.pos_current;
    module_ctx->read.pos_target = data.pos_target;
    module_ctx->read.dc_current = data.dutycycle_current;
    module_ctx->read.dc_target = data.dutycycle_target;
    module_ctx->read_valid = true;
  } else {
    module_ctx->read_valid = false;
  }

  if(simulated_ctx->read_valid) {
    module_ctx->read = simulated_ctx->read;
  } else {
    simulated_ctx->read = module_ctx->read;
  }
}

static void calcdata_module_read_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  error_t err;
  ignpower_data_t data;
  ecu_core_runtime_global_parameters_module_ignpower_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.ignpower[instance];
  ecu_core_runtime_global_parameters_module_ignpower_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.ignpower[instance];

  err = ecu_modules_ignpower_get_data(instance, &data);
  if(err == E_OK) {
    module_ctx->read.operating = data.components_operating || data.crankshaft_operating;
    module_ctx->read_valid = true;
  } else {
    module_ctx->read_valid = false;
  }

  if(simulated_ctx->read_valid) {
    module_ctx->read = simulated_ctx->read;
  } else {
    simulated_ctx->read = module_ctx->read;
  }
}

static void calcdata_module_write_etc(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_module_etc_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.etc[instance];
  ecu_core_runtime_global_parameters_module_etc_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.etc[instance];

  if(simulated_ctx->write_valid) {
    module_ctx->write = simulated_ctx->write;
  } else {
    simulated_ctx->write = module_ctx->write;
  }

  if(module_ctx->write_valid) {
    if(module_ctx->write.set_enabled) {
      (void)ecu_modules_etc_set_enabled(instance, module_ctx->write.enabled);
    }
    if(module_ctx->write.set_target) {
      (void)ecu_modules_etc_set_target_position(instance, module_ctx->write.target);
    }
  }
}

static void calcdata_module_write_vvt(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_module_vvt_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.vvt[instance];
  ecu_core_runtime_global_parameters_module_vvt_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.vvt[instance];

  if(simulated_ctx->write_valid) {
    module_ctx->write = simulated_ctx->write;
  } else {
    simulated_ctx->write = module_ctx->write;
  }

  if(module_ctx->write_valid) {
    if(module_ctx->write.set_enabled) {
      (void)ecu_modules_vvt_set_enabled(instance, module_ctx->write.enabled);
    }
    if(module_ctx->write.set_target_pos) {
      (void)ecu_modules_vvt_set_target_position(instance, module_ctx->write.target_pos);
    }
    if(module_ctx->write.set_target_dc) {
      (void)ecu_modules_vvt_set_target_dutycycle(instance, module_ctx->write.target_dc);
    }
  }
}

static void calcdata_module_write_coolingfan(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_module_coolingfan_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.coolingfan[instance];
  ecu_core_runtime_global_parameters_module_coolingfan_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.coolingfan[instance];

  if(simulated_ctx->write_valid) {
    module_ctx->write = simulated_ctx->write;
  } else {
    simulated_ctx->write = module_ctx->write;
  }

  if(module_ctx->write_valid) {
    if(module_ctx->write.set_activate_trigger) {
      (void)ecu_modules_coolingfan_activate_trigger(instance, module_ctx->write.activate_trigger);
    }
    if(module_ctx->write.set_emergency_trigger) {
      (void)ecu_modules_coolingfan_emergency_trigger(instance, module_ctx->write.emergency_trigger);
    }
  }
}

static void calcdata_module_write_ignpower(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_module_ignpower_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.ignpower[instance];
  ecu_core_runtime_global_parameters_module_ignpower_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.ignpower[instance];

  if(simulated_ctx->write_valid) {
    module_ctx->write = simulated_ctx->write;
  } else {
    simulated_ctx->write = module_ctx->write;
  }

  if(module_ctx->write_valid) {
    if(module_ctx->write.set_operating) {
      (void)ecu_modules_ignpower_set_operating(instance, module_ctx->write.operating);
    } else if(module_ctx->write.set_trigger) {
      (void)ecu_modules_ignpower_trigger_operating_signal(instance);
    }
  }
}

static void calcdata_module_write_indication(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_module_indication_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.indication[instance];
  ecu_core_runtime_global_parameters_module_indication_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.indication[instance];

  if(simulated_ctx->write_valid) {
    module_ctx->write = simulated_ctx->write;
  } else {
    simulated_ctx->write = module_ctx->write;
  }

  if(module_ctx->write_valid) {
    if(module_ctx->write.set_manual_engaged) {
      (void)ecu_modules_indication_manual_set(instance, module_ctx->write.manual_enabled);
    }
  }
}

static void calcdata_module_write_wgcv(ecu_core_ctx_t *ctx, ecu_module_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_module_wgcv_ctx_t *module_ctx = &ctx->runtime.global.parameters.modules.wgcv[instance];
  ecu_core_runtime_global_parameters_module_wgcv_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.modules.wgcv[instance];

  if(simulated_ctx->write_valid) {
    module_ctx->write = simulated_ctx->write;
  } else {
    simulated_ctx->write = module_ctx->write;
  }

  if(module_ctx->write_valid) {
    if(module_ctx->write.set_enabled) {
      (void)ecu_modules_wgcv_set_enabled(instance, module_ctx->write.enabled);
    }
    if(module_ctx->write.set_boost_target) {
      (void)ecu_modules_wgcv_set_target_boost(instance, module_ctx->write.boost_target);
    }
    if(module_ctx->write.set_dc_target) {
      (void)ecu_modules_wgcv_set_dutycycle(instance, module_ctx->write.dc_target);
    }
  }
}
