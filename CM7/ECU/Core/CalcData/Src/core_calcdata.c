/*
 * core_calcdata.c
 *
 *  Created on: Feb 16, 2025
 *      Author: VHEMaster
 */

#include "core_calcdata.h"
#include "config_global.h"

static void calcdata_read_sensor_aps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_ckp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_cmp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_ect(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_egt(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_iat(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_maf(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_map(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_ops(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_ots(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_tps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);
static void calcdata_read_sensor_vss(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result);

const ecu_core_calcdata_ctx_t ecu_core_calcdata_sensor_calc_ctx = {
    .sensors = {
        {
            .func = calcdata_read_sensor_ckp,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_CKP
        {
            .func = calcdata_read_sensor_cmp,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_CMP
        {
            .func = calcdata_read_sensor_ect,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_ECT
        {
            .func = calcdata_read_sensor_egt,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_EGT
        {
            .func = calcdata_read_sensor_iat,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_IAT
        {
            .func = calcdata_read_sensor_maf,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_MAF
        {
            .func = calcdata_read_sensor_map,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_MAP
        {
            .func = calcdata_read_sensor_tps,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_TPS
        {
            .func = calcdata_read_sensor_vss,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_VSS
        {
            .func = calcdata_read_sensor_aps,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_APS
        {
            .func = calcdata_read_sensor_ots,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_OTS
        {
            .func = calcdata_read_sensor_ops,
            .userdata = NULL,
        }, //ECU_SENSOR_TYPE_OPS
    },
};

static void core_calcdata_read_sensors(ecu_core_ctx_t *ctx)
{
  const ecu_config_io_t *io_config = &ctx->engine_config->calibration.io;
  const ecu_core_calcdata_sensors_ctx_t *sensor_ctx;
  ecu_sensor_instance_t instance_max;
  ecu_core_runtime_global_parameters_item_ctx_t result;

  for(uint32_t type = 0; type < ECU_SENSOR_TYPE_MAX; type++) {
    sensor_ctx = &ecu_core_calcdata_sensor_calc_ctx.sensors[type];
    instance_max = io_config->maxs.sensors[type];

    for(ecu_sensor_instance_t instance = 0; instance < instance_max; instance++) {
      memset(&result, 0, sizeof(result));
      if(sensor_ctx->func != NULL) {
        sensor_ctx->func(ctx, instance, sensor_ctx->userdata, &result);
      }
      ctx->runtime.global.parameters.sensors[type][instance] = result;
    }
  }

}

void core_calcdata_loop_slow(ecu_core_ctx_t *ctx)
{
  core_calcdata_read_sensors(ctx);
}

static void calcdata_read_sensor_aps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  aps_data_t data;

  err = ecu_sensors_aps_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.position;
}

static void calcdata_read_sensor_ckp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  ckp_data_t data;

  err = ecu_sensors_ckp_get_value(instance, &data);
  if(err == E_OK && data.validity == CKP_DATA_VALID) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.rpm;
}

static void calcdata_read_sensor_cmp(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  cmp_data_t data;

  err = ecu_sensors_cmp_get_value(instance, &data);
  if(err == E_OK && data.validity == CMP_DATA_VALID) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.position;
}

static void calcdata_read_sensor_ect(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  ect_data_t data;

  err = ecu_sensors_ect_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.temperature;
}

static void calcdata_read_sensor_egt(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  egt_data_t data;

  err = ecu_sensors_egt_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.sensor_temperature;
}

static void calcdata_read_sensor_iat(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  iat_data_t data;

  err = ecu_sensors_iat_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.temperature;
}

static void calcdata_read_sensor_maf(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  maf_data_t data;

  err = ecu_sensors_maf_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.mass_air_flow;
}

static void calcdata_read_sensor_map(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  map_data_t data;

  err = ecu_sensors_map_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.manifold_air_pressure;
}

static void calcdata_read_sensor_ops(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  ops_data_t data;

  err = ecu_sensors_ops_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.pressure;
}

static void calcdata_read_sensor_ots(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  ots_data_t data;

  err = ecu_sensors_ots_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.temperature;
}

static void calcdata_read_sensor_tps(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  tps_data_t data;

  err = ecu_sensors_tps_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.position;
}

static void calcdata_read_sensor_vss(ecu_core_ctx_t *ctx, ecu_sensor_instance_t instance, void *userdata, ecu_core_runtime_global_parameters_item_ctx_t *result)
{
  error_t err;
  vss_data_t data;

  err = ecu_sensors_vss_get_value(instance, &data);
  if(err == E_OK && data.valid) {
    result->valid = true;
  } else {
    result->valid = false;
  }
  result->value = data.speed;
}
