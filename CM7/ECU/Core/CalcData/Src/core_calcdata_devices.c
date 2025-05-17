/*
 * core_calcdata_devices.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "core_calcdata.h"
#include "config_global.h"

static const ecu_core_calcdata_devices_ctx_t ecu_core_calcdata_devices_ctx = {
    .devices = {
        {}, //ECU_DEVICE_TYPE_PULSEDADC
        {}, //ECU_DEVICE_TYPE_TCS
        {}, //ECU_DEVICE_TYPE_FLEXIO
        {
            .max = ECU_DEVICE_WBLS_MAX,
            .func_read = NULL,
            .func_write = NULL,
            .userdata = NULL,
        }, //ECU_DEVICE_TYPE_WBLS
        {
            .max = ECU_DEVICE_STEPPER_MAX,
            .func_read = NULL,
            .func_write = NULL,
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
