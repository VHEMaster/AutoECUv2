/*
 * calcdata_sensors.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "calcdata_sensors.h"
#include "config_global.h"

#define CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx) ((ctx)->runtime.global.parameters_virtual[ECU_CORE_RUNTIME_PARAMS_VIRT_SOURCE_INTERNAL])

static void calcdata_sensor_read_aps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_ckp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_cmp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_ect(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_egt(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_iat(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_maf(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_map(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_ops(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_ots(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_tps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_read_vss(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);

static void calcdata_sensor_invalidate_aps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_ckp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_cmp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_ect(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_egt(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_iat(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_maf(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_map(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_ops(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_ots(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_tps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);
static void calcdata_sensor_invalidate_vss(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata);

static const ecu_core_calcdata_sensors_ctx_t ecu_core_calcdata_sensors_ctx = {
    .sensors = {
        {
            .max = ECU_SENSOR_CKP_MAX,
            .func_read = calcdata_sensor_read_ckp,
            .func_invalidate = calcdata_sensor_invalidate_ckp,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_CKP
        {
            .max = ECU_SENSOR_CMP_MAX,
            .func_read = calcdata_sensor_read_cmp,
            .func_invalidate = calcdata_sensor_invalidate_cmp,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_CMP
        {
            .max = ECU_SENSOR_ECT_MAX,
            .func_read = calcdata_sensor_read_ect,
            .func_invalidate = calcdata_sensor_invalidate_ect,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_ECT
        {
            .max = ECU_SENSOR_EGT_MAX,
            .func_read = calcdata_sensor_read_egt,
            .func_invalidate = calcdata_sensor_invalidate_egt,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_EGT
        {
            .max = ECU_SENSOR_IAT_MAX,
            .func_read = calcdata_sensor_read_iat,
            .func_invalidate = calcdata_sensor_invalidate_iat,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_IAT
        {
            .max = ECU_SENSOR_MAF_MAX,
            .func_read = calcdata_sensor_read_maf,
            .func_invalidate = calcdata_sensor_invalidate_maf,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_MAF
        {
            .max = ECU_SENSOR_MAP_MAX,
            .func_read = calcdata_sensor_read_map,
            .func_invalidate = calcdata_sensor_invalidate_map,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_MAP
        {
            .max = ECU_SENSOR_TPS_MAX,
            .func_read = calcdata_sensor_read_tps,
            .func_invalidate = calcdata_sensor_invalidate_tps,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_TPS
        {
            .max = ECU_SENSOR_VSS_MAX,
            .func_read = calcdata_sensor_read_vss,
            .func_invalidate = calcdata_sensor_invalidate_vss,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_VSS
        {
            .max = ECU_SENSOR_APS_MAX,
            .func_read = calcdata_sensor_read_aps,
            .func_invalidate = calcdata_sensor_invalidate_aps,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_APS
        {
            .max = ECU_SENSOR_OTS_MAX,
            .func_read = calcdata_sensor_read_ots,
            .func_invalidate = calcdata_sensor_invalidate_ots,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_OTS
        {
            .max = ECU_SENSOR_OPS_MAX,
            .func_read = calcdata_sensor_read_ops,
            .func_invalidate = calcdata_sensor_invalidate_ops,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_OPS
    },
};

void core_calcdata_sensors_read(ecu_core_ctx_t *ctx)
{
  error_t err;
  bool enabled;
  const ecu_core_calcdata_sensor_ctx_t *sensor_ctx;
  ecu_sensor_instance_t instance_max;

  for(uint32_t type = 0; type < ECU_SENSOR_TYPE_MAX; type++) {
    sensor_ctx = &ecu_core_calcdata_sensors_ctx.sensors[type];
    instance_max = MIN(sensor_ctx->max, ECU_SENSOR_INSTANCE_MAX);

    if(sensor_ctx->func_read != NULL) {
      for(ecu_sensor_instance_t instance = 0; instance < instance_max; instance++) {
        err = ecu_sensors_get_sensor_enabled(type, instance, &enabled);
        if(err == E_OK && enabled) {
          sensor_ctx->func_read(ctx, instance, sensor_ctx->userdata);
        } else if(sensor_ctx->func_invalidate != NULL) {
          sensor_ctx->func_invalidate(ctx, instance, sensor_ctx->userdata);
        }
      }
    }
  }
}

static void calcdata_sensor_read_aps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  aps_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_APS][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_aps_get_value(instance, &data);
  if(err == E_OK && data.data_valid) {
    param_index = ECU_SENSOR_APS_READ_PARAM_PEDAL_POSITION;
    params_rd_ctx[param_index].value = data.position;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_APS_READ_PARAM_PEDAL_POSITION_UNFILTERED;
    params_rd_ctx[param_index].value = data.position_unfiltered;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_APS_READ_PARAM_PEDAL_POSITION_RAW_A;
    params_rd_ctx[param_index].value = data.positions_raw[0];
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_APS_READ_PARAM_PEDAL_POSITION_RAW_B;
    params_rd_ctx[param_index].value = data.positions_raw[1];
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_APS_READ_PARAM_PEDAL_VOLTAGE_A;
    params_rd_ctx[param_index].value = data.voltages[0];
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_APS_READ_PARAM_PEDAL_VOLTAGE_B;
    params_rd_ctx[param_index].value = data.voltages[1];
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_APS_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.position_valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;

  } else {
    for(param_index = 0; param_index < ECU_SENSOR_APS_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_ckp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  ckp_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_CKP][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_ckp_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_CKP_READ_PARAM_RPM;
    params_rd_ctx[param_index].value = data.rpm;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_CKP_READ_PARAM_INSYNC;
    params_rd_ctx[param_index].value = data.validity >= CKP_DATA_VALID ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_CKP_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_cmp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  cmp_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_CMP][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_cmp_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_CMP_READ_PARAM_CAMSHAFT_ABS_POSITION;
    params_rd_ctx[param_index].value = data.position_absolute;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_CMP_READ_PARAM_CAMSHAFT_REL_POSITION;
    params_rd_ctx[param_index].value = data.position_relative;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_CKP_READ_PARAM_INSYNC;
    params_rd_ctx[param_index].value = data.validity >= CMP_DATA_VALID ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_CMP_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_ect(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  ect_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_ECT][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index = ECU_SENSOR_ECT_READ_PARAM_TEMPERATURE;

  err = ecu_sensors_ect_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_ECT_READ_PARAM_TEMPERATURE;
    params_rd_ctx[param_index].value = data.temperature;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_ECT_READ_PARAM_INPUT_VALUE;
    params_rd_ctx[param_index].value = data.input_value;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_ECT_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_ECT_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_egt(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  egt_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_EGT][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_egt_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_EGT_READ_PARAM_TEMPERATURE;
    params_rd_ctx[param_index].value = data.sensor_temperature;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_EGT_READ_PARAM_COLD_JUNCTION;
    params_rd_ctx[param_index].value = data.reference_temperature;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_EGT_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_ECT_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_iat(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  iat_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_IAT][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_iat_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_IAT_READ_PARAM_TEMPERATURE;
    params_rd_ctx[param_index].value = data.temperature;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_IAT_READ_PARAM_INPUT_VALUE;
    params_rd_ctx[param_index].value = data.input_value;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_IAT_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_ECT_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_maf(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  maf_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_MAF][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_maf_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_MAF_READ_PARAM_MASSAIRFLOW;
    params_rd_ctx[param_index].value = data.mass_air_flow;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_MAF_READ_PARAM_INPUT_VALUE;
    params_rd_ctx[param_index].value = data.input_value;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_MAF_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_ECT_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_map(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  map_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_MAP][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_map_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_MAP_READ_PARAM_PRESSURE;
    params_rd_ctx[param_index].value = data.pressure;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_MAP_READ_PARAM_INPUT_VALUE;
    params_rd_ctx[param_index].value = data.input_value;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_MAP_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_ECT_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_ops(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  ops_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_OPS][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_ops_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_OPS_READ_PARAM_PRESSURE;
    params_rd_ctx[param_index].value = data.pressure;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_OPS_READ_PARAM_INPUT_VALUE;
    params_rd_ctx[param_index].value = data.input_value;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_OPS_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_ECT_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_ots(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  ots_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_OTS][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_ots_get_value(instance, &data);
  if(err == E_OK) {
    param_index = ECU_SENSOR_OTS_READ_PARAM_TEMPERATURE;
    params_rd_ctx[param_index].value = data.temperature;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_OTS_READ_PARAM_INPUT_VALUE;
    params_rd_ctx[param_index].value = data.input_value;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_OTS_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;
  } else {
    for(param_index = 0; param_index < ECU_SENSOR_ECT_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_tps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  tps_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_TPS][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index;

  err = ecu_sensors_tps_get_value(instance, &data);
  if(err == E_OK && data.data_valid) {
    param_index = ECU_SENSOR_TPS_READ_PARAM_THROTTLE_POSITION;
    params_rd_ctx[param_index].value = data.position;
    params_rd_ctx[param_index].valid = data.position_valid;

    param_index = ECU_SENSOR_TPS_READ_PARAM_THROTTLE_POSITION_UNFILTERED;
    params_rd_ctx[param_index].value = data.position_unfiltered;
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_TPS_READ_PARAM_THROTTLE_POSITION_RAW_A;
    params_rd_ctx[param_index].value = data.positions_raw[0];
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_TPS_READ_PARAM_THROTTLE_POSITION_RAW_B;
    params_rd_ctx[param_index].value = data.positions_raw[1];
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_TPS_READ_PARAM_THROTTLE_VOLTAGE_A;
    params_rd_ctx[param_index].value = data.voltages[0];
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_TPS_READ_PARAM_THROTTLE_VOLTAGE_B;
    params_rd_ctx[param_index].value = data.voltages[1];
    params_rd_ctx[param_index].valid = true;

    param_index = ECU_SENSOR_TPS_READ_PARAM_TRUSTED;
    params_rd_ctx[param_index].value = data.position_valid ? ECU_RUNTIME_PARAMETER_TRUE : ECU_RUNTIME_PARAMETER_FALSE;
    params_rd_ctx[param_index].valid = true;

  } else {
    for(param_index = 0; param_index < ECU_SENSOR_APS_READ_PARAM_MAX; param_index++) {
      params_rd_ctx[param_index].valid = false;
    }
  }
}

static void calcdata_sensor_read_vss(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  error_t err;
  vss_data_t data;
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_VSS][instance].params[ECU_COMMON_READ];
  ecu_runtime_param_index_t param_index = ECU_SENSOR_VSS_READ_PARAM_DATA;

  err = ecu_sensors_vss_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    params_rd_ctx[param_index].valid = true;
  } else {
    params_rd_ctx[param_index].valid = false;
  }
  params_rd_ctx[param_index].value = data.speed;
}

static void calcdata_sensor_invalidate_aps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_APS][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_APS_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_ckp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_CKP][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_CKP_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_cmp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_CMP][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_CMP_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_ect(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_ECT][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_ECT_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_egt(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_EGT][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_EGT_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_iat(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_IAT][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_IAT_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_maf(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_MAF][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_MAF_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_map(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_MAP][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_MAP_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_ops(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_OPS][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_OPS_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_ots(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_OTS][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_OTS_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_tps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_TPS][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_TPS_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}

static void calcdata_sensor_invalidate_vss(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata)
{
  ecu_core_runtime_value_ctx_t *params_rd_ctx = CALCDATA_GLOBAL_PARAMETERS_VIRTUAL_INTERNAL(ctx).sensors[ECU_SENSOR_TYPE_VSS][instance].params[ECU_COMMON_READ];

  for(ecu_runtime_param_index_t param_index = 0; param_index < ECU_SENSOR_VSS_READ_PARAM_MAX; param_index++) {
    params_rd_ctx[param_index].valid = false;
  }
}
