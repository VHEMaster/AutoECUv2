/*
 * config_common.c
 *
 *  Created on: Nov 9, 2025
 *      Author: VHEMaster
 */

#include "config_common.h"

#include "config_devices.h"
#include "config_sensors.h"
#include "config_modules.h"

#if (ECU_DEVICE_TYPE_MAX > ECU_ENTITY_TYPE_MAX)
#error "ECU_DEVICE_TYPE_MAX is larger than ECU_ENTITY_TYPE_MAX"
#endif
#if (ECU_SENSOR_TYPE_MAX > ECU_ENTITY_TYPE_MAX)
#error "ECU_SENSOR_TYPE_MAX is larger than ECU_ENTITY_TYPE_MAX"
#endif
#if (ECU_MODULE_TYPE_MAX > ECU_ENTITY_TYPE_MAX)
#error "ECU_MODULE_TYPE_MAX is larger than ECU_ENTITY_TYPE_MAX"
#endif

#if (ECU_DEVICE_INSTANCE_MAX > ECU_ENTITY_INSTANCE_MAX)
#error "ECU_SENSOR_INSTANCE_MAX is larger than ECU_ENTITY_INSTANCE_MAX"
#endif
#if (ECU_SENSOR_INSTANCE_MAX > ECU_ENTITY_INSTANCE_MAX)
#error "ECU_SENSOR_INSTANCE_MAX is larger than ECU_ENTITY_INSTANCE_MAX"
#endif
#if (ECU_MODULE_INSTANCE_MAX > ECU_ENTITY_INSTANCE_MAX)
#error "ECU_SENSOR_INSTANCE_MAX is larger than ECU_ENTITY_INSTANCE_MAX"
#endif

static error_t ecu_config_common_devices(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity);
static error_t ecu_config_common_sensors(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity);
static error_t ecu_config_common_modules(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity);

typedef struct ecu_config_common_ctx_tag ecu_config_common_ctx_t;

typedef error_t (*ecu_entity_config_func_t)(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity);
typedef error_t (*ecu_entity_get_type_max_func_t)(ecu_config_common_entity_type_t *type_max);
typedef error_t (*ecu_entity_get_instance_max_func_t)(ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t *instance_max);

typedef struct {
    ecu_entity_config_func_t configure_func;
    ecu_entity_get_type_max_func_t get_type_max_func;
    ecu_entity_get_instance_max_func_t get_instance_max_func;
}ecu_config_common_entity_config_t;

typedef struct {
    ecu_config_common_entity_config_t entities[ECU_COMMON_ENTITY_MAX];
}ecu_config_common_config_t;

typedef struct {
    void *data_ptr;
}ecu_config_common_entity_parameters_ctx_t;

typedef struct {
    ecu_config_common_entity_parameter_t parameters_count;
    ecu_config_common_entity_parameters_ctx_t parameters[ECU_ENTITY_PARAMETERS_MAX];
} ecu_config_common_entity_instance_ctx_t;

typedef struct {
    ecu_config_common_entity_instance_t instances_count;
    ecu_config_common_entity_instance_ctx_t instances[ECU_ENTITY_INSTANCE_MAX];
}ecu_config_common_entity_type_ctx_t;

typedef struct {
    ecu_config_common_entity_type_t types_count;
    ecu_config_common_entity_type_ctx_t types[ECU_ENTITY_TYPE_MAX];
}ecu_config_common_entity_ctx_t;

typedef struct ecu_config_common_ctx_tag {
    const ecu_config_common_config_t *config;

    ecu_config_common_entity_t entities_count;
    ecu_config_common_entity_ctx_t entities[ECU_COMMON_ENTITY_MAX];
}ecu_config_common_ctx_t;

static const ecu_config_common_config_t ecu_config_common_config = {
    .entities = {
        {
            .configure_func = ecu_config_common_devices,
            .get_type_max_func = ecu_devices_get_type_max,
            .get_instance_max_func = ecu_devices_get_instance_max,
        }, // ECU_COMMON_ENTITY_DEVICE
        {
            .configure_func = ecu_config_common_sensors,
            .get_type_max_func = ecu_sensors_get_type_max,
            .get_instance_max_func = ecu_sensors_get_instance_max,
        }, // ECU_COMMON_ENTITY_SENSOR
        {
            .configure_func = ecu_config_common_modules,
            .get_type_max_func = ecu_modules_get_type_max,
            .get_instance_max_func = ecu_modules_get_instance_max,
        }, // ECU_COMMON_ENTITY_MODULE
    },
};

