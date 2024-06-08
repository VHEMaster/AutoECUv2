/*
 * config_core.c
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */



#include "config_core.h"
#include "compiler.h"
#include "bool.h"

#define ECU_CORE_COMPONENTS_MAX (       \
    ECU_CORE_COMPONENT_CYLINDERS_MAX)

typedef enum {
  ECU_CORE_COMPONENT_LOOP_TYPE_MAIN = 0,
  ECU_CORE_COMPONENT_LOOP_TYPE_SLOW,
  ECU_CORE_COMPONENT_LOOP_TYPE_FAST
}ecu_config_core_component_loop_type_t;

typedef struct {
    ecu_core_component_loop_func_t loop_main;
    ecu_core_component_loop_func_t loop_slow;
    ecu_core_component_loop_func_t loop_fast;
    ecu_core_component_instance_t instance_max;
}ecu_config_core_component_if_instance_t;

typedef struct {
    ecu_core_component_type_t type;
    ecu_core_component_instance_t instance;
    void *ctx;
    bool initialized;
}ecu_config_core_component_instance_t;

typedef struct {
    ecu_config_core_component_if_instance_t interfaces[ECU_CORE_COMPONENT_TYPE_MAX];
    ecu_config_core_component_instance_t core_components[ECU_CORE_COMPONENTS_MAX];
}ecu_config_core_components_t;

static cylinders_ctx_t ecu_config_cylinders_ctx[ECU_CORE_COMPONENT_CYLINDERS_MAX] = {0};

static ecu_config_core_components_t ecu_config_core_components = {
    .interfaces = {
        {
            .loop_main = (ecu_core_component_loop_func_t)cylinders_loop_main,
            .loop_slow = (ecu_core_component_loop_func_t)cylinders_loop_slow,
            .loop_fast = (ecu_core_component_loop_func_t)cylinders_loop_fast,
            .instance_max = ECU_CORE_COMPONENT_CYLINDERS_MAX,
        }, //ECU_CORE_COMPONENT_TYPE_CYLINDERS
    },
    .core_components = {
        {
            .type = ECU_CORE_COMPONENT_TYPE_CYLINDERS,
            .instance = ECU_CORE_COMPONENT_CYLINDERS_1,
            .ctx = &ecu_config_cylinders_ctx[ECU_CORE_COMPONENT_CYLINDERS_1],
        },
    },
};

error_t ecu_core_components_init(void)
{
  error_t err = E_OK;
  ecu_config_core_component_if_instance_t *interface;
  ecu_config_core_component_instance_t *core_component;

  for(int i = 0; i < ITEMSOF(ecu_config_core_components.core_components); i++) {
    core_component = &ecu_config_core_components.core_components[i];
    core_component->initialized = false;

    BREAK_IF_ACTION(core_component->type >= ECU_CORE_COMPONENT_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(core_component->ctx == NULL, err = E_FAULT);

    interface = &ecu_config_core_components.interfaces[core_component->type];
    BREAK_IF_ACTION(core_component->instance >= interface->instance_max, err = E_FAULT);
  }

  return err;
}

ITCM_FUNC static void ecu_core_components_loop(ecu_config_core_component_loop_type_t loop_type)
{
  ecu_config_core_component_if_instance_t *interface;
  ecu_config_core_component_instance_t *core_component;
  ecu_core_component_type_t if_type;

  switch(loop_type) {
    case ECU_CORE_COMPONENT_LOOP_TYPE_FAST:
      for(int i = 0; i < ITEMSOF(ecu_config_core_components.core_components); i++) {
        core_component = &ecu_config_core_components.core_components[i];
        if(core_component->initialized == true) {
          if_type = core_component->type;
          if(if_type < ECU_CORE_COMPONENT_TYPE_MAX) {
            interface = &ecu_config_core_components.interfaces[if_type];
            if(interface->loop_fast != NULL && core_component->ctx != NULL) {
              interface->loop_fast(core_component->ctx);
            }
          }
        }
      }
      break;
    case ECU_CORE_COMPONENT_LOOP_TYPE_SLOW:
      for(int i = 0; i < ITEMSOF(ecu_config_core_components.core_components); i++) {
        core_component = &ecu_config_core_components.core_components[i];
        if(core_component->initialized == true) {
          if_type = core_component->type;
          if(if_type < ECU_CORE_COMPONENT_TYPE_MAX) {
            interface = &ecu_config_core_components.interfaces[if_type];
            if(interface->loop_slow != NULL && core_component->ctx != NULL) {
              interface->loop_slow(core_component->ctx);
            }
          }
        }
      }
      break;
    case ECU_CORE_COMPONENT_LOOP_TYPE_MAIN:
      for(int i = 0; i < ITEMSOF(ecu_config_core_components.core_components); i++) {
        core_component = &ecu_config_core_components.core_components[i];
        if(core_component->initialized == true) {
          if_type = core_component->type;
          if(if_type < ECU_CORE_COMPONENT_TYPE_MAX) {
            interface = &ecu_config_core_components.interfaces[if_type];
            if(interface->loop_main != NULL && core_component->ctx != NULL) {
              interface->loop_main(core_component->ctx);
            }
          }
        }
      }
      break;
    default:
      break;
  }
}

void ecu_core_components_loop_main(void)
{
  ecu_core_components_loop(ECU_CORE_COMPONENT_LOOP_TYPE_MAIN);
}

void ecu_core_components_loop_slow(void)
{
  ecu_core_components_loop(ECU_CORE_COMPONENT_LOOP_TYPE_SLOW);
}

ITCM_FUNC void ecu_core_components_loop_fast(void)
{
  ecu_core_components_loop(ECU_CORE_COMPONENT_LOOP_TYPE_FAST);
}

error_t ecu_core_components_get_ctx(ecu_core_component_type_t type, ecu_core_component_instance_t instance, void **ctx)
{
  error_t err = E_FAULT;
  ecu_config_core_component_instance_t *core_component;

  if(ctx == NULL) {
    err = E_PARAM;
  } else {
    for(int i = 0; i < ITEMSOF(ecu_config_core_components.core_components); i++) {
      core_component = &ecu_config_core_components.core_components[i];
      if(core_component->type == type && core_component->instance == instance) {
        *ctx = core_component->ctx;
        err = E_OK;
        break;
      }
    }
  }

  return err;
}

error_t ecu_core_components_set_initialized(ecu_core_component_type_t type, ecu_core_component_instance_t instance, bool initialized)
{
  error_t err = E_FAULT;
  ecu_config_core_component_instance_t *core_component;

  for(int i = 0; i < ITEMSOF(ecu_config_core_components.core_components); i++) {
    core_component = &ecu_config_core_components.core_components[i];
    if(core_component->type == type && core_component->instance == instance) {
      core_component->initialized = initialized;
      err = E_OK;
      break;
    }
  }

  return err;
}

error_t ecu_core_components_get_cylinders_ctx(ecu_core_component_cylinders_t instance, cylinders_ctx_t **ctx)
{
  return ecu_core_components_get_ctx(ECU_CORE_COMPONENT_TYPE_CYLINDERS, instance, (void**)ctx);
}
