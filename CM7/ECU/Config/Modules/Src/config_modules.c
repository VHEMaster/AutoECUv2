/*
 * config_modules.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */



#include "config_modules.h"
#include "compiler.h"
#include "bool.h"

#define ECU_MODULES_MAX (       \
    ECU_MODULE_TIMING_MAX     + \
    ECU_MODULE_ETC_MAX)

typedef enum {
  ECU_MODULE_LOOP_TYPE_MAIN = 0,
  ECU_MODULE_LOOP_TYPE_SLOW,
  ECU_MODULE_LOOP_TYPE_FAST
}ecu_config_module_loop_type_t;

typedef struct {
    ecu_module_loop_func_t loop_main;
    ecu_module_loop_func_t loop_slow;
    ecu_module_loop_func_t loop_fast;
    ecu_module_instance_t instance_max;
}ecu_config_module_if_instance_t;

typedef struct {
    ecu_module_type_t type;
    ecu_module_instance_t instance;
    void *ctx;
    bool initialized;
}ecu_config_module_instance_t;

typedef struct {
    ecu_config_module_if_instance_t interfaces[ECU_MODULE_TYPE_MAX];
    ecu_config_module_instance_t modules[ECU_MODULES_MAX];
}ecu_config_modules_t;

static timing_ctx_t ecu_config_timing_ctx[ECU_MODULE_TIMING_MAX] = {0};
static etc_ctx_t ecu_config_etc_ctx[ECU_MODULE_ETC_MAX] = {0};

static ecu_config_modules_t ecu_config_modules = {
    .interfaces = {
        {
            .loop_slow = (ecu_module_loop_func_t)NULL,
            .loop_main = (ecu_module_loop_func_t)NULL,
            .loop_fast = (ecu_module_loop_func_t)NULL,
            .instance_max = ECU_MODULE_TIMING_MAX,
        }, //ECU_MODULE_TYPE_ETC
        {
            .loop_main = (ecu_module_loop_func_t)NULL,
            .loop_slow = (ecu_module_loop_func_t)etc_loop_slow,
            .loop_fast = (ecu_module_loop_func_t)etc_loop_fast,
            .instance_max = ECU_MODULE_ETC_MAX,
        }, //ECU_MODULE_TYPE_TIMING
    },
    .modules = {
        {
            .type = ECU_MODULE_TYPE_TIMING,
            .instance = ECU_MODULE_TIMING_1,
            .ctx = &ecu_config_timing_ctx[ECU_MODULE_TIMING_1],
        },
        {
            .type = ECU_MODULE_TYPE_ETC,
            .instance = ECU_MODULE_ETC_1,
            .ctx = &ecu_config_etc_ctx[ECU_MODULE_ETC_1],
        },
        {
            .type = ECU_MODULE_TYPE_ETC,
            .instance = ECU_MODULE_ETC_2,
            .ctx = &ecu_config_etc_ctx[ECU_MODULE_ETC_2],
        },
    },
};

error_t ecu_modules_init(void)
{
  error_t err = E_OK;
  ecu_config_module_if_instance_t *interface;
  ecu_config_module_instance_t *module;

  for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
    module = &ecu_config_modules.modules[i];
    module->initialized = false;

    BREAK_IF_ACTION(module->type >= ECU_MODULE_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(module->ctx == NULL, err = E_FAULT);

    interface = &ecu_config_modules.interfaces[module->type];
    BREAK_IF_ACTION(module->instance >= interface->instance_max, err = E_FAULT);
  }

  return err;
}

ITCM_FUNC static void ecu_modules_loop(ecu_config_module_loop_type_t loop_type)
{
  ecu_config_module_if_instance_t *interface;
  ecu_config_module_instance_t *module;
  ecu_module_type_t if_type;

  switch(loop_type) {
    case ECU_MODULE_LOOP_TYPE_FAST:
      for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
        module = &ecu_config_modules.modules[i];
        if(module->initialized == true) {
          if_type = module->type;
          if(if_type < ECU_MODULE_TYPE_MAX) {
            interface = &ecu_config_modules.interfaces[if_type];
            if(interface->loop_fast != NULL && module->ctx != NULL) {
              interface->loop_fast(module->ctx);
            }
          }
        }
      }
      break;
    case ECU_MODULE_LOOP_TYPE_SLOW:
      for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
        module = &ecu_config_modules.modules[i];
        if(module->initialized == true) {
          if_type = module->type;
          if(if_type < ECU_MODULE_TYPE_MAX) {
            interface = &ecu_config_modules.interfaces[if_type];
            if(interface->loop_slow != NULL && module->ctx != NULL) {
              interface->loop_slow(module->ctx);
            }
          }
        }
      }
      break;
    case ECU_MODULE_LOOP_TYPE_MAIN:
      for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
        module = &ecu_config_modules.modules[i];
        if(module->initialized == true) {
          if_type = module->type;
          if(if_type < ECU_MODULE_TYPE_MAX) {
            interface = &ecu_config_modules.interfaces[if_type];
            if(interface->loop_main != NULL && module->ctx != NULL) {
              interface->loop_main(module->ctx);
            }
          }
        }
      }
      break;
    default:
      break;
  }
}

void ecu_modules_loop_main(void)
{
  ecu_modules_loop(ECU_MODULE_LOOP_TYPE_MAIN);
}

void ecu_modules_loop_slow(void)
{
  ecu_modules_loop(ECU_MODULE_LOOP_TYPE_SLOW);
}

ITCM_FUNC void ecu_modules_loop_fast(void)
{
  ecu_modules_loop(ECU_MODULE_LOOP_TYPE_FAST);
}

error_t ecu_modules_get_module_ctx(ecu_module_type_t type, ecu_module_instance_t instance, void **ctx)
{
  error_t err = E_FAULT;
  ecu_config_module_instance_t *module;

  if(ctx == NULL) {
    err = E_PARAM;
  } else {
    for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
      module = &ecu_config_modules.modules[i];
      if(module->type == type && module->instance == instance) {
        *ctx = module->ctx;
        err = E_OK;
        break;
      }
    }
  }

  return err;
}

error_t ecu_modules_set_module_initialized(ecu_module_type_t type, ecu_module_instance_t instance, bool initialized)
{
  error_t err = E_FAULT;
  ecu_config_module_instance_t *module;

  for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
    module = &ecu_config_modules.modules[i];
    if(module->type == type && module->instance == instance) {
      module->initialized = initialized;
      err = E_OK;
      break;
    }
  }

  return err;
}

error_t ecu_modules_get_timing_ctx(ecu_module_timing_t instance, timing_ctx_t **ctx)
{
  return ecu_modules_get_module_ctx(ECU_MODULE_TYPE_TIMING, instance, (void**)ctx);
}

error_t ecu_modules_get_etc_ctx(ecu_module_etc_t instance, etc_ctx_t **ctx)
{
  return ecu_modules_get_module_ctx(ECU_MODULE_TYPE_ETC, instance, (void**)ctx);
}
