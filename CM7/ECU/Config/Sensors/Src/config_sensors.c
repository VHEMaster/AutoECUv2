/*
 * config_sensors.c
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#include "config_sensors.h"
#include "config_global.h"
#include "compiler.h"
#include "bool.h"

#define ECU_SENSORS_MAX (     \
    ECU_SENSOR_CKP_MAX +      \
    ECU_SENSOR_CMP_MAX +      \
    ECU_SENSOR_ECT_MAX +      \
    ECU_SENSOR_EGT_MAX +      \
    ECU_SENSOR_IAT_MAX +      \
    ECU_SENSOR_MAF_MAX +      \
    ECU_SENSOR_MAP_MAX +      \
    ECU_SENSOR_TPS_MAX +      \
    ECU_SENSOR_VSS_MAX +      \
    ECU_SENSOR_APS_MAX +      \
    ECU_SENSOR_OTS_MAX +      \
    ECU_SENSOR_OPS_MAX)

typedef enum {
  ECU_SENSOR_LOOP_TYPE_MAIN = 0,
  ECU_SENSOR_LOOP_TYPE_SLOW,
  ECU_SENSOR_LOOP_TYPE_FAST
}ecu_config_sensor_loop_type_t;

typedef struct {
    bool initialized;
    bool enabled;
}ecu_config_sensor_instance_ctx_t;

typedef struct {
    ecu_sensor_type_t type;
    ecu_sensor_instance_t instance;
    void *ctx;
    bool initialized;
    bool enabled;

    ecu_core_runtime_value_ctx_t *params_read_ptr;
    ecu_core_runtime_value_ctx_t *params_write_ptr;
}ecu_config_sensor_config_ctx_t;

typedef struct {
    ecu_sensor_loop_func_t loop_main;
    ecu_sensor_loop_func_t loop_slow;
    ecu_sensor_loop_func_t loop_fast;
    ecu_sensor_instance_t instance_max;
    ecu_runtime_param_index_t params_read_count;
    ecu_runtime_param_index_t params_write_count;
}ecu_config_sensor_if_config_ctx_t;

typedef struct {
    uint32_t instance_first;
}ecu_config_sensor_if_instance_ctx_t;


typedef struct {
  ecu_config_sensor_if_config_ctx_t interfaces[ECU_SENSOR_TYPE_MAX];
  ecu_config_sensor_config_ctx_t sensors[ECU_SENSORS_MAX];
}ecu_config_sensors_config_t;

typedef struct {
    const ecu_config_sensors_config_t *config;
    ecu_config_sensor_if_instance_ctx_t interfaces[ECU_SENSOR_TYPE_MAX];
    ecu_config_sensor_instance_ctx_t sensors[ECU_SENSORS_MAX];
}ecu_config_sensors_t;

static ckp_ctx_t ecu_config_ckp_ctx[ECU_SENSOR_CKP_MAX] = {0};
static cmp_ctx_t ecu_config_cmp_ctx[ECU_SENSOR_CMP_MAX] = {0};
static ect_ctx_t ecu_config_ect_ctx[ECU_SENSOR_ECT_MAX] = {0};
static egt_ctx_t ecu_config_egt_ctx[ECU_SENSOR_EGT_MAX] = {0};
static iat_ctx_t ecu_config_iat_ctx[ECU_SENSOR_IAT_MAX] = {0};
static maf_ctx_t ecu_config_maf_ctx[ECU_SENSOR_MAF_MAX] = {0};
static map_ctx_t ecu_config_map_ctx[ECU_SENSOR_MAP_MAX] = {0};
static tps_ctx_t ecu_config_tps_ctx[ECU_SENSOR_TPS_MAX] = {0};
static vss_ctx_t ecu_config_vss_ctx[ECU_SENSOR_VSS_MAX] = {0};
static aps_ctx_t ecu_config_aps_ctx[ECU_SENSOR_APS_MAX] = {0};
static ots_ctx_t ecu_config_ots_ctx[ECU_SENSOR_OTS_MAX] = {0};
static ops_ctx_t ecu_config_ops_ctx[ECU_SENSOR_OPS_MAX] = {0};

static ecu_core_runtime_value_ctx_t ecu_config_ckp_params_read[ECU_SENSOR_CKP_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_cmp_params_read[ECU_SENSOR_CMP_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_ect_params_read[ECU_SENSOR_ECT_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_egt_params_read[ECU_SENSOR_EGT_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_iat_params_read[ECU_SENSOR_IAT_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_maf_params_read[ECU_SENSOR_MAF_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_map_params_read[ECU_SENSOR_MAP_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_tps_params_read[ECU_SENSOR_TPS_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_vss_params_read[ECU_SENSOR_VSS_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_aps_params_read[ECU_SENSOR_APS_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_ots_params_read[ECU_SENSOR_OTS_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_ops_params_read[ECU_SENSOR_OPS_MAX][ECU_SENSOR_CKP_READ_PARAM_MAX] = {0};

static const ecu_config_sensors_config_t ecu_config_sensors = {
    .interfaces = {
        {
            .loop_main = (ecu_sensor_loop_func_t)ckp_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)ckp_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)ckp_loop_fast,
            .instance_max = ECU_SENSOR_CKP_MAX,
            .params_read_count = ECU_SENSOR_CKP_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_CKP
        {
            .loop_main = (ecu_sensor_loop_func_t)cmp_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)cmp_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)cmp_loop_fast,
            .instance_max = ECU_SENSOR_CMP_MAX,
            .params_read_count = ECU_SENSOR_CMP_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_CMP
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)ect_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_ECT_MAX,
            .params_read_count = ECU_SENSOR_ECT_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_ECT
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)egt_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_EGT_MAX,
            .params_read_count = ECU_SENSOR_EGT_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_EGT
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)iat_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_IAT_MAX,
            .params_read_count = ECU_SENSOR_IAT_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_IAT
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)maf_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_MAF_MAX,
            .params_read_count = ECU_SENSOR_MAF_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_MAF
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)map_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_MAP_MAX,
            .params_read_count = ECU_SENSOR_MAP_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_MAP
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)tps_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_TPS_MAX,
            .params_read_count = ECU_SENSOR_TPS_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_TPS
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)vss_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_VSS_MAX,
            .params_read_count = ECU_SENSOR_VSS_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_VSS
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)aps_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_APS_MAX,
            .params_read_count = ECU_SENSOR_APS_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_APS
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)ots_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_OTS_MAX,
            .params_read_count = ECU_SENSOR_OTS_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_OTS
        {
            .loop_main = (ecu_sensor_loop_func_t)NULL,
            .loop_slow = (ecu_sensor_loop_func_t)ops_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)NULL,
            .instance_max = ECU_SENSOR_OPS_MAX,
            .params_read_count = ECU_SENSOR_OPS_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_SENSOR_TYPE_OPS
    },
    .sensors = {
        {
            .type = ECU_SENSOR_TYPE_CKP,
            .instance = ECU_SENSOR_CKP_1,
            .ctx = &ecu_config_ckp_ctx[ECU_SENSOR_CKP_1],
            .params_read_ptr = ecu_config_ckp_params_read[ECU_SENSOR_CKP_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_CMP,
            .instance = ECU_SENSOR_CMP_1,
            .ctx = &ecu_config_cmp_ctx[ECU_SENSOR_CMP_1],
            .params_read_ptr = ecu_config_cmp_params_read[ECU_SENSOR_CMP_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_CMP,
            .instance = ECU_SENSOR_CMP_2,
            .ctx = &ecu_config_cmp_ctx[ECU_SENSOR_CMP_2],
            .params_read_ptr = ecu_config_cmp_params_read[ECU_SENSOR_CMP_2],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_CMP,
            .instance = ECU_SENSOR_CMP_3,
            .ctx = &ecu_config_cmp_ctx[ECU_SENSOR_CMP_3],
            .params_read_ptr = ecu_config_cmp_params_read[ECU_SENSOR_CMP_3],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_CMP,
            .instance = ECU_SENSOR_CMP_4,
            .ctx = &ecu_config_cmp_ctx[ECU_SENSOR_CMP_4],
            .params_read_ptr = ecu_config_cmp_params_read[ECU_SENSOR_CMP_4],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_ECT,
            .instance = ECU_SENSOR_ECT_1,
            .ctx = &ecu_config_ect_ctx[ECU_SENSOR_ECT_1],
            .params_read_ptr = ecu_config_ect_params_read[ECU_SENSOR_ECT_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_ECT,
            .instance = ECU_SENSOR_ECT_2,
            .ctx = &ecu_config_ect_ctx[ECU_SENSOR_ECT_2],
            .params_read_ptr = ecu_config_ect_params_read[ECU_SENSOR_ECT_2],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_EGT,
            .instance = ECU_SENSOR_EGT_1,
            .ctx = &ecu_config_egt_ctx[ECU_SENSOR_EGT_1],
            .params_read_ptr = ecu_config_egt_params_read[ECU_SENSOR_EGT_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_EGT,
            .instance = ECU_SENSOR_EGT_2,
            .ctx = &ecu_config_egt_ctx[ECU_SENSOR_EGT_2],
            .params_read_ptr = ecu_config_egt_params_read[ECU_SENSOR_EGT_2],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_IAT,
            .instance = ECU_SENSOR_IAT_1,
            .ctx = &ecu_config_iat_ctx[ECU_SENSOR_IAT_1],
            .params_read_ptr = ecu_config_iat_params_read[ECU_SENSOR_IAT_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_IAT,
            .instance = ECU_SENSOR_IAT_2,
            .ctx = &ecu_config_iat_ctx[ECU_SENSOR_IAT_2],
            .params_read_ptr = ecu_config_iat_params_read[ECU_SENSOR_IAT_2],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_MAF,
            .instance = ECU_SENSOR_MAF_1,
            .ctx = &ecu_config_maf_ctx[ECU_SENSOR_MAF_1],
            .params_read_ptr = ecu_config_maf_params_read[ECU_SENSOR_MAF_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_MAF,
            .instance = ECU_SENSOR_MAF_2,
            .ctx = &ecu_config_maf_ctx[ECU_SENSOR_MAF_2],
            .params_read_ptr = ecu_config_maf_params_read[ECU_SENSOR_MAF_2],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_MAP,
            .instance = ECU_SENSOR_MAP_1,
            .ctx = &ecu_config_map_ctx[ECU_SENSOR_MAP_1],
            .params_read_ptr = ecu_config_map_params_read[ECU_SENSOR_MAP_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_MAP,
            .instance = ECU_SENSOR_MAP_2,
            .ctx = &ecu_config_map_ctx[ECU_SENSOR_MAP_2],
            .params_read_ptr = ecu_config_map_params_read[ECU_SENSOR_MAP_2],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_MAP,
            .instance = ECU_SENSOR_MAP_3,
            .ctx = &ecu_config_map_ctx[ECU_SENSOR_MAP_3],
            .params_read_ptr = ecu_config_map_params_read[ECU_SENSOR_MAP_3],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_MAP,
            .instance = ECU_SENSOR_MAP_4,
            .ctx = &ecu_config_map_ctx[ECU_SENSOR_MAP_4],
            .params_read_ptr = ecu_config_map_params_read[ECU_SENSOR_MAP_4],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_TPS,
            .instance = ECU_SENSOR_TPS_1,
            .ctx = &ecu_config_tps_ctx[ECU_SENSOR_TPS_1],
            .params_read_ptr = ecu_config_tps_params_read[ECU_SENSOR_TPS_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_TPS,
            .instance = ECU_SENSOR_TPS_2,
            .ctx = &ecu_config_tps_ctx[ECU_SENSOR_TPS_2],
            .params_read_ptr = ecu_config_tps_params_read[ECU_SENSOR_TPS_2],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_VSS,
            .instance = ECU_SENSOR_VSS_1,
            .ctx = &ecu_config_vss_ctx[ECU_SENSOR_VSS_1],
            .params_read_ptr = ecu_config_vss_params_read[ECU_SENSOR_VSS_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_APS,
            .instance = ECU_SENSOR_APS_1,
            .ctx = &ecu_config_aps_ctx[ECU_SENSOR_APS_1],
            .params_read_ptr = ecu_config_aps_params_read[ECU_SENSOR_APS_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_OTS,
            .instance = ECU_SENSOR_OTS_1,
            .ctx = &ecu_config_ots_ctx[ECU_SENSOR_OTS_1],
            .params_read_ptr = ecu_config_ots_params_read[ECU_SENSOR_OTS_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_OTS,
            .instance = ECU_SENSOR_OTS_2,
            .ctx = &ecu_config_ots_ctx[ECU_SENSOR_OTS_2],
            .params_read_ptr = ecu_config_ots_params_read[ECU_SENSOR_OTS_2],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_OPS,
            .instance = ECU_SENSOR_OPS_1,
            .ctx = &ecu_config_ops_ctx[ECU_SENSOR_OPS_1],
            .params_read_ptr = ecu_config_ops_params_read[ECU_SENSOR_OPS_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_SENSOR_TYPE_OPS,
            .instance = ECU_SENSOR_OPS_2,
            .ctx = &ecu_config_ops_ctx[ECU_SENSOR_OPS_2],
            .params_read_ptr = ecu_config_ops_params_read[ECU_SENSOR_OPS_2],
            .params_write_ptr = NULL,
        },
    },
};

static RAM_SECTION ecu_config_sensors_t ecu_config_sensors_ctx = {
    .config = &ecu_config_sensors,
};

error_t ecu_sensors_init(void)
{
  error_t err = E_OK;
  const ecu_config_sensor_if_config_ctx_t *interface_config;
  const ecu_config_sensor_config_ctx_t *sensor_config;
  ecu_config_sensor_if_instance_ctx_t *interface_ctx;
  ecu_config_sensor_instance_ctx_t *sensor_ctx;

  for(int i = 0; i < ECU_SENSORS_MAX; i++) {
    sensor_config = &ecu_config_sensors.sensors[i];
    sensor_ctx = &ecu_config_sensors_ctx.sensors[i];
    sensor_ctx->initialized = false;

    BREAK_IF_ACTION(sensor_config->type >= ECU_SENSOR_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(sensor_config->ctx == NULL, err = E_FAULT);

    interface_config = &ecu_config_sensors.interfaces[sensor_config->type];
    BREAK_IF_ACTION(sensor_config->instance >= interface_config->instance_max, err = E_FAULT);
  }

  for(int i = 0; i < ITEMSOF(ecu_config_sensors.interfaces); i++) {
    interface_config = &ecu_config_sensors.interfaces[i];
    interface_ctx = &ecu_config_sensors_ctx.interfaces[i];

    BREAK_IF_ACTION(interface_config->instance_max > ECU_SENSOR_INSTANCE_MAX, err = E_FAULT);
    interface_ctx->instance_first = ECU_SENSORS_MAX;
    for(int n = 0; n < ECU_SENSORS_MAX; n++) {
      sensor_config = &ecu_config_sensors.sensors[n];
      if(sensor_config->type == i) {
        BREAK_IF_ACTION(sensor_config->instance != 0, err = E_FAULT);
        interface_ctx->instance_first = n;
        break;
      }
    }
    BREAK_IF(err != E_OK);
  }

  return err;
}

ITCM_FUNC static void ecu_sensors_loop(ecu_config_sensor_loop_type_t loop_type)
{
  const ecu_config_sensor_if_config_ctx_t *interface_config;
  const ecu_config_sensor_config_ctx_t *sensor_config;
  ecu_config_sensor_instance_ctx_t *sensor_ctx;
  ecu_sensor_type_t if_type;

  switch(loop_type) {
    case ECU_SENSOR_LOOP_TYPE_FAST:
      for(int i = 0; i < ECU_SENSORS_MAX; i++) {
        sensor_ctx = &ecu_config_sensors_ctx.sensors[i];
        sensor_config = &ecu_config_sensors.sensors[i];
        if(sensor_ctx->initialized == true) {
          if_type = sensor_config->type;
          if(if_type < ECU_SENSOR_TYPE_MAX) {
            interface_config = &ecu_config_sensors.interfaces[if_type];
            if(interface_config->loop_fast != NULL && sensor_config->ctx != NULL) {
              interface_config->loop_fast(sensor_config->ctx);
            }
          }
        }
      }
      break;
    case ECU_SENSOR_LOOP_TYPE_SLOW:
      for(int i = 0; i < ECU_SENSORS_MAX; i++) {
        sensor_ctx = &ecu_config_sensors_ctx.sensors[i];
        sensor_config = &ecu_config_sensors.sensors[i];
        if(sensor_ctx->initialized == true) {
          if_type = sensor_config->type;
          if(if_type < ECU_SENSOR_TYPE_MAX) {
            interface_config = &ecu_config_sensors.interfaces[if_type];
            if(interface_config->loop_slow != NULL && sensor_config->ctx != NULL) {
              interface_config->loop_slow(sensor_config->ctx);
            }
          }
        }
      }
      break;
    case ECU_SENSOR_LOOP_TYPE_MAIN:
      for(int i = 0; i < ECU_SENSORS_MAX; i++) {
        sensor_ctx = &ecu_config_sensors_ctx.sensors[i];
        sensor_config = &ecu_config_sensors.sensors[i];
        if(sensor_ctx->initialized == true) {
          if_type = sensor_config->type;
          if(if_type < ECU_SENSOR_TYPE_MAX) {
            interface_config = &ecu_config_sensors.interfaces[if_type];
            if(interface_config->loop_main != NULL && sensor_config->ctx != NULL) {
              interface_config->loop_main(sensor_config->ctx);
            }
          }
        }
      }
      break;
    default:
      break;
  }
}

void ecu_sensors_loop_main(void)
{
  ecu_sensors_loop(ECU_SENSOR_LOOP_TYPE_MAIN);
}

void ecu_sensors_loop_slow(void)
{
  ecu_sensors_loop(ECU_SENSOR_LOOP_TYPE_SLOW);
}

ITCM_FUNC void ecu_sensors_loop_fast(void)
{
  ecu_sensors_loop(ECU_SENSOR_LOOP_TYPE_FAST);
}

error_t ecu_sensors_get_sensor_ctx(ecu_sensor_type_t type, ecu_sensor_instance_t instance, void **ctx)
{
  error_t err = E_FAULT;
  const ecu_config_sensor_config_ctx_t *sensor_config;

  if(ctx == NULL) {
    err = E_PARAM;
  } else {
    for(int i = 0; i < ECU_SENSORS_MAX; i++) {
      sensor_config = &ecu_config_sensors.sensors[i];
      if(sensor_config->type == type && sensor_config->instance == instance) {
        *ctx = sensor_config->ctx;
        err = E_OK;
        break;
      }
    }
  }

  return err;
}

error_t ecu_sensors_set_sensor_initialized(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool initialized)
{
  error_t err = E_FAULT;
  ecu_config_sensor_instance_ctx_t *ctx;
  const ecu_config_sensor_if_instance_ctx_t *interface;
  const ecu_config_sensor_config_ctx_t *ctx_config;
  const ecu_config_sensor_if_config_ctx_t *interface_config;

  do {
    BREAK_IF(type >= ECU_SENSOR_TYPE_MAX);
    interface_config = &ecu_config_sensors.interfaces[type];
    interface = &ecu_config_sensors_ctx.interfaces[type];
    BREAK_IF(instance >= interface_config->instance_max);
    BREAK_IF(interface->instance_first >= ECU_SENSORS_MAX);

    ctx = &ecu_config_sensors_ctx.sensors[interface->instance_first];
    ctx_config = &ecu_config_sensors.sensors[interface->instance_first];
    ctx = &ctx[instance];
    ctx_config = &ctx_config[instance];
    if(ctx_config->type == type && ctx_config->instance == instance) {
      ctx->initialized = initialized;
      err = E_OK;
    }
  } while(0);

  return err;
}

error_t ecu_sensors_get_sensor_initialized(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool *initialized)
{
  error_t err = E_FAULT;
  const ecu_config_sensor_instance_ctx_t *ctx;
  const ecu_config_sensor_if_instance_ctx_t *interface;
  const ecu_config_sensor_config_ctx_t *ctx_config;
  const ecu_config_sensor_if_config_ctx_t *interface_config;

  do {
    BREAK_IF(type >= ECU_SENSOR_TYPE_MAX);
    interface_config = &ecu_config_sensors.interfaces[type];
    interface = &ecu_config_sensors_ctx.interfaces[type];
    BREAK_IF(instance >= interface_config->instance_max);
    BREAK_IF(interface->instance_first >= ECU_SENSORS_MAX);

    ctx = &ecu_config_sensors_ctx.sensors[interface->instance_first];
    ctx_config = &ecu_config_sensors.sensors[interface->instance_first];
    ctx = &ctx[instance];
    ctx_config = &ctx_config[instance];
    if(ctx_config->type == type && ctx_config->instance == instance) {
      *initialized = ctx->initialized;
      err = E_OK;
    }
  } while(0);

  return err;
}

error_t ecu_sensors_set_sensor_enabled(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool enabled)
{
  error_t err = E_FAULT;
  ecu_config_sensor_instance_ctx_t *ctx;
  const ecu_config_sensor_if_instance_ctx_t *interface;
  const ecu_config_sensor_config_ctx_t *ctx_config;
  const ecu_config_sensor_if_config_ctx_t *interface_config;

  do {
    BREAK_IF(type >= ECU_SENSOR_TYPE_MAX);
    interface_config = &ecu_config_sensors.interfaces[type];
    interface = &ecu_config_sensors_ctx.interfaces[type];
    BREAK_IF(instance >= interface_config->instance_max);
    BREAK_IF(interface->instance_first >= ECU_SENSORS_MAX);

    ctx = &ecu_config_sensors_ctx.sensors[interface->instance_first];
    ctx_config = &ecu_config_sensors.sensors[interface->instance_first];
    ctx = &ctx[instance];
    ctx_config = &ctx_config[instance];
    if(ctx_config->type == type && ctx_config->instance == instance) {
      ctx->enabled = enabled;
      err = E_OK;
    }
  } while(0);

  return err;
}

error_t ecu_sensors_get_sensor_enabled(ecu_sensor_type_t type, ecu_sensor_instance_t instance, bool *enabled)
{
  error_t err = E_FAULT;
  const ecu_config_sensor_instance_ctx_t *ctx;
  const ecu_config_sensor_if_instance_ctx_t *interface;
  const ecu_config_sensor_config_ctx_t *ctx_config;
  const ecu_config_sensor_if_config_ctx_t *interface_config;

  do {
    BREAK_IF(type >= ECU_SENSOR_TYPE_MAX);
    interface_config = &ecu_config_sensors.interfaces[type];
    interface = &ecu_config_sensors_ctx.interfaces[type];
    BREAK_IF(instance >= interface_config->instance_max);
    BREAK_IF(interface->instance_first >= ECU_SENSORS_MAX);

    ctx = &ecu_config_sensors_ctx.sensors[interface->instance_first];
    ctx_config = &ecu_config_sensors.sensors[interface->instance_first];
    ctx = &ctx[instance];
    ctx_config = &ctx_config[instance];
    if(ctx_config->type == type && ctx_config->instance == instance) {
      *enabled = ctx->enabled;
      err = E_OK;
    }
  } while(0);

  return err;
}

error_t ecu_sensors_get_type_max(ecu_sensor_type_t *type_max)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(type_max == NULL, err = E_PARAM);

    *type_max = ECU_SENSOR_TYPE_MAX;

  } while(0);

  return err;
}

error_t ecu_sensors_get_instance_max(ecu_sensor_type_t type, ecu_sensor_instance_t *instance_max)
{
  error_t err = E_OK;
  const ecu_config_sensor_if_config_ctx_t *interface_config;

  do {
    BREAK_IF_ACTION(type >= ECU_SENSOR_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(instance_max == NULL, err = E_PARAM);

    interface_config = &ecu_config_sensors.interfaces[type];
    *instance_max = interface_config->instance_max;

  } while(0);

  return err;
}

error_t ecu_sensors_get_instance_parameters_read(ecu_sensor_type_t type, ecu_sensor_instance_t instance, ecu_core_runtime_value_ctx_t **read, ecu_runtime_param_index_t *count)
{
  error_t err = E_OK;
  const ecu_config_sensor_config_ctx_t *ctx_config;
  const ecu_config_sensor_if_config_ctx_t *interface_config;
  const ecu_config_sensor_if_instance_ctx_t *interface;

  do {
    BREAK_IF_ACTION(type >= ECU_SENSOR_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(read == NULL, err = E_PARAM);
    BREAK_IF_ACTION(count == NULL, err = E_PARAM);

    interface_config = &ecu_config_sensors.interfaces[type];
    interface = &ecu_config_sensors_ctx.interfaces[type];
    BREAK_IF_ACTION(instance >= interface_config->instance_max, err = E_PARAM);
    BREAK_IF_ACTION(interface->instance_first >= ECU_SENSORS_MAX, err = E_FAULT);

    ctx_config = &ecu_config_sensors.sensors[interface->instance_first];
    ctx_config = &ctx_config[instance];

    *count = interface_config->params_read_count;
    *read = &ctx_config->params_read_ptr[instance];

  } while(0);

  return err;
}

error_t ecu_sensors_get_instance_parameters_write(ecu_sensor_type_t type, ecu_sensor_instance_t instance, ecu_core_runtime_value_ctx_t **write, ecu_runtime_param_index_t *count)
{
  error_t err = E_OK;
  const ecu_config_sensor_config_ctx_t *ctx_config;
  const ecu_config_sensor_if_instance_ctx_t *interface;
  const ecu_config_sensor_if_config_ctx_t *interface_config;

  do {
    BREAK_IF_ACTION(type >= ECU_SENSOR_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(write == NULL, err = E_PARAM);
    BREAK_IF_ACTION(count == NULL, err = E_PARAM);

    interface_config = &ecu_config_sensors.interfaces[type];
    interface = &ecu_config_sensors_ctx.interfaces[type];
    BREAK_IF_ACTION(instance >= interface_config->instance_max, err = E_PARAM);
    BREAK_IF_ACTION(interface->instance_first >= ECU_SENSORS_MAX, err = E_FAULT);

    ctx_config = &ecu_config_sensors.sensors[interface->instance_first];
    ctx_config = &ctx_config[instance];

    *count = interface_config->params_write_count;
    *write = &ctx_config->params_write_ptr[instance];

  } while(0);

  return err;
}

error_t ecu_sensors_get_ckp_ctx(ecu_sensor_ckp_t instance, ckp_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_CKP, instance, (void**)ctx);
}

error_t ecu_sensors_get_cmp_ctx(ecu_sensor_cmp_t instance, cmp_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_CMP, instance, (void**)ctx);
}

error_t ecu_sensors_get_ect_ctx(ecu_sensor_ect_t instance, ect_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_ECT, instance, (void**)ctx);
}

error_t ecu_sensors_get_egt_ctx(ecu_sensor_egt_t instance, egt_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_EGT, instance, (void**)ctx);
}

error_t ecu_sensors_get_iat_ctx(ecu_sensor_iat_t instance, iat_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_IAT, instance, (void**)ctx);
}

error_t ecu_sensors_get_maf_ctx(ecu_sensor_maf_t instance, maf_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_MAF, instance, (void**)ctx);
}

error_t ecu_sensors_get_map_ctx(ecu_sensor_map_t instance, map_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_MAP, instance, (void**)ctx);
}

error_t ecu_sensors_get_tps_ctx(ecu_sensor_tps_t instance, tps_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_TPS, instance, (void**)ctx);
}

error_t ecu_sensors_get_vss_ctx(ecu_sensor_vss_t instance, vss_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_VSS, instance, (void**)ctx);
}

error_t ecu_sensors_get_aps_ctx(ecu_sensor_aps_t instance, aps_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_APS, instance, (void**)ctx);
}

error_t ecu_sensors_get_ots_ctx(ecu_sensor_ots_t instance, ots_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_OTS, instance, (void**)ctx);
}

error_t ecu_sensors_get_ops_ctx(ecu_sensor_ops_t instance, ops_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_OPS, instance, (void**)ctx);
}
