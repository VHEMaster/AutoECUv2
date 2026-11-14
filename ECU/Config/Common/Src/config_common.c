/*
 * config_common.c
 *
 *  Created on: Nov 9, 2025
 *      Author: VHEMaster
 */

#include "config_common.h"

#ifdef CORE_CM7
#include "config_devices.h"
#include "config_sensors.h"
#include "config_modules.h"
#include "config_timings.h"
#endif /* CORE_CM7 */

#include "config_global.h"

#include "ipc_sharedmemory.h"
#include "config_ipc.h"

#ifdef CORE_CM7
#if (ECU_DEVICE_TYPE_MAX > ECU_ENTITY_TYPE_MAX)
#error "ECU_DEVICE_TYPE_MAX is larger than ECU_ENTITY_TYPE_MAX"
#endif
#if (ECU_SENSOR_TYPE_MAX > ECU_ENTITY_TYPE_MAX)
#error "ECU_SENSOR_TYPE_MAX is larger than ECU_ENTITY_TYPE_MAX"
#endif
#if (ECU_MODULE_TYPE_MAX > ECU_ENTITY_TYPE_MAX)
#error "ECU_MODULE_TYPE_MAX is larger than ECU_ENTITY_TYPE_MAX"
#endif
#if (ECU_TIMING_TYPE_MAX > ECU_ENTITY_TYPE_MAX)
#error "ECU_TIMING_TYPE_MAX is larger than ECU_ENTITY_TYPE_MAX"
#endif

#if (ECU_DEVICE_INSTANCE_MAX > ECU_ENTITY_INSTANCE_MAX)
#error "ECU_DEVICE_INSTANCE_MAX is larger than ECU_ENTITY_INSTANCE_MAX"
#endif
#if (ECU_SENSOR_INSTANCE_MAX > ECU_ENTITY_INSTANCE_MAX)
#error "ECU_SENSOR_INSTANCE_MAX is larger than ECU_ENTITY_INSTANCE_MAX"
#endif
#if (ECU_MODULE_INSTANCE_MAX > ECU_ENTITY_INSTANCE_MAX)
#error "ECU_MODULE_INSTANCE_MAX is larger than ECU_ENTITY_INSTANCE_MAX"
#endif
#if (ECU_TIMING_INSTANCE_MAX > ECU_ENTITY_INSTANCE_MAX)
#error "ECU_TIMING_INSTANCE_MAX is larger than ECU_ENTITY_INSTANCE_MAX"
#endif

static error_t ecu_config_common_entity(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity);

typedef struct ecu_config_common_ctx_tag ecu_config_common_ctx_t;

typedef error_t (*ecu_entity_get_type_max_func_t)(ecu_config_common_entity_type_t *type_max);
typedef error_t (*ecu_entity_get_instance_max_func_t)(ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t *instance_max);
typedef error_t (*ecu_entity_get_instance_max_func_t)(ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t *instance_max);
typedef error_t (*ecu_entity_get_instance_params_read_write_func_t)(ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t instance, ecu_core_runtime_value_ctx_t **params, ecu_runtime_param_index_t *count);
typedef error_t (*ecu_entity_get_type_params_count_read_write_func_t)(ecu_config_common_entity_type_t type, ecu_runtime_param_index_t *count);
#endif /* CORE_CM7 */

#ifdef CORE_CM7
typedef struct {
    ecu_config_common_entity_instance_t instance_limit;
    ecu_entity_get_type_max_func_t get_type_max_func;
    ecu_entity_get_instance_max_func_t get_instance_max_func;
    ecu_entity_get_type_params_count_read_write_func_t get_type_params_count_read_write_funcs[ECU_COMMON_READ_WRITE_MAX];
}ecu_config_common_entity_config_t;

typedef struct {
    ecu_core_runtime_global_instance_parameters_ctx_t *parameters;
} ecu_config_common_entity_instance_ctx_t;

typedef struct {
    ecu_config_common_entity_instance_t instances_count;
    ecu_config_common_entity_instance_ctx_t instances[ECU_ENTITY_INSTANCE_MAX];
}ecu_config_common_entity_type_ctx_t;

typedef struct {
    ecu_config_common_entity_type_t types_count;
    ecu_config_common_entity_type_ctx_t types[ECU_ENTITY_TYPE_MAX];
}ecu_config_common_entity_ctx_t;

