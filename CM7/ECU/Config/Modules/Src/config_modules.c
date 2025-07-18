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
    ECU_MODULE_ETC_MAX        + \
    ECU_MODULE_VVT_MAX        + \
    ECU_MODULE_FUELPUMP_MAX   + \
    ECU_MODULE_COOLINGFAN_MAX + \
    ECU_MODULE_IGNPOWER_MAX   + \
    ECU_MODULE_INDICATION_MAX + \
    ECU_MODULE_WGCV_MAX)

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
}ecu_config_module_instance_t;

typedef struct {
    bool initialized;
}ecu_config_module_instance_runtime_t;

typedef struct {
    ecu_config_module_if_instance_t interfaces[ECU_MODULE_TYPE_MAX];
    ecu_config_module_instance_t modules[ECU_MODULES_MAX];
}ecu_config_modules_t;

typedef struct {
    ecu_config_module_instance_runtime_t modules[ECU_MODULES_MAX];
}ecu_config_modules_runtime_t;

static timing_ctx_t ecu_config_timing_ctx[ECU_MODULE_TIMING_MAX] = {0};
static etc_ctx_t ecu_config_etc_ctx[ECU_MODULE_ETC_MAX] = {0};
static vvt_ctx_t ecu_config_vvt_ctx[ECU_MODULE_VVT_MAX] = {0};
static fuelpump_ctx_t ecu_config_fuelpump_ctx[ECU_MODULE_FUELPUMP_MAX] = {0};
static coolingfan_ctx_t ecu_config_coolingfan_ctx[ECU_MODULE_COOLINGFAN_MAX] = {0};
static ignpower_ctx_t ecu_config_ignpower_ctx[ECU_MODULE_IGNPOWER_MAX] = {0};
static indication_ctx_t ecu_config_indication_ctx[ECU_MODULE_INDICATION_MAX] = {0};
static wgcv_ctx_t ecu_config_wgcv_ctx[ECU_MODULE_WGCV_MAX] = {0};

static RAM_SECTION ecu_config_modules_runtime_t ecu_config_modules_runtime = {0};

