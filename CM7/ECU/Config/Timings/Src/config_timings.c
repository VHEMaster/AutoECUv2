/*
 * config_timings.c
 *
 *  Created on: Nov 13, 2025
 *      Author: VHEMaster
 */

#include "config_timings.h"
#include "config_global.h"
#include "compiler.h"
#include "bool.h"

#define ECU_TIMINGS_MAX (       \
    ECU_TIMING_BASE_MAX       + \
    ECU_TIMING_IGNITION_MAX   + \
    ECU_TIMING_INJECTION_MAX  + \
    ECU_TIMING_ROUGH_MAX)

typedef enum {
  ECU_TIMING_LOOP_TYPE_MAIN = 0,
  ECU_TIMING_LOOP_TYPE_SLOW,
  ECU_TIMING_LOOP_TYPE_FAST
}ecu_config_timing_loop_type_t;

typedef struct {
    ecu_timing_type_t type;
    ecu_timing_instance_t instance;
    void *ctx;
    bool initialized;
    bool enabled;

    ecu_core_runtime_value_ctx_t *params_read_ptr;
    ecu_core_runtime_value_ctx_t *params_write_ptr;
}ecu_config_timing_instance_t;

typedef struct {
    ecu_timing_loop_func_t loop_main;
    ecu_timing_loop_func_t loop_slow;
    ecu_timing_loop_func_t loop_fast;
    ecu_timing_instance_t instance_max;
    ecu_config_timing_instance_t *instance_first;
    ecu_runtime_param_index_t params_read_count;
    ecu_runtime_param_index_t params_write_count;
}ecu_config_timing_if_instance_t;

typedef struct {
    ecu_config_timing_if_instance_t interfaces[ECU_TIMING_TYPE_MAX];
    ecu_config_timing_instance_t timings[ECU_TIMINGS_MAX];
}ecu_config_timings_t;

static timing_base_ctx_t ecu_config_timing_ctx[ECU_TIMING_BASE_MAX] = {0};
static ignition_ctx_t ecu_config_ignition_ctx[ECU_TIMING_IGNITION_MAX] = {0};
static injection_ctx_t ecu_config_injection_ctx[ECU_TIMING_INJECTION_MAX] = {0};
static rough_ctx_t ecu_config_rough_ctx[ECU_TIMING_ROUGH_MAX] = {0};

