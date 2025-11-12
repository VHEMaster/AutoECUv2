/*
 *calcdata_devices.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "calcdata_devices.h"
#include "config_global.h"

#define CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx) ((ctx)->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_INTERNAL])

static void calcdata_device_read_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);
static void calcdata_device_read_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);

static void calcdata_device_write_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);
static void calcdata_device_write_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);

static void calcdata_device_invalidate_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);
static void calcdata_device_invalidate_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);

static const ecu_core_calcdata_devices_ctx_t ecu_core_calcdata_devices_ctx = {
    .devices = {
        {}, //ECU_DEVICE_TYPE_PULSEDADC
        {}, //ECU_DEVICE_TYPE_TCS
        {}, //ECU_DEVICE_TYPE_FLEXIO
        {
            .max = ECU_DEVICE_WBLS_MAX,
            .func_read = calcdata_device_read_wbls,
            .func_write = calcdata_device_write_wbls,
            .func_invalidate = calcdata_device_invalidate_wbls,
            .userdata = NULL,
        }, //ECU_DEVICE_TYPE_WBLS
        {
            .max = ECU_DEVICE_STEPPER_MAX,
            .func_read = calcdata_device_read_stepper,
            .func_write = calcdata_device_write_stepper,
            .func_invalidate = calcdata_device_invalidate_stepper,
            .userdata = NULL,
        }, //ECU_DEVICE_TYPE_STEPPER
        {}, //ECU_DEVICE_TYPE_OUTPUT
        {}, //ECU_DEVICE_TYPE_MOTOR
        {}, //ECU_DEVICE_TYPE_FLASH
    },
};

void core_calcdata_devices_read(ecu_core_ctx_t *ctx)
{
  error_t err;
  bool enabled;
  const ecu_core_calcdata_device_ctx_t *device_ctx;
  ecu_device_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_DEVICE_TYPE_MAX; type++) {
    device_ctx = &ecu_core_calcdata_devices_ctx.devices[type];
    instance_max = MIN(device_ctx->max, ECU_DEVICE_INSTANCE_MAX);

    if(device_ctx->func_read != NULL) {
      for(ecu_device_instance_t instance = 0; instance < instance_max; instance++) {
        err = ecu_devices_get_device_enabled(type, instance, &enabled);
        if(err == E_OK && enabled) {
          device_ctx->func_read(ctx, instance, device_ctx->userdata);
        } else if(device_ctx->func_invalidate != NULL) {
          device_ctx->func_invalidate(ctx, instance, device_ctx->userdata);
        }
      }
    }
  }
}

void core_calcdata_devices_write(ecu_core_ctx_t *ctx)
{
  error_t err;
  bool enabled;
  const ecu_core_calcdata_device_ctx_t *device_ctx;
  ecu_device_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_DEVICE_TYPE_MAX; type++) {
    device_ctx = &ecu_core_calcdata_devices_ctx.devices[type];
    instance_max = MIN(device_ctx->max, ECU_DEVICE_INSTANCE_MAX);

    if(device_ctx->func_write != NULL) {
      for(ecu_device_instance_t instance = 0; instance < instance_max; instance++) {
        err = ecu_devices_get_device_enabled(type, instance, &enabled);
        if(err == E_OK && enabled) {
          device_ctx->func_write(ctx, instance, device_ctx->userdata);
        } else if(device_ctx->func_invalidate != NULL) {
          device_ctx->func_invalidate(ctx, instance, device_ctx->userdata);
        }
      }
    }
  }
}

static void calcdata_device_read_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  error_t err;
  cj125_data_t data;
  ecu_core_runtime_global_parameters_device_ctx_t *device_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).devices[ECU_DEVICE_TYPE_WBLS][instance];

  err = ecu_devices_wbls_get_data(instance, &data);
  if(err == E_OK) {
    device_ctx->read[ECU_MODULE_ETC_READ_PARAM_ENABLED].value = data.lambda_value;
    device_ctx->read[ECU_MODULE_ETC_READ_PARAM_ENABLED].valid = true;
    device_ctx->read[ECU_MODULE_ETC_READ_PARAM_ENABLED].value = data.operating_status;
    device_ctx->read[ECU_MODULE_ETC_READ_PARAM_ENABLED].valid = true;

  } else {
    for(ecu_runtime_param_index_t i = 0; i < ECU_DEVICE_WBLS_READ_PARAM_MAX; i++) {
      device_ctx->read[i].valid = false;
    }
  }
}

static void calcdata_device_read_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  error_t err;
  ecu_core_runtime_global_parameters_device_ctx_t *device_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).devices[ECU_DEVICE_TYPE_STEPPER][instance];
  int32_t position;
  bool failure;

  err = ecu_devices_stepper_get_current(instance, &position);
  if(err == E_OK) {
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_POS_CURRENT].value = position;
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_POS_CURRENT].valid = true;
  } else {
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_POS_CURRENT].valid = false;
  }

  err = ecu_devices_stepper_get_target(instance, &position);
  if(err == E_OK) {
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_POS_TARGET].value = position;
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_POS_TARGET].valid = true;

  } else {
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_POS_TARGET].valid = false;
  }

  err = ecu_devices_stepper_is_failure(instance, &failure);
  if(err == E_OK) {
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_FAILURE].value = failure ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_FAILURE].valid = true;
  } else {
    device_ctx->read[ECU_DEVICE_STEPPER_READ_PARAM_FAILURE].valid = false;
  }
}

static void calcdata_device_write_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_device_ctx_t *device_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).devices[ECU_DEVICE_TYPE_WBLS][instance];

  if(device_ctx->write[ECU_DEVICE_WBLS_WRITE_PARAM_HEATUP_TYPE].valid) {
    (void)ecu_devices_wbls_set_heatup(instance, device_ctx->write[ECU_DEVICE_WBLS_WRITE_PARAM_HEATUP_TYPE].value);
    device_ctx->write[ECU_DEVICE_WBLS_WRITE_PARAM_HEATUP_TYPE].valid = false;
  }
}

static void calcdata_device_write_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_device_ctx_t *device_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).devices[ECU_DEVICE_TYPE_STEPPER][instance];

  if(device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_ENABLED].valid) {
    (void)ecu_devices_stepper_enable(instance, device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_ENABLED].value > ECU_RUNTIME_PARAMETER_FALSE ? true : false);
    device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_ENABLED].valid = false;
  }
  if(device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_RESET].valid) {
    (void)ecu_devices_stepper_pos_reset(instance, device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_RESET].value);
    device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_RESET].valid = false;
  }
  if(device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_TARGET].valid) {
    (void)ecu_devices_stepper_set_target(instance, device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_TARGET].value);
    device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_TARGET].valid = false;
  }
  if(device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_CURRENT].valid) {
    (void)ecu_devices_stepper_set_current(instance, device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_CURRENT].value);
    device_ctx->write[ECU_DEVICE_STEPPER_WRITE_PARAM_POS_CURRENT].valid = false;
  }
}

static void calcdata_device_invalidate_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_device_ctx_t *device_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).devices[ECU_DEVICE_TYPE_STEPPER][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_DEVICE_STEPPER_READ_PARAM_MAX; i++) {
    device_ctx->read[i].valid = false;
  }
  for(ecu_runtime_param_index_t i = 0; i < ECU_DEVICE_STEPPER_WRITE_PARAM_MAX; i++) {
    device_ctx->write[i].valid = false;
  }
}

static void calcdata_device_invalidate_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_device_ctx_t *device_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).devices[ECU_DEVICE_TYPE_WBLS][instance];

  for(ecu_runtime_param_index_t i = 0; i < ECU_DEVICE_WBLS_READ_PARAM_MAX; i++) {
    device_ctx->read[i].valid = false;
  }
  for(ecu_runtime_param_index_t i = 0; i < ECU_DEVICE_WBLS_WRITE_PARAM_MAX; i++) {
    device_ctx->write[i].valid = false;
  }
}