typedef struct {
    ecu_config_common_entity_t entities_count;
    ecu_config_common_entity_ctx_t entities[ECU_COMMON_ENTITY_MAX];
}ecu_config_common_layout_t;

typedef struct {
    ecu_core_runtime_value_ctx_t values[ECU_CONFIG_PARAMETERS_MAX];
}ecu_config_common_virtual_ctx_t;
#endif /* CORE_CM7 */

typedef struct {
    ecu_config_common_shareable_layout_ctx_t layout;
#ifdef CORE_CM7
    ecu_config_common_virtual_ctx_t sources[ECU_COMMON_OVERALL_SOURCE_MAX];
#endif /* CORE_CM7 */
#ifdef CORE_CM4
    ecu_core_runtime_value_ctx_t values[0];
#endif /* CORE_CM4 */
}ecu_config_common_shareable_ctx_t;

typedef struct {
    ecu_ipc_sharedmemory_t sharedmemory_index;
#ifdef CORE_CM7
    ecu_config_common_entity_config_t entities[ECU_COMMON_ENTITY_MAX];
#endif /* CORE_CM7 */
}ecu_config_common_config_t;

typedef struct ecu_config_common_ctx_tag {
    const ecu_config_common_config_t *config;
    ipc_sharedmemory_ctx_t *ipc_ctx;
    ecu_config_common_shareable_ctx_t *sharedmemory;
    uint32_t sharedmemory_bytes;

#ifdef CORE_CM7
    ecu_config_common_layout_t layout;

    ecu_config_common_shareable_ctx_t shareable;

    ecu_config_common_virtual_ctx_t *common_ctx_sources[ECU_COMMON_OVERALL_SOURCE_MAX];
    ecu_core_runtime_global_instance_parameters_ctx_t *core_runtime_destinations[ECU_COMMON_OVERALL_SOURCE_MAX][ECU_COMMON_ENTITY_MAX];

    uint32_t params_allocated;
    uint32_t params_free;
#endif /* CORE_CM7 */

    bool ipc_initialized;
}ecu_config_common_ctx_t;

static const ecu_config_common_config_t ecu_config_common_config = {
    .sharedmemory_index = ECU_IPC_SHAREDMEMORY_1,

#ifdef CORE_CM7
    .entities = {
        {
            .instance_limit = ECU_DEVICE_INSTANCE_MAX,
            .get_type_max_func = ecu_devices_get_type_max,
            .get_instance_max_func = ecu_devices_get_instance_max,
            .get_type_params_count_read_write_funcs = {
                ecu_devices_get_type_parameters_count_read,
                ecu_devices_get_type_parameters_count_write,
            },
        }, // ECU_COMMON_ENTITY_DEVICE
        {
            .instance_limit = ECU_SENSOR_INSTANCE_MAX,
            .get_type_max_func = ecu_sensors_get_type_max,
            .get_instance_max_func = ecu_sensors_get_instance_max,
            .get_type_params_count_read_write_funcs = {
                ecu_sensors_get_type_parameters_count_read,
                ecu_sensors_get_type_parameters_count_write,
            },
        }, // ECU_COMMON_ENTITY_SENSOR
        {
            .instance_limit = ECU_MODULE_INSTANCE_MAX,
            .get_type_max_func = ecu_modules_get_type_max,
            .get_instance_max_func = ecu_modules_get_instance_max,
            .get_type_params_count_read_write_funcs = {
                ecu_modules_get_type_parameters_count_read,
                ecu_modules_get_type_parameters_count_write,
            },
        }, // ECU_COMMON_ENTITY_MODULE
        {
            .instance_limit = ECU_TIMING_INSTANCE_MAX,
            .get_type_max_func = ecu_timings_get_type_max,
            .get_instance_max_func = ecu_timings_get_instance_max,
            .get_type_params_count_read_write_funcs = {
                ecu_timings_get_type_parameters_count_read,
                ecu_timings_get_type_parameters_count_write,
            },
        }, // ECU_COMMON_ENTITY_TIMING
    },
#endif /* CORE_CM7 */
};

static RAM_SECTION ecu_config_common_ctx_t ecu_config_common_ctx;

