/*
 * calcdata.c
 *
 *  Created on: Feb 16, 2025
 *      Author: VHEMaster
 */

#include "calcdata.h"
#include "config_global.h"

#include "calcdata_sensors.h"
#include "calcdata_devices.h"
#include "calcdata_modules.h"
#include "calcdata_timings.h"
#include "calcdata_banked.h"

#include "calcdata_inputs.h"
#include "calcdata_outputs.h"

#include "calcdata_proc.h"
#include "calcdata_simulation.h"

static void calcdata_prepare(ecu_core_ctx_t *ctx);
static void calcdata_invalidate(ecu_core_ctx_t *ctx);
static void calcdata_process(ecu_core_ctx_t *ctx);
static void calcdata_inputs_read(ecu_core_ctx_t *ctx);
static void calcdata_inputs_write(ecu_core_ctx_t *ctx);

time_msmnt_item_t time_calcdata = {0};

void core_calcdata_loop_slow(ecu_core_ctx_t *ctx)
{
  core_calcdata_simulation(ctx);

  time_msmt_start(&time_calcdata);

  calcdata_prepare(ctx);
  calcdata_invalidate(ctx);

  calcdata_inputs_read(ctx);

  core_calcdata_banked_apply(ctx);

  calcdata_process(ctx);

  calcdata_inputs_write(ctx);

  time_msmt_stop(&time_calcdata);
}

STATIC_INLINE void calcdata_prepare(ecu_core_ctx_t *ctx)
{
  ctx->runtime.global.cylinders_count = ctx->calibration->cylinders.cylinders_count;
  ctx->runtime.global.banks_count = ctx->calibration->cylinders.banks_count;
}

STATIC_INLINE void calcdata_invalidate(ecu_core_ctx_t *ctx)
{
  core_calcdata_proc_input_interp_invalidate(ctx);
  core_calcdata_proc_output_invalidate(ctx);
}

STATIC_INLINE void calcdata_process(ecu_core_ctx_t *ctx)
{
  core_calcdata_inputs_process(ctx, CORE_CALCDATA_INPUTS_STAGE_1);
  core_calcdata_outputs_process(ctx, CORE_CALCDATA_OUTPUTS_STAGE_1);

  core_calcdata_inputs_process(ctx, CORE_CALCDATA_INPUTS_STAGE_2);
  core_calcdata_outputs_process(ctx, CORE_CALCDATA_OUTPUTS_STAGE_2);

  core_calcdata_inputs_process(ctx, CORE_CALCDATA_INPUTS_STAGE_3);
  core_calcdata_outputs_process(ctx, CORE_CALCDATA_OUTPUTS_STAGE_3);
}

STATIC_INLINE void calcdata_inputs_read(ecu_core_ctx_t *ctx)
{
  core_calcdata_sensors_read(ctx);
  core_calcdata_devices_read(ctx);
  core_calcdata_modules_read(ctx);
  core_calcdata_timings_read(ctx);
}

STATIC_INLINE void calcdata_inputs_write(ecu_core_ctx_t *ctx)
{
  core_calcdata_devices_write(ctx);
  core_calcdata_modules_write(ctx);
  core_calcdata_timings_write(ctx);
}
