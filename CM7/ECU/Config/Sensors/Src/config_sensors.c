/*
 * config_sensors.c
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#include "config_sensors.h"
#include "compiler.h"

#define ECU_SENSORS_MAX (     \
    ECU_SENSOR_CKP_MAX +      \
    ECU_SENSOR_CMP_MAX +      \
    ECU_SENSOR_ECT_MAX +      \
    ECU_SENSOR_EGT_MAX +      \
    ECU_SENSOR_IAT_MAX +      \
    ECU_SENSOR_MAF_MAX +      \
    ECU_SENSOR_MAP_MAX +      \
    ECU_SENSOR_TPS_MAX +      \
    ECU_SENSOR_APS_MAX)

typedef enum {
  ECU_SENSOR_LOOP_TYPE_MAIN = 0,
  ECU_SENSOR_LOOP_TYPE_SLOW,
  ECU_SENSOR_LOOP_TYPE_FAST
}ecu_config_sensor_loop_type_t;

typedef struct {
    ecu_sensor_loop_func_t loop_main;
    ecu_sensor_loop_func_t loop_slow;
    ecu_sensor_loop_func_t loop_fast;
    ecu_sensor_instance_t instance_max;
}ecu_config_sensor_if_instance_t;

typedef struct {
    ecu_sensor_type_t type;
    ecu_sensor_instance_t instance;
    void *ctx;
    bool initialized;
}ecu_config_sensor_instance_t;

typedef struct {
    ecu_config_sensor_if_instance_t interfaces[ECU_SENSOR_TYPE_MAX];
    ecu_config_sensor_instance_t sensors[ECU_SENSORS_MAX];
}ecu_config_sensors_t;

static ckp_ctx_t ecu_config_ckp_ctx[ECU_SENSOR_CKP_MAX] = {0};
static cmp_ctx_t ecu_config_cmp_ctx[ECU_SENSOR_CMP_MAX] = {0};
static ect_ctx_t ecu_config_ect_ctx[ECU_SENSOR_ECT_MAX] = {0};
static egt_ctx_t ecu_config_egt_ctx[ECU_SENSOR_EGT_MAX] = {0};
static iat_ctx_t ecu_config_iat_ctx[ECU_SENSOR_IAT_MAX] = {0};
static maf_ctx_t ecu_config_maf_ctx[ECU_SENSOR_MAF_MAX] = {0};
static map_ctx_t ecu_config_map_ctx[ECU_SENSOR_MAP_MAX] = {0};
static tps_ctx_t ecu_config_tps_ctx[ECU_SENSOR_TPS_MAX] = {0};
static aps_ctx_t ecu_config_aps_ctx[ECU_SENSOR_APS_MAX] = {0};

static ecu_config_sensors_t ecu_config_sensors = {
    .interfaces = {
        {
            .loop_main = (ecu_sensor_loop_func_t)ckp_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)ckp_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)ckp_loop_fast,
            .instance_max = ECU_SENSOR_CKP_MAX,
        }, //ECU_SENSOR_TYPE_CKP
        {
            .loop_main = (ecu_sensor_loop_func_t)cmp_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)cmp_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)cmp_loop_fast,
            .instance_max = ECU_SENSOR_CMP_MAX,
        }, //ECU_SENSOR_TYPE_CMP
        {
            .loop_main = (ecu_sensor_loop_func_t)ect_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)ect_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)ect_loop_fast,
            .instance_max = ECU_SENSOR_ECT_MAX,
        }, //ECU_SENSOR_TYPE_ECT
        {
            .loop_main = (ecu_sensor_loop_func_t)egt_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)egt_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)egt_loop_fast,
            .instance_max = ECU_SENSOR_EGT_MAX,
        }, //ECU_SENSOR_TYPE_EGT
        {
            .loop_main = (ecu_sensor_loop_func_t)iat_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)iat_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)iat_loop_fast,
            .instance_max = ECU_SENSOR_IAT_MAX,
        }, //ECU_SENSOR_TYPE_IAT
        {
            .loop_main = (ecu_sensor_loop_func_t)maf_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)maf_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)maf_loop_fast,
            .instance_max = ECU_SENSOR_MAF_MAX,
        }, //ECU_SENSOR_TYPE_MAF
        {
            .loop_main = (ecu_sensor_loop_func_t)map_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)map_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)map_loop_fast,
            .instance_max = ECU_SENSOR_MAP_MAX,
        }, //ECU_SENSOR_TYPE_MAP
        {
            .loop_main = (ecu_sensor_loop_func_t)tps_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)tps_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)tps_loop_fast,
            .instance_max = ECU_SENSOR_TPS_MAX,
        }, //ECU_SENSOR_TYPE_TPS
        {
            .loop_main = (ecu_sensor_loop_func_t)aps_loop_main,
            .loop_slow = (ecu_sensor_loop_func_t)aps_loop_slow,
            .loop_fast = (ecu_sensor_loop_func_t)aps_loop_fast,
            .instance_max = ECU_SENSOR_APS_MAX,
        }, //ECU_SENSOR_TYPE_APS
    },
    .sensors = {
        {
            .type = ECU_SENSOR_TYPE_CKP,
            .instance = ECU_SENSOR_CKP_1,
            .ctx = &ecu_config_ckp_ctx[ECU_SENSOR_CKP_1],
        },
        {
            .type = ECU_SENSOR_TYPE_CMP,
            .instance = ECU_SENSOR_CMP_1,
            .ctx = &ecu_config_cmp_ctx[ECU_SENSOR_CMP_1],
        },
        {
            .type = ECU_SENSOR_TYPE_CMP,
            .instance = ECU_SENSOR_CMP_2,
            .ctx = &ecu_config_cmp_ctx[ECU_SENSOR_CMP_2],
        },
        {
            .type = ECU_SENSOR_TYPE_CMP,
            .instance = ECU_SENSOR_CMP_3,
            .ctx = &ecu_config_cmp_ctx[ECU_SENSOR_CMP_3],
        },
        {
            .type = ECU_SENSOR_TYPE_CMP,
            .instance = ECU_SENSOR_CMP_4,
            .ctx = &ecu_config_cmp_ctx[ECU_SENSOR_CMP_4],
        },
        {
            .type = ECU_SENSOR_TYPE_ECT,
            .instance = ECU_SENSOR_ECT_1,
            .ctx = &ecu_config_ect_ctx[ECU_SENSOR_ECT_1],
        },
        {
            .type = ECU_SENSOR_TYPE_ECT,
            .instance = ECU_SENSOR_ECT_2,
            .ctx = &ecu_config_ect_ctx[ECU_SENSOR_ECT_2],
        },
        {
            .type = ECU_SENSOR_TYPE_EGT,
            .instance = ECU_SENSOR_EGT_1,
            .ctx = &ecu_config_egt_ctx[ECU_SENSOR_EGT_1],
        },
        {
            .type = ECU_SENSOR_TYPE_EGT,
            .instance = ECU_SENSOR_EGT_2,
            .ctx = &ecu_config_egt_ctx[ECU_SENSOR_EGT_2],
        },
        {
            .type = ECU_SENSOR_TYPE_IAT,
            .instance = ECU_SENSOR_IAT_1,
            .ctx = &ecu_config_iat_ctx[ECU_SENSOR_IAT_1],
        },
        {
            .type = ECU_SENSOR_TYPE_IAT,
            .instance = ECU_SENSOR_IAT_2,
            .ctx = &ecu_config_iat_ctx[ECU_SENSOR_IAT_2],
        },
        {
            .type = ECU_SENSOR_TYPE_MAF,
            .instance = ECU_SENSOR_MAF_1,
            .ctx = &ecu_config_maf_ctx[ECU_SENSOR_MAF_1],
        },
        {
            .type = ECU_SENSOR_TYPE_MAF,
            .instance = ECU_SENSOR_MAF_2,
            .ctx = &ecu_config_maf_ctx[ECU_SENSOR_MAF_2],
        },
        {
            .type = ECU_SENSOR_TYPE_MAP,
            .instance = ECU_SENSOR_MAP_1,
            .ctx = &ecu_config_map_ctx[ECU_SENSOR_MAP_1],
        },
        {
            .type = ECU_SENSOR_TYPE_MAP,
            .instance = ECU_SENSOR_MAP_2,
            .ctx = &ecu_config_map_ctx[ECU_SENSOR_MAP_2],
        },
        {
            .type = ECU_SENSOR_TYPE_MAP,
            .instance = ECU_SENSOR_MAP_3,
            .ctx = &ecu_config_map_ctx[ECU_SENSOR_MAP_3],
        },
        {
            .type = ECU_SENSOR_TYPE_MAP,
            .instance = ECU_SENSOR_MAP_4,
            .ctx = &ecu_config_map_ctx[ECU_SENSOR_MAP_4],
        },
        {
            .type = ECU_SENSOR_TYPE_TPS,
            .instance = ECU_SENSOR_TPS_1,
            .ctx = &ecu_config_tps_ctx[ECU_SENSOR_TPS_1],
        },
        {
            .type = ECU_SENSOR_TYPE_TPS,
            .instance = ECU_SENSOR_TPS_2,
            .ctx = &ecu_config_tps_ctx[ECU_SENSOR_TPS_2],
        },
        {
            .type = ECU_SENSOR_TYPE_APS,
            .instance = ECU_SENSOR_APS_1,
            .ctx = &ecu_config_aps_ctx[ECU_SENSOR_APS_1],
        },
    },
};

error_t ecu_sensors_init(void)
{
  error_t err = E_OK;
  ecu_config_sensor_if_instance_t *interface;
  ecu_config_sensor_instance_t *sensor;

  for(int i = 0; i < ITEMSOF(ecu_config_sensors.sensors); i++) {
    sensor = &ecu_config_sensors.sensors[i];
    sensor->initialized = false;

    BREAK_IF_ACTION(sensor->type >= ECU_SENSOR_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(sensor->ctx == NULL, err = E_FAULT);

    interface = &ecu_config_sensors.interfaces[sensor->type];
    BREAK_IF_ACTION(sensor->instance >= interface->instance_max, err = E_FAULT);
  }

  return err;
}

ITCM_FUNC static void ecu_sensors_loop(ecu_config_sensor_loop_type_t loop_type)
{
  ecu_config_sensor_if_instance_t *interface;
  ecu_config_sensor_instance_t *sensor;
  ecu_sensor_type_t if_type;

  switch(loop_type) {
    case ECU_SENSOR_LOOP_TYPE_FAST:
      for(int i = 0; i < ITEMSOF(ecu_config_sensors.sensors); i++) {
        sensor = &ecu_config_sensors.sensors[i];
        if(sensor->initialized == true) {
          if_type = sensor->type;
          if(if_type < ECU_SENSOR_TYPE_MAX) {
            interface = &ecu_config_sensors.interfaces[if_type];
            if(interface->loop_fast != NULL && sensor->ctx != NULL) {
              interface->loop_fast(sensor->ctx);
            }
          }
        }
      }
      break;
    case ECU_SENSOR_LOOP_TYPE_SLOW:
      for(int i = 0; i < ITEMSOF(ecu_config_sensors.sensors); i++) {
        sensor = &ecu_config_sensors.sensors[i];
        if(sensor->initialized == true) {
          if_type = sensor->type;
          if(if_type < ECU_SENSOR_TYPE_MAX) {
            interface = &ecu_config_sensors.interfaces[if_type];
            if(interface->loop_slow != NULL && sensor->ctx != NULL) {
              interface->loop_slow(sensor->ctx);
            }
          }
        }
      }
      break;
    case ECU_SENSOR_LOOP_TYPE_MAIN:
      for(int i = 0; i < ITEMSOF(ecu_config_sensors.sensors); i++) {
        sensor = &ecu_config_sensors.sensors[i];
        if(sensor->initialized == true) {
          if_type = sensor->type;
          if(if_type < ECU_SENSOR_TYPE_MAX) {
            interface = &ecu_config_sensors.interfaces[if_type];
            if(interface->loop_main != NULL && sensor->ctx != NULL) {
              interface->loop_main(sensor->ctx);
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
  ecu_config_sensor_instance_t *sensor;

  if(ctx == NULL) {
    err = E_PARAM;
  } else {
    for(int i = 0; i < ITEMSOF(ecu_config_sensors.sensors); i++) {
      sensor = &ecu_config_sensors.sensors[i];
      if(sensor->type == type && sensor->instance == instance) {
        *ctx = sensor->ctx;
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
  ecu_config_sensor_instance_t *sensor;

  for(int i = 0; i < ITEMSOF(ecu_config_sensors.sensors); i++) {
    sensor = &ecu_config_sensors.sensors[i];
    if(sensor->type == type && sensor->instance == instance) {
      sensor->initialized = initialized;
      err = E_OK;
      break;
    }
  }

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

error_t ecu_sensors_get_aps_ctx(ecu_sensor_aps_t instance, aps_ctx_t **ctx)
{
  return ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_APS, instance, (void**)ctx);
}