#ifdef CORE_CM7
error_t ecu_config_common_init(ecu_core_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);

    memset(&ecu_config_common_ctx, 0, sizeof(ecu_config_common_ctx_t));
    ecu_config_common_ctx.config = &ecu_config_common_config;
    ecu_config_common_ctx.layout.entities_count = ECU_COMMON_ENTITY_MAX;
    ecu_config_common_ctx.shareable.layout.sources_count = ECU_COMMON_OVERALL_SOURCE_MAX;
    ecu_config_common_ctx.shareable.layout.sources_limit = ECU_COMMON_OVERALL_SOURCE_MAX;
    ecu_config_common_ctx.shareable.layout.source_capacity = ECU_CONFIG_PARAMETERS_MAX;
    ecu_config_common_ctx.shareable.layout.source.entity_limit = ECU_COMMON_ENTITY_MAX;
    ecu_config_common_ctx.shareable.layout.source.entity_count = ecu_config_common_ctx.layout.entities_count;

    for(ecu_config_common_overall_source_t v = 0; v < ECU_COMMON_OVERALL_SOURCE_MAX; v++) {
      ecu_config_common_ctx.common_ctx_sources[v] = &ecu_config_common_ctx.shareable.sources[v];
      if(v != ECU_COMMON_OVERALL_SOURCE_FINAL) {
        ecu_config_common_ctx.core_runtime_destinations[v][ECU_COMMON_ENTITY_DEVICE] = (ecu_core_runtime_global_instance_parameters_ctx_t *)ctx->runtime.global.parameters_virtual[v].devices;
        ecu_config_common_ctx.core_runtime_destinations[v][ECU_COMMON_ENTITY_SENSOR] = (ecu_core_runtime_global_instance_parameters_ctx_t *)ctx->runtime.global.parameters_virtual[v].sensors;
        ecu_config_common_ctx.core_runtime_destinations[v][ECU_COMMON_ENTITY_MODULE] = (ecu_core_runtime_global_instance_parameters_ctx_t *)ctx->runtime.global.parameters_virtual[v].modules;
        ecu_config_common_ctx.core_runtime_destinations[v][ECU_COMMON_ENTITY_TIMING] = (ecu_core_runtime_global_instance_parameters_ctx_t *)ctx->runtime.global.parameters_virtual[v].timings;
      } else {
        ecu_config_common_ctx.core_runtime_destinations[v][ECU_COMMON_ENTITY_DEVICE] = (ecu_core_runtime_global_instance_parameters_ctx_t *)ctx->runtime.global.parameters.devices;
        ecu_config_common_ctx.core_runtime_destinations[v][ECU_COMMON_ENTITY_SENSOR] = (ecu_core_runtime_global_instance_parameters_ctx_t *)ctx->runtime.global.parameters.sensors;
        ecu_config_common_ctx.core_runtime_destinations[v][ECU_COMMON_ENTITY_MODULE] = (ecu_core_runtime_global_instance_parameters_ctx_t *)ctx->runtime.global.parameters.modules;
        ecu_config_common_ctx.core_runtime_destinations[v][ECU_COMMON_ENTITY_TIMING] = (ecu_core_runtime_global_instance_parameters_ctx_t *)ctx->runtime.global.parameters.timings;
      }
    }

    ecu_config_common_ctx.params_allocated = 0;
    ecu_config_common_ctx.params_free = ECU_CONFIG_PARAMETERS_MAX;

    for(ecu_config_common_entity_t e = 0; e < ecu_config_common_ctx.layout.entities_count; e++) {
      err = ecu_config_common_entity(ctx, e);
      BREAK_IF(err != E_OK);
    }
  } while(0);

  return err;
}
#endif /* CORE_CM7 */

