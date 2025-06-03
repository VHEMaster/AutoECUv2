/*
 * core_calcdata.c
 *
 *  Created on: Feb 16, 2025
 *      Author: VHEMaster
 */

#include "core_calcdata.h"
#include "config_global.h"

#include "core_calcdata_sensors.h"
#include "core_calcdata_devices.h"
#include "core_calcdata_modules.h"
#include "core_calcdata_timings.h"
#include "core_calcdata_inputs.h"

void core_calcdata_loop_slow(ecu_core_ctx_t *ctx)
{
  core_calcdata_sensors_read(ctx);
  core_calcdata_devices_read(ctx);
  core_calcdata_modules_read(ctx);
  core_calcdata_timings_read(ctx);

  core_calcdata_inputs_apply_sensors(ctx);

  core_calcdata_devices_write(ctx);
  core_calcdata_modules_write(ctx);
  core_calcdata_timings_write(ctx);
}
