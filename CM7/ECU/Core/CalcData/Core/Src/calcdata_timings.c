/*
 * calcdata_timings.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "calcdata_timings.h"
#include "config_global.h"

#define CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx) ((ctx)->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_INTERNAL])

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
  ecu_core_runtime_global_instance_parameters_ctx_t *timing_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).timings[ECU_TIMING_TYPE_IGNITION];
  ecu_core_runtime_global_ignition_ctx_t *dst_ctx = &ctx->runtime.global.ignition;
  ecu_timing_ignition_read_params_t param_index;
  ecu_timing_ignition_read_params_t param_index_base_gr;
  ecu_timing_ignition_read_params_t param_index_base_gr_cy;

  for(int i = 0; i < ECU_CONFIG_IGNITION_GROUP_MAX; i++) {
    param_index_base_gr = ECU_TIMING_IGNITION_READ_PARAM_GR1_START + ((ECU_TIMING_IGNITION_READ_PARAM_GR1_END - ECU_TIMING_IGNITION_READ_PARAM_GR1_START + 1) * i);
    for(ecu_cylinder_t cy = 0; cy < ECU_CYLINDER_MAX; cy++) {
      param_index_base_gr_cy = param_index_base_gr + ECU_TIMING_IGNITION_READ_PARAM_GR1_CY1_START + ((ECU_TIMING_IGNITION_READ_PARAM_GR1_CY1_END - ECU_TIMING_IGNITION_READ_PARAM_GR1_CY1_START + 1) * cy);

      param_index = param_index_base_gr_cy + ECU_TIMING_IGNITION_READ_PARAM_GR1_CY1_ADVANCE - ECU_TIMING_IGNITION_READ_PARAM_GR1_CY1_START;
      timing_ctx->read[param_index].value = dst_ctx->groups[i].advance_cy[cy];
      timing_ctx->read[param_index].valid = true;
    }
    param_index = param_index_base_gr + ECU_TIMING_IGNITION_READ_PARAM_GR1_SATURATION_TIME - ECU_TIMING_IGNITION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].saturation_time;
    timing_ctx->read[param_index].valid = true;
  }
}

static void calcdata_timing_read_injection(ecu_core_ctx_t *ctx, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *timing_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).timings[ECU_TIMING_TYPE_INJECTION];
  ecu_core_runtime_global_injection_ctx_t *dst_ctx = &ctx->runtime.global.injection;
  ecu_timing_injection_write_params_t param_index;
  ecu_timing_injection_write_params_t param_index_base;

  for(int i = 0; i < ECU_CONFIG_INJECTION_GROUP_MAX; i++) {
    param_index_base = ECU_TIMING_INJECTION_READ_PARAM_GR1_START + ((ECU_TIMING_INJECTION_READ_PARAM_GR1_END - ECU_TIMING_INJECTION_READ_PARAM_GR1_START + 1) * i);

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_PHASE_MEAN - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].phase_mean;
    timing_ctx->read[param_index].valid = true;

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_LAG_TIME - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].lag_time;
    timing_ctx->read[param_index].valid = true;

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_TIME_INJECT_MEAN - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].time_inject_mean;
    timing_ctx->read[param_index].valid = true;

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_DUTYCYCLE_MAX - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].dutycycle_max;
    timing_ctx->read[param_index].valid = true;

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_DUTYCYCLE_MEAN - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].dutycycle_mean;
    timing_ctx->read[param_index].valid = true;

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_ENRICHMENT_LATE_PHASE - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].enrichment_late_phase;
    timing_ctx->read[param_index].valid = true;

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_INJECTOR_INPUT_PRESSURE_MEAN - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].injector_input_pressure_mean;
    timing_ctx->read[param_index].valid = true;

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_INJECTOR_OUTPUT_PRESSURE_MEAN - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].injector_output_pressure_mean;
    timing_ctx->read[param_index].valid = true;

    param_index = param_index_base + ECU_TIMING_INJECTION_READ_PARAM_GR1_INJECTOR_PRESSURE_DIFF_MEAN - ECU_TIMING_INJECTION_READ_PARAM_GR1_START;
    timing_ctx->read[param_index].value = dst_ctx->groups[i].injector_pressure_diff_mean;
    timing_ctx->read[param_index].valid = true;
  }
}

static void calcdata_timing_write_ignition(ecu_core_ctx_t *ctx, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *timing_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).timings[ECU_TIMING_TYPE_IGNITION];
  ecu_core_runtime_global_ignition_input_ctx_t *dst_ctx;
  ecu_timing_ignition_write_params_t param_index;
  ecu_timing_ignition_write_params_t param_index_base;

  for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) {
    dst_ctx = &ctx->runtime.global.ignition.input_banked[b];
    param_index_base = ECU_TIMING_IGNITION_WRITE_PARAM_B1_START + ((ECU_TIMING_IGNITION_WRITE_PARAM_B1_END - ECU_TIMING_IGNITION_WRITE_PARAM_B1_START + 1) * b);

    param_index = param_index_base + ECU_TIMING_IGNITION_WRITE_PARAM_B1_ALLOWED - ECU_TIMING_IGNITION_WRITE_PARAM_B1_START;
    if(timing_ctx->write[param_index].valid) {
      dst_ctx->allowed.value = timing_ctx->write[param_index].value;
      dst_ctx->allowed.valid = true;
      timing_ctx->write[param_index].valid = false;
    }
    param_index = param_index_base + ECU_TIMING_IGNITION_WRITE_PARAM_B1_ADVANCE - ECU_TIMING_IGNITION_WRITE_PARAM_B1_START;
    if(timing_ctx->write[param_index].valid) {
      dst_ctx->ignition_advance.value = timing_ctx->write[param_index].value;
      dst_ctx->ignition_advance.valid = true;
      timing_ctx->write[param_index].valid = false;
    }
  }
}

static void calcdata_timing_write_injection(ecu_core_ctx_t *ctx, void *userdata)
{
  ecu_core_runtime_global_instance_parameters_ctx_t *timing_ctx = &CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).timings[ECU_TIMING_TYPE_INJECTION];
  ecu_core_runtime_global_injection_input_ctx_t *dst_ctx;
  ecu_timing_injection_write_params_t param_index;
  ecu_timing_injection_write_params_t param_index_base;

  for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) {
    dst_ctx = &ctx->runtime.global.injection.input_banked[b];
    param_index_base = ECU_TIMING_INJECTION_WRITE_PARAM_B1_START + ((ECU_TIMING_INJECTION_WRITE_PARAM_B1_END - ECU_TIMING_INJECTION_WRITE_PARAM_B1_START + 1) * b);

    param_index = param_index_base + ECU_TIMING_INJECTION_WRITE_PARAM_B1_ALLOWED - ECU_TIMING_INJECTION_WRITE_PARAM_B1_START;
    if(timing_ctx->write[param_index].valid) {
      dst_ctx->allowed.value = timing_ctx->write[param_index].value;
      dst_ctx->allowed.valid = true;
      timing_ctx->write[param_index].valid = false;
    }

    param_index = param_index_base + ECU_TIMING_INJECTION_WRITE_PARAM_B1_INJECTION_MASS - ECU_TIMING_INJECTION_WRITE_PARAM_B1_START;
    if(timing_ctx->write[param_index].valid) {
      dst_ctx->injection_mass.value = timing_ctx->write[param_index].value;
      dst_ctx->injection_mass.valid = true;
      timing_ctx->write[param_index].valid = false;
    }

    param_index = param_index_base + ECU_TIMING_INJECTION_WRITE_PARAM_B1_INJECTION_PHASE - ECU_TIMING_INJECTION_WRITE_PARAM_B1_START;
    if(timing_ctx->write[param_index].valid) {
      dst_ctx->injection_phase.value = timing_ctx->write[param_index].value;
      dst_ctx->injection_phase.valid = true;
      timing_ctx->write[param_index].valid = false;
    }
  }
}
