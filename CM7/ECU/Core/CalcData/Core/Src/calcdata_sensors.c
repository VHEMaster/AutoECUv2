/*
 * calcdata_sensors.c
 *
 *  Created on: May 17, 2025
 *      Author: VHEMaster
 */

#include "calcdata_sensors.h"
#include "config_global.h"

static void calcdata_sensor_read_aps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_ckp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_cmp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_ect(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_egt(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_iat(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_maf(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_map(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_ops(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_ots(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_tps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);
static void calcdata_sensor_read_vss(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result);

static const ecu_core_calcdata_sensors_ctx_t ecu_core_calcdata_sensors_ctx = {
    .sensors = {
        {
            .max = ECU_SENSOR_CKP_MAX,
            .func_read = calcdata_sensor_read_ckp,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_CKP
        {
            .max = ECU_SENSOR_CMP_MAX,
            .func_read = calcdata_sensor_read_cmp,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_CMP
        {
            .max = ECU_SENSOR_ECT_MAX,
            .func_read = calcdata_sensor_read_ect,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_ECT
        {
            .max = ECU_SENSOR_EGT_MAX,
            .func_read = calcdata_sensor_read_egt,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_EGT
        {
            .max = ECU_SENSOR_IAT_MAX,
            .func_read = calcdata_sensor_read_iat,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_IAT
        {
            .max = ECU_SENSOR_MAF_MAX,
            .func_read = calcdata_sensor_read_maf,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_MAF
        {
            .max = ECU_SENSOR_MAP_MAX,
            .func_read = calcdata_sensor_read_map,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_MAP
        {
            .max = ECU_SENSOR_TPS_MAX,
            .func_read = calcdata_sensor_read_tps,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_TPS
        {
            .max = ECU_SENSOR_VSS_MAX,
            .func_read = calcdata_sensor_read_vss,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_VSS
        {
            .max = ECU_SENSOR_APS_MAX,
            .func_read = calcdata_sensor_read_aps,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_APS
        {
            .max = ECU_SENSOR_OTS_MAX,
            .func_read = calcdata_sensor_read_ots,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_OTS
        {
            .max = ECU_SENSOR_OPS_MAX,
            .func_read = calcdata_sensor_read_ops,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_OPS
    },
};

void core_calcdata_sensors_read(ecu_core_ctx_t *ctx)
{
  const ecu_core_calcdata_sensor_ctx_t *sensor_ctx;
  ecu_sensor_instance_t instance_max;
  ecu_core_runtime_global_parameters_sensor_value_ctx_t result;
  ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensor_value_ctx;
  ecu_core_runtime_global_parameters_sensor_value_ctx_t *sensor_value_simulated_ctx;

  for(uint32_t type = 0; type < ECU_SENSOR_TYPE_MAX; type++) {
    sensor_ctx = &ecu_core_calcdata_sensors_ctx.sensors[type];
    instance_max = MIN(sensor_ctx->max, ECU_SENSOR_INSTANCE_MAX);

    if(sensor_ctx->func_read != NULL) {
      for(ecu_sensor_instance_t instance = 0; instance < instance_max; instance++) {
        memset(&result, 0, sizeof(result));
        sensor_ctx->func_read(ctx, instance, sensor_ctx->userdata, &result);
        sensor_value_ctx = &ctx->runtime.global.parameters.sensors[type][instance];
        sensor_value_simulated_ctx = &ctx->runtime.global.parameters_simulated.sensors[type][instance];

        if(sensor_value_simulated_ctx->read_valid) {
          *sensor_value_ctx = *sensor_value_simulated_ctx;
        } else {
          *sensor_value_ctx = result;
          sensor_value_simulated_ctx->value = result.value;

        }
      }
    } else {
      memset(&ctx->runtime.global.parameters.sensors[type], 0, sizeof(ctx->runtime.global.parameters.sensors[type]));
    }
  }
}

static void calcdata_sensor_read_aps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  aps_data_t data;

  err = ecu_sensors_aps_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.position;
}

static void calcdata_sensor_read_ckp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  ckp_data_t data;

  err = ecu_sensors_ckp_get_value(instance, &data);
  if(err == E_OK && data.validity == CKP_DATA_VALID) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.rpm;
}

static void calcdata_sensor_read_cmp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  cmp_data_t data;

  err = ecu_sensors_cmp_get_value(instance, &data);
  if(err == E_OK && data.validity == CMP_DATA_VALID) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.position;
}

static void calcdata_sensor_read_ect(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  ect_data_t data;

  err = ecu_sensors_ect_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.temperature;
}

static void calcdata_sensor_read_egt(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  egt_data_t data;

  err = ecu_sensors_egt_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.sensor_temperature;
}

static void calcdata_sensor_read_iat(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  iat_data_t data;

  err = ecu_sensors_iat_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.temperature;
}

static void calcdata_sensor_read_maf(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  maf_data_t data;

  err = ecu_sensors_maf_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.mass_air_flow;
}

static void calcdata_sensor_read_map(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  map_data_t data;

  err = ecu_sensors_map_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.manifold_air_pressure;
}

static void calcdata_sensor_read_ops(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  ops_data_t data;

  err = ecu_sensors_ops_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.pressure;
}

static void calcdata_sensor_read_ots(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  ots_data_t data;

  err = ecu_sensors_ots_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.temperature;
}

static void calcdata_sensor_read_tps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  tps_data_t data;

  err = ecu_sensors_tps_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.position;
}

static void calcdata_sensor_read_vss(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_sensor_value_ctx_t *result)
{
  error_t err;
  vss_data_t data;

  err = ecu_sensors_vss_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->read_valid = true;
  } else {
    result->read_valid = false;
  }
  result->value = data.speed;
}
