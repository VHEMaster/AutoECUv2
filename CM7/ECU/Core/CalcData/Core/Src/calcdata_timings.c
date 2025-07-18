/*
 * calcdata_timings.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "calcdata_timings.h"
#include "config_global.h"

static void calcdata_timing_read_ignition(ecu_core_ctx_t *ctx, void *userdata);
static void calcdata_timing_read_injection(ecu_core_ctx_t *ctx, void *userdata);

static void calcdata_timing_write_ignition(ecu_core_ctx_t *ctx, void *userdata);
static void calcdata_timing_write_injection(ecu_core_ctx_t *ctx, void *userdata);

static const ecu_core_calcdata_timings_ctx_t ecu_core_calcdata_timings_ctx = {
    .timings = {
        {
            .func_read = calcdata_timing_read_ignition,
            .func_write = calcdata_timing_write_ignition,
            .userdata = NULL,
        }, //ECU_TIMING_TYPE_IGNITION
        {
            .func_read = calcdata_timing_read_injection,
            .func_write = calcdata_timing_write_injection,
            .userdata = NULL,
        }, //ECU_TIMING_TYPE_INJECTION
    },
};

void core_calcdata_timings_read(ecu_core_ctx_t *ctx)
{
  const ecu_core_calcdata_timing_ctx_t *timing_ctx;

  for(uint32_t type = 0; type < ECU_TIMING_TYPE_MAX; type++) {
    timing_ctx = &ecu_core_calcdata_timings_ctx.timings[type];

    if(timing_ctx->func_read != NULL) {
      timing_ctx->func_read(ctx, timing_ctx->userdata);
    }
  }
}

void core_calcdata_timings_write(ecu_core_ctx_t *ctx)
{
  const ecu_core_calcdata_timing_ctx_t *timing_ctx;

  for(uint32_t type = 0; type < ECU_TIMING_TYPE_MAX; type++) {
    timing_ctx = &ecu_core_calcdata_timings_ctx.timings[type];

    if(timing_ctx->func_write != NULL) {
      timing_ctx->func_write(ctx, timing_ctx->userdata);
    }
  }
}

static void calcdata_timing_read_ignition(ecu_core_ctx_t *ctx, void *userdata)
{
  ecu_core_runtime_global_parameters_timing_ignition_ctx_t *timing_ctx = &ctx->runtime.global.parameters.timings.ignition;
  ecu_core_runtime_global_parameters_timing_ignition_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.timings.ignition;
  ecu_core_runtime_global_ignition_ctx_t *dst_ctx = &ctx->runtime.global.ignition;

  //err = ecu_timings_ignition_get_data(&data);

  for(int i = 0; i < ECU_CONFIG_IGNITION_GROUP_MAX; i++) {
    for(ecu_cylinder_t cy = 0; cy < ECU_CYLINDER_MAX; cy++) {
      timing_ctx->read.groups[i].advance_cy[cy] = dst_ctx->groups[i].advance_cy[cy];
    }
    timing_ctx->read.groups[i].saturation_time = dst_ctx->groups[i].saturation_time;
  }
  timing_ctx->read_valid = true;

  if(simulated_ctx->read_valid) {
    timing_ctx->read = simulated_ctx->read;
  } else {
    simulated_ctx->read = timing_ctx->read;
  }
}

static void calcdata_timing_read_injection(ecu_core_ctx_t *ctx, void *userdata)
{
  ecu_core_runtime_global_parameters_timing_injection_ctx_t *timing_ctx = &ctx->runtime.global.parameters.timings.injection;
  ecu_core_runtime_global_parameters_timing_injection_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.timings.injection;
  ecu_core_runtime_global_injection_ctx_t *dst_ctx = &ctx->runtime.global.injection;

  for(int i = 0; i < ECU_CONFIG_INJECTION_GROUP_MAX; i++) {
    timing_ctx->read.groups[i].phase_mean = dst_ctx->groups[i].phase_mean;
    timing_ctx->read.groups[i].lag_time = dst_ctx->groups[i].lag_time;
    timing_ctx->read.groups[i].time_inject_mean = dst_ctx->groups[i].time_inject_mean;
    timing_ctx->read.groups[i].dutycycle_max = dst_ctx->groups[i].dutycycle_max;
    timing_ctx->read.groups[i].dutycycle_mean = dst_ctx->groups[i].dutycycle_mean;
    timing_ctx->read.groups[i].enrichment_late_phase = dst_ctx->groups[i].enrichment_late_phase;

    timing_ctx->read.groups[i].injector_input_pressure_mean = dst_ctx->groups[i].injector_input_pressure_mean;
    timing_ctx->read.groups[i].injector_output_pressure_mean = dst_ctx->groups[i].injector_output_pressure_mean;
    timing_ctx->read.groups[i].injector_pressure_diff_mean = dst_ctx->groups[i].injector_pressure_diff_mean;
  }
  timing_ctx->read_valid = true;

  if(simulated_ctx->read_valid) {
    timing_ctx->read = simulated_ctx->read;
  } else {
    simulated_ctx->read = timing_ctx->read;
  }
}

static void calcdata_timing_write_ignition(ecu_core_ctx_t *ctx, void *userdata)
{
  ecu_core_runtime_global_parameters_timing_ignition_ctx_t *timing_ctx = &ctx->runtime.global.parameters.timings.ignition;
  ecu_core_runtime_global_parameters_timing_ignition_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.timings.ignition;
  ecu_core_runtime_global_ignition_input_ctx_t *dst_ctx;


  if(simulated_ctx->write_valid) {
    memcpy(timing_ctx->write, simulated_ctx->write, sizeof(*timing_ctx->write));
    timing_ctx->write_valid = true;
  } else {
    if(timing_ctx->write_valid) {
      memcpy(simulated_ctx->write, timing_ctx->write, sizeof(*simulated_ctx->write));
    }
  }

  if(timing_ctx->write_valid) {
    for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) {
      dst_ctx = &ctx->runtime.global.ignition.input_banked[b];
      dst_ctx->allowed = timing_ctx->write[b].allowed;
      dst_ctx->ignition_advance = timing_ctx->write[b].ignition_advance;
    }
    ctx->runtime.global.ignition.input_valid = true;
    timing_ctx->write_valid = false;
  }
}

static void calcdata_timing_write_injection(ecu_core_ctx_t *ctx, void *userdata)
{
  ecu_core_runtime_global_parameters_timing_injection_ctx_t *timing_ctx = &ctx->runtime.global.parameters.timings.injection;
  ecu_core_runtime_global_parameters_timing_injection_ctx_t *simulated_ctx = &ctx->runtime.global.parameters_simulated.timings.injection;
  ecu_core_runtime_global_injection_input_ctx_t *dst_ctx;

  if(simulated_ctx->write_valid) {
    memcpy(timing_ctx->write, simulated_ctx->write, sizeof(*timing_ctx->write));
    timing_ctx->write_valid = true;
  } else {
    if(timing_ctx->write_valid) {
      memcpy(simulated_ctx->write, timing_ctx->write, sizeof(*simulated_ctx->write));
    }
  }

  if(timing_ctx->write_valid) {
    for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) {
      dst_ctx = &ctx->runtime.global.injection.input_banked[b];
      dst_ctx->allowed = timing_ctx->write[b].allowed;
      dst_ctx->injection_mass = timing_ctx->write[b].injection_mass;
      dst_ctx->injection_phase = timing_ctx->write[b].injection_phase;
    }
    ctx->runtime.global.injection.input_valid = true;
    timing_ctx->write_valid = false;
  }
}
