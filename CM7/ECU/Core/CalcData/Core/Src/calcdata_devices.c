/*
 *calcdata_devices.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "calcdata_devices.h"
#include "config_global.h"

static void calcdata_device_read_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);
static void calcdata_device_read_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);

static void calcdata_device_write_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);
static void calcdata_device_write_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata);

static const ecu_core_calcdata_devices_ctx_t ecu_core_calcdata_devices_ctx = {
    .devices = {
        {}, //ECU_DEVICE_TYPE_PULSEDADC
        {}, //ECU_DEVICE_TYPE_TCS
        {}, //ECU_DEVICE_TYPE_FLEXIO
        {
            .max = ECU_DEVICE_WBLS_MAX,
            .func_read = calcdata_device_read_wbls,
            .func_write = calcdata_device_write_wbls,
            .userdata = NULL,
        }, //ECU_DEVICE_TYPE_WBLS
        {
            .max = ECU_DEVICE_STEPPER_MAX,
            .func_read = calcdata_device_read_stepper,
            .func_write = calcdata_device_write_stepper,
            .userdata = NULL,
        }, //ECU_DEVICE_TYPE_STEPPER
        {}, //ECU_DEVICE_TYPE_OUTPUT
        {}, //ECU_DEVICE_TYPE_MOTOR
        {}, //ECU_DEVICE_TYPE_FLASH
    },
};

void core_calcdata_devices_read(ecu_core_ctx_t *ctx)
{
  const ecu_core_calcdata_device_ctx_t *device_ctx;
  ecu_device_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_DEVICE_TYPE_MAX; type++) {
    device_ctx = &ecu_core_calcdata_devices_ctx.devices[type];
    instance_max = MIN(device_ctx->max, ECU_DEVICE_INSTANCE_MAX);

    if(device_ctx->func_read != NULL) {
      for(ecu_device_instance_t instance = 0; instance < instance_max; instance++) {
        device_ctx->func_read(ctx, instance, device_ctx->userdata);
      }
    }
  }
}

void core_calcdata_devices_write(ecu_core_ctx_t *ctx)
{
  const ecu_core_calcdata_device_ctx_t *device_ctx;
  ecu_device_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_DEVICE_TYPE_MAX; type++) {
    device_ctx = &ecu_core_calcdata_devices_ctx.devices[type];
    instance_max = MIN(device_ctx->max, ECU_DEVICE_INSTANCE_MAX);

    if(device_ctx->func_write != NULL) {
      for(ecu_device_instance_t instance = 0; instance < instance_max; instance++) {
        device_ctx->func_write(ctx, instance, device_ctx->userdata);
      }
    }
  }
}

static void calcdata_device_read_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  error_t err;
  cj125_data_t data;
  ecu_core_runtime_global_parameters_device_wbls_ctx_t *device_ctx = &ctx->runtime.global.parameters.devices.wbls[instance];
  ecu_core_runtime_global_parameters_device_wbls_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.devices.wbls[instance];

  err = ecu_devices_wbls_get_data(instance, &data);
  if(err == E_OK) {
    device_ctx->read.lambda_value = data.lambda_value;
    device_ctx->read.status = data.operating_status;
    device_ctx->read_valid = true;

  } else {
    device_ctx->read_valid = false;
  }

  if(simulated_ctx->read_valid) {
    device_ctx->read = simulated_ctx->read;
  } else {
    simulated_ctx->read = device_ctx->read;
  }
}

static void calcdata_device_read_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  error_t err;
  ecu_core_runtime_global_parameters_device_stepper_ctx_t *device_ctx = &ctx->runtime.global.parameters.devices.stepper[instance];
  ecu_core_runtime_global_parameters_device_stepper_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.devices.stepper[instance];

  err = ecu_devices_stepper_get_current(instance, &device_ctx->read.pos_current);
  err |= ecu_devices_stepper_get_target(instance, &device_ctx->read.pos_target);
  err |= ecu_devices_stepper_is_failure(instance, &device_ctx->read.failure);
  if(err == E_OK) {
    device_ctx->read_valid = true;
  } else {
    device_ctx->read_valid = false;
  }

  if(simulated_ctx->read_valid) {
    device_ctx->read = simulated_ctx->read;
  } else {
    simulated_ctx->read = device_ctx->read;
  }
}

static void calcdata_device_write_wbls(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_device_wbls_ctx_t *device_ctx = &ctx->runtime.global.parameters.devices.wbls[instance];
  ecu_core_runtime_global_parameters_device_wbls_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.devices.wbls[instance];

  if(simulated_ctx->write_valid) {
    device_ctx->write = simulated_ctx->write;
    device_ctx->write_valid = true;
  } else {
    if(device_ctx->write_valid) {
      simulated_ctx->write = device_ctx->write;
    }
  }

  if(device_ctx->write_valid) {
    (void)ecu_devices_wbls_set_heatup(instance, device_ctx->write.heatup);
    device_ctx->write_valid = false;
  }
}

static void calcdata_device_write_stepper(ecu_core_ctx_t *ctx, ecu_device_instance_t instance, void *userdata)
{
  ecu_core_runtime_global_parameters_device_stepper_ctx_t *device_ctx = &ctx->runtime.global.parameters.devices.stepper[instance];
  ecu_core_runtime_global_parameters_device_stepper_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.devices.stepper[instance];

  if(simulated_ctx->write_valid) {
    device_ctx->write = simulated_ctx->write;
    device_ctx->write_valid = true;
  } else {
    if(device_ctx->write_valid) {
      simulated_ctx->write = device_ctx->write;
    }
  }

  if(device_ctx->write_valid) {
    if(device_ctx->write.set_enabled) {
      (void)ecu_devices_stepper_enable(instance, device_ctx->write.enabled);
    }
    if(device_ctx->write.set_reset) {
      (void)ecu_devices_stepper_pos_reset(instance, device_ctx->write.pos_reset);
    } else {
      if(device_ctx->write.set_target) {
        (void)ecu_devices_stepper_set_target(instance, device_ctx->write.pos_target);
      }
      if(device_ctx->write.set_current) {
        (void)ecu_devices_stepper_set_current(instance, device_ctx->write.pos_current);
      }
    }
    device_ctx->write_valid = false;
  }
}