static ecu_core_runtime_value_ctx_t ecu_config_timing_base_params_read[ECU_TIMING_BASE_MAX][ECU_TIMING_BASE_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_timing_ignition_params_read[ECU_TIMING_IGNITION_MAX][ECU_TIMING_IGNITION_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_timing_injection_params_read[ECU_TIMING_INJECTION_MAX][ECU_TIMING_INJECTION_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_timing_rough_params_read[ECU_TIMING_ROUGH_MAX][ECU_TIMING_BASE_READ_PARAM_MAX] = {0};

//static ecu_core_runtime_value_ctx_t ecu_config_timing_base_params_write[ECU_TIMING_BASE_MAX][ECU_TIMING_BASE_WRITE_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_timing_ignition_params_write[ECU_TIMING_IGNITION_MAX][ECU_TIMING_IGNITION_WRITE_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_timing_injection_params_write[ECU_TIMING_INJECTION_MAX][ECU_TIMING_INJECTION_WRITE_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_timing_rough_params_write[ECU_TIMING_BASE_MAX][ECU_TIMING_BASE_WRITE_PARAM_MAX] = {0};

static ecu_config_timings_t ecu_config_timings = {
    .interfaces = {
        {
            .loop_slow = (ecu_timing_loop_func_t)NULL,
            .loop_main = (ecu_timing_loop_func_t)NULL,
            .loop_fast = (ecu_timing_loop_func_t)NULL,
            .instance_max = ECU_TIMING_BASE_MAX,
            .params_read_count = ECU_TIMING_BASE_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_TIMING_TYPE_BASE
        {
            .loop_slow = (ecu_timing_loop_func_t)NULL,
            .loop_main = (ecu_timing_loop_func_t)NULL,
            .loop_fast = (ecu_timing_loop_func_t)NULL,
            .instance_max = ECU_TIMING_IGNITION_MAX,
            .params_read_count = ECU_TIMING_IGNITION_READ_PARAM_MAX,
            .params_write_count = ECU_TIMING_IGNITION_WRITE_PARAM_MAX,
        }, //ECU_TIMING_TYPE_IGNITION
        {
            .loop_slow = (ecu_timing_loop_func_t)NULL,
            .loop_main = (ecu_timing_loop_func_t)NULL,
            .loop_fast = (ecu_timing_loop_func_t)NULL,
            .instance_max = ECU_TIMING_INJECTION_MAX,
            .params_read_count = ECU_TIMING_INJECTION_READ_PARAM_MAX,
            .params_write_count = ECU_TIMING_INJECTION_WRITE_PARAM_MAX,
        }, //ECU_TIMING_TYPE_INJECTION
        {
            .loop_slow = (ecu_timing_loop_func_t)NULL,
            .loop_main = (ecu_timing_loop_func_t)NULL,
            .loop_fast = (ecu_timing_loop_func_t)NULL,
            .instance_max = ECU_TIMING_ROUGH_MAX,
            .params_read_count = ECU_TIMING_ROUGH_READ_PARAM_MAX,
            .params_write_count = 0,
        }, //ECU_TIMING_TYPE_ROUGH
    },
    .timings = {
        {
            .type = ECU_TIMING_TYPE_BASE,
            .instance = ECU_TIMING_BASE_1,
            .ctx = &ecu_config_timing_ctx[ECU_TIMING_BASE_1],
            .params_read_ptr = ecu_config_timing_base_params_read[ECU_TIMING_BASE_1],
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_TIMING_TYPE_IGNITION,
            .instance = ECU_TIMING_IGNITION_1,
            .ctx = &ecu_config_ignition_ctx[ECU_TIMING_IGNITION_1],
            .params_read_ptr = ecu_config_timing_ignition_params_read[ECU_TIMING_IGNITION_1],
            .params_write_ptr = ecu_config_timing_ignition_params_write[ECU_TIMING_IGNITION_1],
        },
        {
            .type = ECU_TIMING_TYPE_INJECTION,
            .instance = ECU_TIMING_INJECTION_1,
            .ctx = &ecu_config_injection_ctx[ECU_TIMING_INJECTION_1],
            .params_read_ptr = ecu_config_timing_injection_params_read[ECU_TIMING_INJECTION_1],
            .params_write_ptr = ecu_config_timing_injection_params_write[ECU_TIMING_INJECTION_1],
        },
        {
            .type = ECU_TIMING_TYPE_ROUGH,
            .instance = ECU_TIMING_ROUGH_1,
            .ctx = &ecu_config_rough_ctx[ECU_TIMING_ROUGH_1],
            .params_read_ptr = ecu_config_timing_rough_params_read[ECU_TIMING_ROUGH_1],
            .params_write_ptr = NULL,
        },
    }
};

error_t ecu_timings_init(void)
{
  error_t err = E_OK;
  ecu_config_timing_if_instance_t *interface;
  ecu_config_timing_instance_t *timing;

  for(int i = 0; i < ITEMSOF(ecu_config_timings.timings); i++) {
    timing = &ecu_config_timings.timings[i];
    timing->initialized = false;

    BREAK_IF_ACTION(timing->type >= ECU_TIMING_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(timing->ctx == NULL, err = E_FAULT);

    interface = &ecu_config_timings.interfaces[timing->type];
    BREAK_IF_ACTION(timing->instance >= interface->instance_max, err = E_FAULT);
  }

  for(int i = 0; i < ITEMSOF(ecu_config_timings.interfaces); i++) {
    interface = &ecu_config_timings.interfaces[i];

    BREAK_IF_ACTION(interface->instance_max > ECU_TIMING_INSTANCE_MAX, err = E_FAULT);
    interface->instance_first = NULL;
    for(int n = 0; n < ITEMSOF(ecu_config_timings.timings); n++) {
      timing = &ecu_config_timings.timings[n];
      if(timing->type == i) {
        BREAK_IF_ACTION(timing->instance != 0, err = E_FAULT);
        interface->instance_first = timing;
        break;
      }
    }
    BREAK_IF(err != E_OK);
  }

  return err;
}

ITCM_FUNC static void ecu_timings_loop(ecu_config_timing_loop_type_t loop_type)
{
  ecu_config_timing_if_instance_t *interface;
  ecu_config_timing_instance_t *timing;
  ecu_timing_type_t if_type;

  switch(loop_type) {
    case ECU_TIMING_LOOP_TYPE_FAST:
      for(int i = 0; i < ITEMSOF(ecu_config_timings.timings); i++) {
        timing = &ecu_config_timings.timings[i];
        if(timing->initialized == true) {
          if_type = timing->type;
          if(if_type < ECU_TIMING_TYPE_MAX) {
            interface = &ecu_config_timings.interfaces[if_type];
            if(interface->loop_fast != NULL && timing->ctx != NULL) {
              interface->loop_fast(timing->ctx);
            }
          }
        }
      }
      break;
    case ECU_TIMING_LOOP_TYPE_SLOW:
      for(int i = 0; i < ITEMSOF(ecu_config_timings.timings); i++) {
        timing = &ecu_config_timings.timings[i];
        if(timing->initialized == true) {
          if_type = timing->type;
          if(if_type < ECU_TIMING_TYPE_MAX) {
            interface = &ecu_config_timings.interfaces[if_type];
            if(interface->loop_slow != NULL && timing->ctx != NULL) {
              interface->loop_slow(timing->ctx);
            }
          }
        }
      }
      break;
    case ECU_TIMING_LOOP_TYPE_MAIN:
      for(int i = 0; i < ITEMSOF(ecu_config_timings.timings); i++) {
        timing = &ecu_config_timings.timings[i];
        if(timing->initialized == true) {
          if_type = timing->type;
          if(if_type < ECU_TIMING_TYPE_MAX) {
            interface = &ecu_config_timings.interfaces[if_type];
            if(interface->loop_main != NULL && timing->ctx != NULL) {
              interface->loop_main(timing->ctx);
            }
          }
        }
      }
      break;
    default:
      break;
  }
}

void ecu_timings_loop_main(void)
{
  ecu_timings_loop(ECU_TIMING_LOOP_TYPE_MAIN);
}

void ecu_timings_loop_slow(void)
{
  ecu_timings_loop(ECU_TIMING_LOOP_TYPE_SLOW);
}

ITCM_FUNC void ecu_timings_loop_fast(void)
{
  ecu_timings_loop(ECU_TIMING_LOOP_TYPE_FAST);
}

error_t ecu_timings_get_timing_ctx(ecu_timing_type_t type, ecu_timing_instance_t instance, void **ctx)
{
  error_t err = E_FAULT;
  const ecu_config_timing_instance_t *timing;

  if(ctx == NULL) {
    err = E_PARAM;
  } else {
    for(int i = 0; i < ITEMSOF(ecu_config_timings.timings); i++) {
      timing = &ecu_config_timings.timings[i];
      if(timing->type == type && timing->instance == instance) {
        *ctx = timing->ctx;
        err = E_OK;
        break;
      }
    }
  }

  return err;
}

error_t ecu_timings_set_timing_initialized(ecu_timing_type_t type, ecu_timing_instance_t instance, bool initialized)
{
  error_t err = E_FAULT;
  ecu_config_timing_instance_t *ctx;
  ecu_config_timing_if_instance_t *interface;

  do {
    BREAK_IF(type >= ECU_TIMING_TYPE_MAX);
    interface = &ecu_config_timings.interfaces[type];
    BREAK_IF(instance >= interface->instance_max);

    ctx = ecu_config_timings.interfaces[type].instance_first;
    if(ctx != NULL) {
      ctx = &ctx[instance];
      if(ctx->type == type && ctx->instance == instance) {
        ctx->initialized = initialized;
        err = E_OK;
      }
    }
  } while(0);

  return err;
}

error_t ecu_timings_get_timing_initialized(ecu_timing_type_t type, ecu_timing_instance_t instance, bool *initialized)
{
  error_t err = E_FAULT;
  const ecu_config_timing_instance_t *ctx;
  const ecu_config_timing_if_instance_t *interface;

  do {
    BREAK_IF(type >= ECU_TIMING_TYPE_MAX);
    interface = &ecu_config_timings.interfaces[type];
    BREAK_IF(instance >= interface->instance_max);

    ctx = interface->instance_first;
    if(ctx != NULL) {
      ctx = &ctx[instance];
      if(ctx->type == type && ctx->instance == instance) {
        *initialized = ctx->initialized;
        err = E_OK;
      }
    }
  } while(0);

  return err;
}

error_t ecu_timings_set_timing_enabled(ecu_timing_type_t type, ecu_timing_instance_t instance, bool enabled)
{
  error_t err = E_FAULT;
  ecu_config_timing_instance_t *ctx;
  ecu_config_timing_if_instance_t *interface;

  do {
    BREAK_IF(type >= ECU_TIMING_TYPE_MAX);
    interface = &ecu_config_timings.interfaces[type];
    BREAK_IF(instance >= interface->instance_max);

    ctx = ecu_config_timings.interfaces[type].instance_first;
    if(ctx != NULL) {
      ctx = &ctx[instance];
      if(ctx->type == type && ctx->instance == instance) {
        ctx->enabled = enabled;
        err = E_OK;
      }
    }
  } while(0);

  return err;
}

error_t ecu_timings_get_timing_enabled(ecu_timing_type_t type, ecu_timing_instance_t instance, bool *enabled)
{
  error_t err = E_FAULT;
  const ecu_config_timing_instance_t *ctx;
  const ecu_config_timing_if_instance_t *interface;

  do {
    BREAK_IF(type >= ECU_TIMING_TYPE_MAX);
    interface = &ecu_config_timings.interfaces[type];
    BREAK_IF(instance >= interface->instance_max);

    ctx = interface->instance_first;
    if(ctx != NULL) {
      ctx = &ctx[instance];
      if(ctx->type == type && ctx->instance == instance) {
        *enabled = ctx->enabled;
        err = E_OK;
      }
    }
  } while(0);

  return err;
}

error_t ecu_timings_get_type_max(ecu_timing_type_t *type_max)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(type_max == NULL, err = E_PARAM);

    *type_max = ECU_TIMING_TYPE_MAX;

  } while(0);

  return err;
}

error_t ecu_timings_get_instance_max(ecu_timing_type_t type, ecu_timing_instance_t *instance_max)
{
  error_t err = E_OK;
  const ecu_config_timing_if_instance_t *interface;

  do {
    BREAK_IF_ACTION(type >= ECU_TIMING_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(instance_max == NULL, err = E_PARAM);

    interface = &ecu_config_timings.interfaces[type];
    *instance_max = interface->instance_max;

  } while(0);

  return err;
}

error_t ecu_timings_get_instance_parameters_read(ecu_timing_type_t type, ecu_timing_instance_t instance, ecu_core_runtime_value_ctx_t **read, ecu_runtime_param_index_t *count)
{
  error_t err = E_OK;
  ecu_config_timing_instance_t *ctx;
  const ecu_config_timing_if_instance_t *interface;

  do {
    BREAK_IF_ACTION(type >= ECU_TIMING_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(read == NULL, err = E_PARAM);
    BREAK_IF_ACTION(count == NULL, err = E_PARAM);

    interface = &ecu_config_timings.interfaces[type];
    BREAK_IF_ACTION(instance >= interface->instance_max, err = E_PARAM);

    ctx = interface->instance_first;
    BREAK_IF_ACTION(ctx == NULL, err = E_NOTRDY);
    ctx = &ctx[instance];

    *count = interface->params_read_count;
    *read = &ctx->params_read_ptr[instance];

  } while(0);

  return err;
}

error_t ecu_timings_get_instance_parameters_write(ecu_timing_type_t type, ecu_timing_instance_t instance, ecu_core_runtime_value_ctx_t **write, ecu_runtime_param_index_t *count)
{
  error_t err = E_OK;
  ecu_config_timing_instance_t *ctx;
  const ecu_config_timing_if_instance_t *interface;

  do {
    BREAK_IF_ACTION(type >= ECU_TIMING_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(write == NULL, err = E_PARAM);
    BREAK_IF_ACTION(count == NULL, err = E_PARAM);

    interface = &ecu_config_timings.interfaces[type];
    BREAK_IF_ACTION(instance >= interface->instance_max, err = E_PARAM);

    ctx = interface->instance_first;
    BREAK_IF_ACTION(ctx == NULL, err = E_NOTRDY);
    ctx = &ctx[instance];

    *count = interface->params_write_count;
    *write = &ctx->params_write_ptr[instance];

  } while(0);

  return err;
}

error_t ecu_timings_get_base_ctx(ecu_timing_base_t instance, timing_base_ctx_t **ctx)
{
  return ecu_timings_get_timing_ctx(ECU_TIMING_TYPE_BASE, instance, (void**)ctx);
}

error_t ecu_timings_get_ignition_ctx(ecu_timing_base_t instance, ignition_ctx_t **ctx)
{
  return ecu_timings_get_timing_ctx(ECU_TIMING_TYPE_IGNITION, instance, (void**)ctx);
}

error_t ecu_timings_get_injection_ctx(ecu_timing_base_t instance, injection_ctx_t **ctx)
{
  return ecu_timings_get_timing_ctx(ECU_TIMING_TYPE_IGNITION, instance, (void**)ctx);
}

error_t ecu_timings_get_rough_ctx(ecu_timing_base_t instance, rough_ctx_t **ctx)
{
  return ecu_timings_get_timing_ctx(ECU_TIMING_TYPE_ROUGH, instance, (void**)ctx);
}