void ecu_config_common_ipc_init(void)
{
  error_t err = E_OK;

  do {
#ifdef CORE_CM4
    memset(&ecu_config_common_ctx, 0, sizeof(ecu_config_common_ctx_t));
    ecu_config_common_ctx.config = &ecu_config_common_config;
#endif /* CORE_CM4 */

    err = ecu_ipc_get_sharedmemory_ctx(ecu_config_common_ctx.config->sharedmemory_index, &ecu_config_common_ctx.ipc_ctx);
    BREAK_IF(err != E_OK);

    err = ipc_sharedmemory_lock(ecu_config_common_ctx.ipc_ctx);
    BREAK_IF(err != E_OK);

    err = ipc_sharedmemory_get_buffer(ecu_config_common_ctx.ipc_ctx, (void **)&ecu_config_common_ctx.sharedmemory, &ecu_config_common_ctx.sharedmemory_bytes);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(ecu_config_common_ctx.sharedmemory == NULL, err = E_FAULT);
    BREAK_IF_ACTION(ecu_config_common_ctx.sharedmemory_bytes < sizeof(ecu_config_common_shareable_ctx_t), err = E_FAULT);

#ifdef CORE_CM7
    memcpy(ecu_config_common_ctx.sharedmemory, &ecu_config_common_ctx.shareable, sizeof(ecu_config_common_shareable_ctx_t));
#endif /* CORE_CM7 */

    ecu_config_common_ctx.ipc_initialized = true;

  } while(0);

  if(err != E_OK) {
    BREAKPOINT(0);
  }
}

void ecu_config_common_ipc_loop(void)
{
  error_t err = E_OK;

  do {
    BREAK_IF(ecu_config_common_ctx.ipc_initialized != true);

    // TODO: replace to MDMA transaction in 32-bit WORD mode
    (void)err;
#ifdef CORE_CM7
    CacheClean(&ecu_config_common_ctx.shareable, sizeof(ecu_config_common_shareable_ctx_t));
    memcpy(ecu_config_common_ctx.sharedmemory, &ecu_config_common_ctx.shareable, sizeof(ecu_config_common_shareable_ctx_t));
#endif /* CORE_CM7 */
  } while(0);
}

error_t ecu_config_common_get_entity_max(ecu_config_common_entity_t *entity_max)
{
  error_t err = E_OK;
  ecu_config_common_shareable_ctx_t *shareable;

#ifdef CORE_CM7
  shareable = &ecu_config_common_ctx.shareable;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  shareable = ecu_config_common_ctx.sharedmemory;
#endif /* CORE_CM4 */

  do {
    BREAK_IF_ACTION(entity_max == NULL, err = E_PARAM);

    *entity_max = shareable->layout.source.entity_count;

  } while(0);

  return err;
}

error_t ecu_config_common_get_entity_type_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t *type_max)
{
  error_t err = E_OK;
  ecu_config_common_shareable_ctx_t *shareable;
  ecu_config_common_shareable_layout_entity_ctx_t *entity_ctx;

#ifdef CORE_CM7
  shareable = &ecu_config_common_ctx.shareable;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  shareable = ecu_config_common_ctx.sharedmemory;
#endif /* CORE_CM4 */

  do {
    BREAK_IF_ACTION(entity >= shareable->layout.source.entity_count, err = E_PARAM);
    BREAK_IF_ACTION(type_max == NULL, err = E_PARAM);

    entity_ctx = &shareable->layout.source.entities[entity];
    *type_max = entity_ctx->type_count;

  } while(0);

  return err;
}

error_t ecu_config_common_get_entity_type_instance_max(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t *instance_max)
{
  error_t err = E_OK;
  ecu_config_common_shareable_ctx_t *shareable;
  ecu_config_common_shareable_layout_entity_ctx_t *entity_ctx;
  ecu_config_common_shareable_layout_type_ctx_t *type_ctx;

#ifdef CORE_CM7
  shareable = &ecu_config_common_ctx.shareable;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  shareable = ecu_config_common_ctx.sharedmemory;
#endif /* CORE_CM4 */

  do {
    BREAK_IF_ACTION(entity >= shareable->layout.source.entity_count, err = E_PARAM);
    BREAK_IF_ACTION(instance_max == NULL, err = E_PARAM);

    entity_ctx = &shareable->layout.source.entities[entity];
    BREAK_IF_ACTION(type >= entity_ctx->type_count, err = E_PARAM);

    type_ctx = &entity_ctx->types[type];
    *instance_max = type_ctx->instance_count;

  } while(0);

  return err;
}

