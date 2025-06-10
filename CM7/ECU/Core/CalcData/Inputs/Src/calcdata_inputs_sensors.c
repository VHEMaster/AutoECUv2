/*
 * calcdata_inputs_sensors.c
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#include "calcdata_inputs_sensors.h"
#include "config_global.h"

#include "math_fast.h"

static void core_calcdata_inputs_process_sensors_global(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_banked(ecu_core_ctx_t *ctx, ecu_bank_t bank);

static void core_calcdata_inputs_process_sensors_global_aps(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_global_ckp(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_global_ect(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_global_ops(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_global_ots(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_global_vss(ecu_core_ctx_t *ctx, ecu_bank_t bank);

static void core_calcdata_inputs_process_sensors_banked_cmp(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_banked_egt(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_banked_iat(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_banked_maf(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_banked_map(ecu_core_ctx_t *ctx, ecu_bank_t bank);
static void core_calcdata_inputs_process_sensors_banked_tps(ecu_core_ctx_t *ctx, ecu_bank_t bank);

void core_calcdata_inputs_sensors_process(ecu_core_ctx_t *ctx)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;

  for(ecu_bank_t bank = 0; bank < banks_count; bank++) {
    core_calcdata_inputs_process_sensors_global(ctx, bank);
    core_calcdata_inputs_process_sensors_banked(ctx, bank);
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_global(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  core_calcdata_inputs_process_sensors_global_aps(ctx, bank);
  core_calcdata_inputs_process_sensors_global_ckp(ctx, bank);
  core_calcdata_inputs_process_sensors_global_ect(ctx, bank);
  core_calcdata_inputs_process_sensors_global_ops(ctx, bank);
  core_calcdata_inputs_process_sensors_global_ots(ctx, bank);
  core_calcdata_inputs_process_sensors_global_vss(ctx, bank);
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_banked(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  core_calcdata_inputs_process_sensors_banked_cmp(ctx, bank);
  core_calcdata_inputs_process_sensors_banked_egt(ctx, bank);
  core_calcdata_inputs_process_sensors_banked_iat(ctx, bank);
  core_calcdata_inputs_process_sensors_banked_maf(ctx, bank);
  core_calcdata_inputs_process_sensors_banked_map(ctx, bank);
  core_calcdata_inputs_process_sensors_banked_tps(ctx, bank);
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_global_aps(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_APS_ACCELERATOR;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_APS;

  src = ctx->runtime.banked.raw.global.sensors_aps[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_global_ckp(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_CKP_PRIMARY;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_CKP;

  src = ctx->runtime.banked.raw.global.sensors_ckp[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_global_ect(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;
  ecu_core_runtime_value_ctx_t dest_value;
  uint32_t value_count;

  ecu_config_io_instance_t instances_count = ECU_CONFIG_IO_ECT_MAX;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_ECT;
  ecu_config_calcdata_setup_ect_calc_model_t calc_model = ctx->calibration->calcdata.setup.ect_model.calc_model;
  float rms_min = ctx->calibration->calcdata.setup.ect_model.rms_min;
  float tempval;

  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  memset(&dest_value, 0, sizeof(dest_value));
  value_count = 0;

  for(ecu_config_io_ect_t instance = 0; instance < instances_count; instance++) {
    src = ctx->runtime.banked.raw.global.sensors_ect[instance];
    if(src != NULL) {
      if(src->valid) {
        dest_value.valid = true;
        if(calc_model == CALCDATA_ECT_CALC_MODEL_MAXIMAL) {
          if(dest_value.value > src->value || value_count == 0) {
            dest_value.value = src->value;
          }
        } else if(calc_model == CALCDATA_ECT_CALC_MODEL_MEAN) {
          dest_value.value += src->value;
        } else if(calc_model == CALCDATA_ECT_CALC_MODEL_RMS) {
          tempval = src->value - rms_min;
          if(tempval > 0.0f) {
            dest_value.value += tempval * tempval;
          }
        }
        value_count++;
      }
    }
  }

  if(value_count) {
    if(calc_model == CALCDATA_ECT_CALC_MODEL_MEAN) {
      dest_value.value /= value_count;
    } else if(calc_model == CALCDATA_ECT_CALC_MODEL_RMS) {
      dest_value.value /= value_count;
      dest_value.value = fast_sqrt(dest_value.value);
      dest_value.value += rms_min;
    }
  }
  *dest_ptr = dest_value;
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_global_ops(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_OPS_PRIMARY;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_OPS;

  src = ctx->runtime.banked.raw.global.sensors_ops[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_global_ots(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_OTS_PRIMARY;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_OTS;

  src = ctx->runtime.banked.raw.global.sensors_ots[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_global_vss(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_VSS_PRIMARY;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_GLOBAL_VSS;

  src = ctx->runtime.banked.raw.global.sensors_vss[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_banked_cmp(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t **src_array;
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;
  ecu_config_calcdata_relation_input_source_index_t dest_index;

  ecu_config_io_instance_t instance;

  src_array = ctx->runtime.banked.raw.banks[bank].sensors_cmp;

  instance = ECU_CONFIG_IO_CMP_SOHC;
  src = src_array[instance];
  if(src != NULL) {
    for(dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_CMP_IN;
        dest_index <= CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_CMP_EX; dest_index++) {
      dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
      memcpy(dest_ptr, src, sizeof(*dest_ptr));
    }
  } else {
    dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_CMP_IN;
    dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
    instance = ECU_CONFIG_IO_CMP_DOHC_INTAKE;
    src = src_array[instance];
    if(src != NULL) {
      memcpy(dest_ptr, src, sizeof(*dest_ptr));
    } else {
      memset(dest_ptr, 0, sizeof(*dest_ptr));
    }

    dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_CMP_EX;
    dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
    instance = ECU_CONFIG_IO_CMP_DOHC_EXHAUST;
    src = src_array[instance];
    if(src != NULL) {
      memcpy(dest_ptr, src, sizeof(*dest_ptr));
    } else {
      memset(dest_ptr, 0, sizeof(*dest_ptr));
    }

  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_banked_egt(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_EGT_PRIMARY;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_EGT;

  src = ctx->runtime.banked.raw.banks[bank].sensors_egt[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_banked_iat(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;
  ecu_config_io_iat_t type;

  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_IAT;

  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;

  for(type = 0; type < ECU_CONFIG_IO_IAT_MAX; type++) {
    src = ctx->runtime.banked.raw.banks[bank].sensors_iat[type];
    if(src != NULL && src->valid) {
      ctx->runtime.banked.source.banks[bank].data_iat.active_type = type;
      src = ctx->runtime.banked.raw.banks[bank].sensors_iat[type];
      break;
    }
  }

  if(type < ECU_CONFIG_IO_IAT_MAX && src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_banked_maf(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const uint32_t banks_count = ctx->runtime.global.banks_count;
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_MAF_PRIMARY;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAF;
  ecu_sensor_maf_t maf_current;
  uint32_t banks_per_maf = 0;

  maf_current = ctx->calibration->io.banked.global.common.sensor_maf[instance];
  if(maf_current < ECU_SENSOR_MAF_MAX) {
    banks_per_maf = banks_count;
  } else {
    maf_current = ctx->calibration->io.banked.banks[bank].common.sensor_maf[instance];
    if(maf_current < ECU_SENSOR_MAF_MAX) {
      for(ecu_bank_t b = 0; b < banks_count; b++) {
        if(ctx->calibration->io.banked.banks[b].common.sensor_maf[instance] == maf_current) {
          banks_per_maf++;
        }
      }
    }
  }

  if(banks_per_maf <= 0) {
    banks_per_maf = 1;
  }

  src = ctx->runtime.banked.raw.banks[bank].sensors_maf[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    dest_ptr->value = src->value / banks_per_maf;
    dest_ptr->valid = src->valid;
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_banked_map(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_MAP_MANIFOLD;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_MAP;

  src = ctx->runtime.banked.raw.banks[bank].sensors_map[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}

STATIC_INLINE void core_calcdata_inputs_process_sensors_banked_tps(ecu_core_ctx_t *ctx, ecu_bank_t bank)
{
  const ecu_core_runtime_value_ctx_t *src;
  ecu_core_runtime_value_ctx_t *dest_ptr;

  ecu_config_io_instance_t instance = ECU_CONFIG_IO_TPS_PRIMARY;
  ecu_config_calcdata_relation_input_source_index_t dest_index = CALCDATA_RELATION_INPUT_SOURCE_SENSOR_BANKED_TPS;

  src = ctx->runtime.banked.raw.banks[bank].sensors_tps[instance];
  dest_ptr = &ctx->runtime.banked.source.banks[bank].inputs[dest_index].value;
  if(src != NULL) {
    memcpy(dest_ptr, src, sizeof(*dest_ptr));
  } else {
    memset(dest_ptr, 0, sizeof(*dest_ptr));
  }
}