static RAM_SECTION ecu_config_common_ctx_t ecu_config_common_ctx = {
    .config = &ecu_config_common_config,
};

error_t ecu_config_common_init(ecu_core_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    ecu_config_common_ctx.entities_count = ECU_COMMON_ENTITY_MAX;
    for(ecu_config_common_entity_t e = 0; e < ecu_config_common_ctx.entities_count; e++) {
      err = ecu_config_common_ctx.config->entities[e].configure_func(ctx, e);
      BREAK_IF(err != E_OK);
    }

  } while(0);

  return err;
}

error_t ecu_config_common_get_entity_max(ecu_config_common_entity_t *entity_max)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(entity_max == NULL, err = E_PARAM);

    *entity_max = ecu_config_common_ctx.entities_count;

  } while(0);

  return err;
}

error_t ecu_config_common_get_entity_type_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t *type_max)
{
  error_t err = E_OK;
  ecu_config_common_entity_ctx_t *entity_ctx;
  do {
    BREAK_IF_ACTION(entity >= ecu_config_common_ctx.entities_count, err = E_PARAM);
    BREAK_IF_ACTION(type_max == NULL, err = E_PARAM);

    entity_ctx = &ecu_config_common_ctx.entities[entity];
    *type_max = entity_ctx->types_count;

  } while(0);

  return err;
}

error_t ecu_config_common_get_entity_type_instance_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t *instance_max)
{
  error_t err = E_OK;
  ecu_config_common_entity_ctx_t *entity_ctx;
  ecu_config_common_entity_type_ctx_t *type_ctx;

  do {
    BREAK_IF_ACTION(entity >= ecu_config_common_ctx.entities_count, err = E_PARAM);
    BREAK_IF_ACTION(instance_max == NULL, err = E_PARAM);

    entity_ctx = &ecu_config_common_ctx.entities[entity];
    BREAK_IF_ACTION(type >= entity_ctx->types_count, err = E_PARAM);

    type_ctx = &entity_ctx->types[type];
    *instance_max = type_ctx->instances_count;

  } while(0);

  return err;
}

error_t ecu_config_common_get_entity_type_instance_parameter_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t instance, ecu_config_common_entity_parameter_t *parameter_max)
{
  error_t err = E_OK;
  ecu_config_common_entity_ctx_t *entity_ctx;
  ecu_config_common_entity_type_ctx_t *type_ctx;
  ecu_config_common_entity_instance_ctx_t *instance_ctx;

  do {
    BREAK_IF_ACTION(entity >= ecu_config_common_ctx.entities_count, err = E_PARAM);
    BREAK_IF_ACTION(parameter_max == NULL, err = E_PARAM);

    entity_ctx = &ecu_config_common_ctx.entities[entity];
    BREAK_IF_ACTION(type >= entity_ctx->types_count, err = E_PARAM);

    type_ctx = &entity_ctx->types[type];
    BREAK_IF_ACTION(instance >= type_ctx->instances_count, err = E_PARAM);

    instance_ctx = &type_ctx->instances[instance];
    *parameter_max = instance_ctx->parameters_count;

  } while(0);

  return err;
}