static const ecu_config_modules_t ecu_config_modules = {
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
        {
            .loop_main = (ecu_module_loop_func_t)NULL,
            .loop_slow = (ecu_module_loop_func_t)vvt_loop_slow,
            .loop_fast = (ecu_module_loop_func_t)NULL,
            .instance_max = ECU_MODULE_VVT_MAX,
        }, //ECU_MODULE_TYPE_VVT
        {
            .loop_main = (ecu_module_loop_func_t)NULL,
            .loop_slow = (ecu_module_loop_func_t)fuelpump_loop_slow,
            .loop_fast = (ecu_module_loop_func_t)NULL,
            .instance_max = ECU_MODULE_FUELPUMP_MAX,
        }, //ECU_MODULE_TYPE_FUELPUMP
        {
            .loop_main = (ecu_module_loop_func_t)NULL,
            .loop_slow = (ecu_module_loop_func_t)coolingfan_loop_slow,
            .loop_fast = (ecu_module_loop_func_t)NULL,
            .instance_max = ECU_MODULE_COOLINGFAN_MAX,
        }, //ECU_MODULE_TYPE_COOLINGFAN
        {
            .loop_main = (ecu_module_loop_func_t)NULL,
            .loop_slow = (ecu_module_loop_func_t)ignpower_loop_slow,
            .loop_fast = (ecu_module_loop_func_t)NULL,
            .instance_max = ECU_MODULE_IGNPOWER_MAX,
        }, //ECU_MODULE_TYPE_IGNPOWER
        {
            .loop_main = (ecu_module_loop_func_t)NULL,
            .loop_slow = (ecu_module_loop_func_t)indication_loop_slow,
            .loop_fast = (ecu_module_loop_func_t)NULL,
            .instance_max = ECU_MODULE_INDICATION_MAX,
        }, //ECU_MODULE_TYPE_INDICATION
        {
            .loop_main = (ecu_module_loop_func_t)NULL,
            .loop_slow = (ecu_module_loop_func_t)wgcv_loop_slow,
            .loop_fast = (ecu_module_loop_func_t)NULL,
            .instance_max = ECU_MODULE_WGCV_MAX,
        }, //ECU_MODULE_TYPE_WGCV
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
        {
            .type = ECU_MODULE_TYPE_VVT,
            .instance = ECU_MODULE_VVT_1,
            .ctx = &ecu_config_vvt_ctx[ECU_MODULE_VVT_1],
        },
        {
            .type = ECU_MODULE_TYPE_VVT,
            .instance = ECU_MODULE_VVT_2,
            .ctx = &ecu_config_vvt_ctx[ECU_MODULE_VVT_2],
        },
        {
            .type = ECU_MODULE_TYPE_VVT,
            .instance = ECU_MODULE_VVT_3,
            .ctx = &ecu_config_vvt_ctx[ECU_MODULE_VVT_3],
        },
        {
            .type = ECU_MODULE_TYPE_VVT,
            .instance = ECU_MODULE_VVT_4,
            .ctx = &ecu_config_vvt_ctx[ECU_MODULE_VVT_4],
        },
        {
            .type = ECU_MODULE_TYPE_FUELPUMP,
            .instance = ECU_MODULE_FUELPUMP_1,
            .ctx = &ecu_config_fuelpump_ctx[ECU_MODULE_FUELPUMP_1],
        },
        {
            .type = ECU_MODULE_TYPE_FUELPUMP,
            .instance = ECU_MODULE_FUELPUMP_2,
            .ctx = &ecu_config_fuelpump_ctx[ECU_MODULE_FUELPUMP_2],
        },
        {
            .type = ECU_MODULE_TYPE_COOLINGFAN,
            .instance = ECU_MODULE_COOLINGFAN_1,
            .ctx = &ecu_config_coolingfan_ctx[ECU_MODULE_COOLINGFAN_1],
        },
        {
            .type = ECU_MODULE_TYPE_COOLINGFAN,
            .instance = ECU_MODULE_COOLINGFAN_2,
            .ctx = &ecu_config_coolingfan_ctx[ECU_MODULE_COOLINGFAN_2],
        },
        {
            .type = ECU_MODULE_TYPE_IGNPOWER,
            .instance = ECU_MODULE_IGNPOWER_1,
            .ctx = &ecu_config_ignpower_ctx[ECU_MODULE_IGNPOWER_1],
        },
        {
            .type = ECU_MODULE_TYPE_INDICATION,
            .instance = ECU_MODULE_INDICATION_1,
            .ctx = &ecu_config_indication_ctx[ECU_MODULE_INDICATION_1],
        },
        {
            .type = ECU_MODULE_TYPE_INDICATION,
            .instance = ECU_MODULE_INDICATION_2,
            .ctx = &ecu_config_indication_ctx[ECU_MODULE_INDICATION_2],
        },
        {
            .type = ECU_MODULE_TYPE_INDICATION,
            .instance = ECU_MODULE_INDICATION_3,
            .ctx = &ecu_config_indication_ctx[ECU_MODULE_INDICATION_3],
        },
        {
            .type = ECU_MODULE_TYPE_INDICATION,
            .instance = ECU_MODULE_INDICATION_4,
            .ctx = &ecu_config_indication_ctx[ECU_MODULE_INDICATION_4],
        },
        {
            .type = ECU_MODULE_TYPE_INDICATION,
            .instance = ECU_MODULE_INDICATION_5,
            .ctx = &ecu_config_indication_ctx[ECU_MODULE_INDICATION_5],
        },
        {
            .type = ECU_MODULE_TYPE_INDICATION,
            .instance = ECU_MODULE_INDICATION_6,
            .ctx = &ecu_config_indication_ctx[ECU_MODULE_INDICATION_6],
        },
        {
            .type = ECU_MODULE_TYPE_WGCV,
            .instance = ECU_MODULE_WGCV_1,
            .ctx = &ecu_config_wgcv_ctx[ECU_MODULE_WGCV_1],
        },
        {
            .type = ECU_MODULE_TYPE_WGCV,
            .instance = ECU_MODULE_WGCV_2,
            .ctx = &ecu_config_wgcv_ctx[ECU_MODULE_WGCV_2],
        },
    },
};