error_t ecu_config_common_get_entity_type_params_count(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_read_write_t read_write, ecu_runtime_param_index_t *params_max)
{
  error_t err = E_OK;
  ecu_config_common_shareable_ctx_t *shareable;
  ecu_config_common_shareable_layout_entity_ctx_t *entity_ctx;
  ecu_config_common_shareable_layout_type_ctx_t *type_ctx;

#ifdef CORE_CM7
  shareable = &ecu_config_common_ctx.shareable;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  shareable = ecu_config_common_ctx.sharedmemory;
#endif /* CORE_CM4 */

  do {
    BREAK_IF_ACTION(entity >= shareable->layout.source.entity_count, err = E_PARAM);
    BREAK_IF_ACTION(read_write >= ECU_COMMON_READ_WRITE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(params_max == NULL, err = E_PARAM);

    entity_ctx = &shareable->layout.source.entities[entity];
    BREAK_IF_ACTION(type >= entity_ctx->type_count, err = E_PARAM);

    type_ctx = &entity_ctx->types[type];

    *params_max = type_ctx->readwrite[read_write].params_count;


  } while(0);

  return err;
}

error_t ecu_config_common_get_entity_type_instance_params(ecu_config_common_entity_t entity, ecu_config_common_entity_type_t type, ecu_config_common_entity_instance_t instance, ecu_config_common_read_write_t read_write, ecu_core_runtime_value_ctx_t **params, ecu_runtime_param_index_t *params_max)
{
  error_t err = E_OK;
  ecu_config_common_shareable_ctx_t *shareable;
  ecu_config_common_shareable_layout_entity_ctx_t *entity_ctx;
  ecu_config_common_shareable_layout_type_ctx_t *type_ctx;
  ecu_config_common_shareable_layout_instance_ctx_t *instance_ctx;
  uint32_t param_first_index;
  ecu_config_common_overall_source_t source = ECU_COMMON_OVERALL_SOURCE_FINAL;

#ifdef CORE_CM7
  shareable = &ecu_config_common_ctx.shareable;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  shareable = ecu_config_common_ctx.sharedmemory;
#endif /* CORE_CM4 */

  do {
    BREAK_IF_ACTION(source >= shareable->layout.sources_count, err = E_PARAM);
    BREAK_IF_ACTION(entity >= shareable->layout.source.entity_count, err = E_PARAM);
    BREAK_IF_ACTION(read_write >= ECU_COMMON_READ_WRITE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(params_max == NULL, err = E_PARAM);

    entity_ctx = &shareable->layout.source.entities[entity];
    BREAK_IF_ACTION(type >= entity_ctx->type_count, err = E_PARAM);

    type_ctx = &entity_ctx->types[type];
    BREAK_IF_ACTION(instance >= type_ctx->instance_count, err = E_PARAM);

    instance_ctx = &type_ctx->instances[instance];
    param_first_index = instance_ctx->readwrite[read_write].param_first_index;

    *params_max = type_ctx->readwrite[read_write].params_count;

#ifdef CORE_CM7
    *params = &shareable->sources[ECU_COMMON_OVERALL_SOURCE_FINAL].values[param_first_index];
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    *params = &shareable->values[source * shareable->layout.sources_limit + param_first_index];
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_config_common_get_parameter_ptr_by_id(ecu_config_parameter_id_t id, ecu_core_runtime_value_ctx_t **ptr)
{
  error_t err = E_OK;
  ecu_config_common_shareable_ctx_t *shareable;
  ecu_config_common_shareable_layout_entity_ctx_t *entity_ctx;
  ecu_config_common_shareable_layout_type_ctx_t *type_ctx;
  ecu_config_common_shareable_layout_instance_ctx_t *instance_ctx;
  uint32_t param_first_index;
  ecu_config_common_overall_source_t source = ECU_COMMON_OVERALL_SOURCE_FINAL;

  ecu_config_common_entity_parameter_t parameter = id.bitfield.parameter;
  ecu_config_common_entity_instance_t instance = id.bitfield.instance;
  ecu_config_common_entity_type_t type = id.bitfield.type;
  ecu_config_common_read_write_t read_write = id.bitfield.read_write;
  ecu_config_common_entity_t entity = id.bitfield.entity;

#ifdef CORE_CM7
  shareable = &ecu_config_common_ctx.shareable;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  shareable = ecu_config_common_ctx.sharedmemory;
#endif /* CORE_CM4 */

  do {
    BREAK_IF_ACTION(source >= shareable->layout.sources_count, err = E_PARAM);
    BREAK_IF_ACTION(entity >= shareable->layout.source.entity_count, err = E_PARAM);
    BREAK_IF_ACTION(read_write >= ECU_COMMON_READ_WRITE_MAX, err = E_PARAM);

    entity_ctx = &shareable->layout.source.entities[entity];
    BREAK_IF_ACTION(type >= entity_ctx->type_count, err = E_PARAM);

    type_ctx = &entity_ctx->types[type];
    BREAK_IF_ACTION(instance >= type_ctx->instance_count, err = E_PARAM);

    instance_ctx = &type_ctx->instances[instance];
    param_first_index = instance_ctx->readwrite[read_write].param_first_index;
    BREAK_IF_ACTION(param_first_index >= shareable->layout.source_capacity, err = E_FAULT);

    BREAK_IF_ACTION(parameter >= type_ctx->readwrite[read_write].params_count, err = E_PARAM);

#ifdef CORE_CM7
    *ptr = &shareable->sources[ECU_COMMON_OVERALL_SOURCE_FINAL].values[param_first_index + parameter];
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    *ptr = &shareable->values[source * shareable->layout.sources_limit + param_first_index + parameter];
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

error_t ecu_config_common_get_parameter_value_by_id(ecu_config_parameter_id_t id, ecu_core_runtime_value_ctx_t *ptr)
{
  error_t err = E_OK;
  ecu_config_common_shareable_ctx_t *shareable;
  ecu_config_common_shareable_layout_entity_ctx_t *entity_ctx;
  ecu_config_common_shareable_layout_type_ctx_t *type_ctx;
  ecu_config_common_shareable_layout_instance_ctx_t *instance_ctx;
  uint32_t param_first_index, param_index;
  ecu_config_common_overall_source_t source = ECU_COMMON_OVERALL_SOURCE_FINAL;

  ecu_config_common_entity_parameter_t parameter = id.bitfield.parameter;
  ecu_config_common_entity_instance_t instance = id.bitfield.instance;
  ecu_config_common_entity_type_t type = id.bitfield.type;
  ecu_config_common_read_write_t read_write = id.bitfield.read_write;
  ecu_config_common_entity_t entity = id.bitfield.entity;

#ifdef CORE_CM7
  shareable = &ecu_config_common_ctx.shareable;
#endif /* CORE_CM7 */
#ifdef CORE_CM4
  shareable = ecu_config_common_ctx.sharedmemory;
#endif /* CORE_CM4 */

  do {
    BREAK_IF_ACTION(ptr == NULL, err = E_PARAM);
    BREAK_IF_ACTION(source >= shareable->layout.sources_count, err = E_PARAM);
    BREAK_IF_ACTION(entity >= shareable->layout.source.entity_count, err = E_PARAM);
    BREAK_IF_ACTION(read_write >= ECU_COMMON_READ_WRITE_MAX, err = E_PARAM);

    entity_ctx = &shareable->layout.source.entities[entity];
    BREAK_IF_ACTION(type >= entity_ctx->type_count, err = E_PARAM);

    type_ctx = &entity_ctx->types[type];
    BREAK_IF_ACTION(instance >= type_ctx->instance_count, err = E_PARAM);

    instance_ctx = &type_ctx->instances[instance];
    param_first_index = instance_ctx->readwrite[read_write].param_first_index;
    BREAK_IF_ACTION(param_first_index >= shareable->layout.source_capacity, err = E_FAULT);

    BREAK_IF_ACTION(parameter >= type_ctx->readwrite[read_write].params_count, err = E_PARAM);
    param_index = param_first_index + parameter;

    BREAK_IF_ACTION(param_index >= shareable->layout.source_capacity, err = E_PARAM);

#ifdef CORE_CM7
    *ptr = shareable->sources[ECU_COMMON_OVERALL_SOURCE_FINAL].values[param_index];
#endif /* CORE_CM7 */

#ifdef CORE_CM4
    *ptr = shareable->values[source * shareable->layout.sources_limit + param_index];
#endif /* CORE_CM4 */

  } while(0);

  return err;
}

#ifdef CORE_CM7
static error_t ecu_config_common_entity(ecu_core_ctx_t *ctx, ecu_config_common_entity_t entity)
{
  error_t err = E_OK;
  ecu_config_common_entity_ctx_t *entity_ctx;
  ecu_config_common_entity_type_ctx_t *type_ctx;
  ecu_config_common_entity_instance_ctx_t *instance_ctx;
  const ecu_config_common_entity_config_t *entity_config;
  ecu_core_runtime_global_instance_parameters_ctx_t *parameters;
  ecu_core_runtime_global_instance_parameters_ctx_t *virtual_parameters;
  ecu_config_common_virtual_ctx_t *virtual_ctx;
  ecu_config_common_shareable_layout_entity_ctx_t *shareable_entity;
  ecu_config_common_shareable_layout_type_ctx_t *shareable_type;
  ecu_config_common_shareable_layout_type_readwrite_ctx_t *shareable_type_readwrite;
  ecu_config_common_shareable_layout_instance_readwrite_ctx_t *shareable_instance_readwrite;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    entity_ctx = &ecu_config_common_ctx.layout.entities[entity];
    entity_config = &ecu_config_common_ctx.config->entities[entity];

    err = entity_config->get_type_max_func(&entity_ctx->types_count);
    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(entity_ctx->types_count > ECU_ENTITY_TYPE_MAX, err = E_FAULT);

    shareable_entity = &ecu_config_common_ctx.shareable.layout.source.entities[entity];
    shareable_entity->type_limit = ECU_ENTITY_TYPE_MAX;
    shareable_entity->type_count = entity_ctx->types_count;

    for(ecu_config_common_entity_type_t t = 0; t < entity_ctx->types_count; t++) {
      type_ctx = &entity_ctx->types[t];
      err = entity_config->get_instance_max_func(t, &type_ctx->instances_count);
      BREAK_IF(err != E_OK);
      BREAK_IF_ACTION(type_ctx->instances_count > ECU_ENTITY_INSTANCE_MAX, err = E_FAULT);
      BREAK_IF_ACTION(type_ctx->instances_count > entity_config->instance_limit, err = E_FAULT);

      shareable_type = &shareable_entity->types[t];
      shareable_type->instance_limit = ECU_ENTITY_INSTANCE_MAX;
      shareable_type->instance_count = type_ctx->instances_count;

      for(ecu_config_common_read_write_t rw = 0; rw < ECU_COMMON_READ_WRITE_MAX; rw++) {
        shareable_type_readwrite = &shareable_type->readwrite[rw];
        err = entity_config->get_type_params_count_read_write_funcs[rw](t, &shareable_type_readwrite->params_count);
        BREAK_IF(err != E_OK);
      }
      BREAK_IF(err != E_OK);

      for(ecu_config_common_entity_instance_t i = 0; i < type_ctx->instances_count; i++) {
        instance_ctx = &type_ctx->instances[i];
        parameters = &ecu_config_common_ctx.core_runtime_destinations[ECU_COMMON_OVERALL_SOURCE_FINAL][entity][t * entity_config->instance_limit + i];
        instance_ctx->parameters = parameters;

        for(ecu_config_common_read_write_t rw = 0; rw < ECU_COMMON_READ_WRITE_MAX; rw++) {
          shareable_type_readwrite = &shareable_type->readwrite[rw];
          parameters->count[rw] = shareable_type_readwrite->params_count;

          if(ecu_config_common_ctx.params_free < parameters->count[rw]) {
            err = E_OVERFLOW;
            BREAKPOINT(0);
            break;
          }

          shareable_instance_readwrite = &shareable_type->instances[i].readwrite[rw];
          shareable_instance_readwrite->param_first_index = ecu_config_common_ctx.params_allocated;

          for(ecu_config_common_overall_source_t v = 0; v < ECU_COMMON_OVERALL_SOURCE_MAX; v++) {
            virtual_parameters = &ecu_config_common_ctx.core_runtime_destinations[v][entity][t * entity_config->instance_limit + i];
            virtual_parameters->count[rw] = parameters->count[rw];
            if(virtual_parameters->count[rw] > 0) {
              virtual_ctx = ecu_config_common_ctx.common_ctx_sources[v];
              virtual_parameters->params[rw] = &virtual_ctx->values[ecu_config_common_ctx.params_allocated];
              virtual_parameters->source_index[rw] = ecu_config_common_ctx.params_allocated;
            }
          }
          ecu_config_common_ctx.params_allocated += parameters->count[rw];
          ecu_config_common_ctx.params_free -= parameters->count[rw];
        }
        BREAK_IF(err != E_OK);
      }
      BREAK_IF(err != E_OK);
    }
    BREAK_IF(err != E_OK);


  } while(0);

  return err;
}
#endif /* CORE_CM7 */
