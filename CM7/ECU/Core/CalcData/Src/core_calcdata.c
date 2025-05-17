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

static ecu_core_calcdata_ctx_t ecu_core_calcdata_ctx = {

};

void core_calcdata_loop_slow(ecu_core_ctx_t *ctx)
{
  core_calcdata_sensors_read(ctx);
  core_calcdata_devices_read(ctx);
  core_calcdata_modules_read(ctx);


  core_calcdata_devices_write(ctx);
  core_calcdata_modules_write(ctx);
}