error_t ecu_modules_init(void)
{
  error_t err = E_OK;
  const ecu_config_module_if_instance_t *interface;
  const ecu_config_module_instance_t *module;
  ecu_config_module_instance_runtime_t *module_runtime;

  for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
    module = &ecu_config_modules.modules[i];
    module_runtime = &ecu_config_modules_runtime.modules[i];
    module_runtime->initialized = false;

    BREAK_IF_ACTION(module->type >= ECU_MODULE_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(module->ctx == NULL, err = E_FAULT);

    interface = &ecu_config_modules.interfaces[module->type];
    BREAK_IF_ACTION(module->instance >= interface->instance_max, err = E_FAULT);
  }

  return err;
}

ITCM_FUNC static void ecu_modules_loop(ecu_config_module_loop_type_t loop_type)
{
  const ecu_config_module_if_instance_t *interface;
  const ecu_config_module_instance_t *module;
  ecu_config_module_instance_runtime_t *module_runtime;
  ecu_module_type_t if_type;

  switch(loop_type) {
    case ECU_MODULE_LOOP_TYPE_FAST:
      for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
        module = &ecu_config_modules.modules[i];
        module_runtime = &ecu_config_modules_runtime.modules[i];
        if(module_runtime->initialized == true) {
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
        module_runtime = &ecu_config_modules_runtime.modules[i];
        if(module_runtime->initialized == true) {
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
        module_runtime = &ecu_config_modules_runtime.modules[i];
        if(module_runtime->initialized == true) {
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
  const ecu_config_module_instance_t *module;

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
  const ecu_config_module_instance_t *module;
  ecu_config_module_instance_runtime_t *module_runtime;

  for(int i = 0; i < ITEMSOF(ecu_config_modules.modules); i++) {
    module = &ecu_config_modules.modules[i];
    module_runtime = &ecu_config_modules_runtime.modules[i];
    if(module->type == type && module->instance == instance) {
      module_runtime->initialized = initialized;
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

error_t ecu_modules_get_vvt_ctx(ecu_module_vvt_t instance, vvt_ctx_t **ctx)
{
  return ecu_modules_get_module_ctx(ECU_MODULE_TYPE_VVT, instance, (void**)ctx);
}

error_t ecu_modules_get_fuelpump_ctx(ecu_module_fuelpump_t instance, fuelpump_ctx_t **ctx)
{
  return ecu_modules_get_module_ctx(ECU_MODULE_TYPE_FUELPUMP, instance, (void**)ctx);
}

error_t ecu_modules_get_coolingfan_ctx(ecu_module_coolingfan_t instance, coolingfan_ctx_t **ctx)
{
  return ecu_modules_get_module_ctx(ECU_MODULE_TYPE_COOLINGFAN, instance, (void**)ctx);
}

error_t ecu_modules_get_indication_ctx(ecu_module_indication_t instance, indication_ctx_t **ctx)
{
  return ecu_modules_get_module_ctx(ECU_MODULE_TYPE_INDICATION, instance, (void**)ctx);
}

error_t ecu_modules_get_wgcv_ctx(ecu_module_wgcv_t instance, wgcv_ctx_t **ctx)
{
  return ecu_modules_get_module_ctx(ECU_MODULE_TYPE_WGCV, instance, (void**)ctx);
}