static error_t ecu_config_common_devices(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity)
{
  error_t err = E_OK;
  ecu_config_common_entity_ctx_t *entity_ctx;
  ecu_config_common_entity_type_ctx_t *type_ctx;
  ecu_config_common_entity_instance_ctx_t *instance_ctx;
  const ecu_config_common_entity_config_t *entity_config;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(entity != ECU_COMMON_ENTITY_DEVICE, err = E_PARAM);
    entity_ctx = &ecu_config_common_ctx.entities[entity];
    entity_config = &ecu_config_common_ctx.config->entities[entity];

    err = entity_config->get_type_max_func(&entity_ctx->types_count);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(entity_ctx->types_count > ECU_ENTITY_TYPE_MAX, err = E_FAULT);

    for(ecu_config_common_entity_type_t t = 0; t < entity_ctx->types_count; t++) {
      type_ctx = &entity_ctx->types[t];
      err = entity_config->get_instance_max_func(t, &type_ctx->instances_count);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(type_ctx->instances_count > ECU_ENTITY_INSTANCE_MAX, err = E_FAULT);

      for(ecu_config_common_entity_instance_t i = 0; i < type_ctx->instances_count; i++) {
        instance_ctx = &type_ctx->instances[i];

        instance_ctx->parameters_count = 0;
        BREAK_IF_ACTION(instance_ctx->parameters_count > ECU_ENTITY_PARAMETERS_MAX, err = E_FAULT);

        // TODO: IMPLEMENT
        BREAK_IF(err != E_OK);

      }
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);


  } while(0);

  return err;
}

static error_t ecu_config_common_sensors(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity)
{
  error_t err = E_OK;
  ecu_config_common_entity_ctx_t *entity_ctx;
  ecu_config_common_entity_type_ctx_t *type_ctx;
  ecu_config_common_entity_instance_ctx_t *instance_ctx;
  const ecu_config_common_entity_config_t *entity_config;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(entity != ECU_COMMON_ENTITY_SENSOR, err = E_PARAM);
    entity_ctx = &ecu_config_common_ctx.entities[entity];
    entity_config = &ecu_config_common_ctx.config->entities[entity];

    err = entity_config->get_type_max_func(&entity_ctx->types_count);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(entity_ctx->types_count > ECU_ENTITY_TYPE_MAX, err = E_FAULT);

    for(ecu_config_common_entity_type_t t = 0; t < entity_ctx->types_count; t++) {
      type_ctx = &entity_ctx->types[t];
      err = entity_config->get_instance_max_func(t, &type_ctx->instances_count);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(type_ctx->instances_count > ECU_ENTITY_INSTANCE_MAX, err = E_FAULT);

      for(ecu_config_common_entity_instance_t i = 0; i < type_ctx->instances_count; i++) {
        instance_ctx = &type_ctx->instances[i];

        instance_ctx->parameters_count = 1;
        BREAK_IF_ACTION(instance_ctx->parameters_count > ECU_ENTITY_PARAMETERS_MAX, err = E_FAULT);

        // TODO: IMPLEMENT PROPERLY
        instance_ctx->parameters[0].data_ptr = &ctx->runtime.global.parameters.sensors[t][i];
        BREAK_IF(err != E_OK);
      }
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);


  } while(0);

  return err;
}

static error_t ecu_config_common_modules(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity)
{
  error_t err = E_OK;
  ecu_config_common_entity_ctx_t *entity_ctx;
  ecu_config_common_entity_type_ctx_t *type_ctx;
  ecu_config_common_entity_instance_ctx_t *instance_ctx;
  const ecu_config_common_entity_config_t *entity_config;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    BREAK_IF_ACTION(entity != ECU_COMMON_ENTITY_MODULE, err = E_PARAM);
    entity_ctx = &ecu_config_common_ctx.entities[entity];
    entity_config = &ecu_config_common_ctx.config->entities[entity];

    err = entity_config->get_type_max_func(&entity_ctx->types_count);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(entity_ctx->types_count > ECU_ENTITY_TYPE_MAX, err = E_FAULT);

    for(ecu_config_common_entity_type_t t = 0; t < entity_ctx->types_count; t++) {
      type_ctx = &entity_ctx->types[t];
      err = entity_config->get_instance_max_func(t, &type_ctx->instances_count);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(type_ctx->instances_count > ECU_ENTITY_INSTANCE_MAX, err = E_FAULT);

      for(ecu_config_common_entity_instance_t i = 0; i < type_ctx->instances_count; i++) {
        instance_ctx = &type_ctx->instances[i];

        instance_ctx->parameters_count = 0;
        BREAK_IF_ACTION(instance_ctx->parameters_count > ECU_ENTITY_PARAMETERS_MAX, err = E_FAULT);

        // TODO: IMPLEMENT
        BREAK_IF(err != E_OK);

      }
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);


  } while(0);

  return err;
}
