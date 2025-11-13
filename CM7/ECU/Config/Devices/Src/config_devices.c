/*
 * config_devices.c
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#include "config_devices.h"
#include "config_global.h"
#include "compiler.h"
#include "bool.h"

#define ECU_DEVICES_MAX (     \
    ECU_DEVICE_PULSEDADC_MAX +      \
    ECU_DEVICE_TCS_MAX +      \
    ECU_DEVICE_FLEXIO_MAX +   \
    ECU_DEVICE_WBLS_MAX +   \
    ECU_DEVICE_STEPPER_MAX +  \
    ECU_DEVICE_OUTPUT_MAX +   \
    ECU_DEVICE_MOTOR_MAX +   \
    ECU_DEVICE_FLASH_MAX)

typedef enum {
  ECU_DEVICE_LOOP_TYPE_MAIN = 0,
  ECU_DEVICE_LOOP_TYPE_SLOW,
  ECU_DEVICE_LOOP_TYPE_FAST
}ecu_config_device_loop_type_t;

typedef struct {
    ecu_device_type_t type;
    ecu_device_instance_t instance;
    void *ctx;
    bool initialized;
    bool enabled;

    ecu_core_runtime_value_ctx_t *params_read_ptr;
    ecu_core_runtime_value_ctx_t *params_write_ptr;
}ecu_config_device_instance_t;

typedef struct {
    ecu_device_loop_func_t loop_main;
    ecu_device_loop_func_t loop_slow;
    ecu_device_loop_func_t loop_fast;
    ecu_device_instance_t instance_max;
    ecu_config_device_instance_t *instance_first;
    ecu_runtime_param_index_t params_read_count;
    ecu_runtime_param_index_t params_write_count;
}ecu_config_device_if_instance_t;

typedef struct {
    ecu_config_device_if_instance_t interfaces[ECU_DEVICE_TYPE_MAX];
    ecu_config_device_instance_t devices[ECU_DEVICES_MAX];
}ecu_config_devices_t;

static pulsedadc_ctx_t ecu_config_pulsedadc_ctx[ECU_DEVICE_PULSEDADC_MAX] = {0};
static max31855_ctx_t ecu_config_max31855_ctx[ECU_DEVICE_TCS_MAX] = {0};
static l9966_ctx_t ecu_config_l9966_ctx[ECU_DEVICE_FLEXIO_MAX] = {0};
static cj125_ctx_t ecu_config_cj125_ctx[ECU_DEVICE_WBLS_MAX] = {0};
static tle4729_ctx_t ecu_config_tle4729_ctx[ECU_DEVICE_STEPPER_MAX] = {0};
static tle6240_ctx_t ecu_config_tle6240_ctx[ECU_DEVICE_OUTPUT_MAX] = {0};
static l9960_ctx_t ecu_config_l9960_ctx[ECU_DEVICE_MOTOR_MAX] = {0};
static qspi_ctx_t ecu_config_qspi_ctx[ECU_DEVICE_FLASH_MAX] = {0};

//static ecu_core_runtime_value_ctx_t ecu_config_pulsedadc_params_read[ECU_DEVICE_PULSEDADC_MAX][ECU_DEVICE_PULSEDADC_READ_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_max31855_params_read[ECU_DEVICE_TCS_MAX][ECU_DEVICE_TCS_READ_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_l9966_params_read[ECU_DEVICE_FLEXIO_MAX][ECU_DEVICE_FLEXIO_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_cj125_params_read[ECU_DEVICE_WBLS_MAX][ECU_DEVICE_WBLS_READ_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_tle4729_params_read[ECU_DEVICE_STEPPER_MAX][ECU_DEVICE_STEPPER_READ_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_tle6240_params_read[ECU_DEVICE_OUTPUT_MAX][ECU_DEVICE_OUTPUT_READ_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_l9960_params_read[ECU_DEVICE_MOTOR_MAX][ECU_DEVICE_MOTOR_READ_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_qspi_params_read[ECU_DEVICE_FLASH_MAX][ECU_DEVICE_FLASH_READ_PARAM_MAX] = {0};

//static ecu_core_runtime_value_ctx_t ecu_config_pulsedadc_params_write[ECU_DEVICE_PULSEDADC_MAX][ECU_DEVICE_PULSEDADC_WRITE_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_max31855_params_write[ECU_DEVICE_TCS_MAX][ECU_DEVICE_TCS_WRITE_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_l9966_params_write[ECU_DEVICE_FLEXIO_MAX][ECU_DEVICE_FLEXIO_WRITE_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_cj125_params_write[ECU_DEVICE_WBLS_MAX][ECU_DEVICE_WBLS_WRITE_PARAM_MAX] = {0};
static ecu_core_runtime_value_ctx_t ecu_config_tle4729_params_write[ECU_DEVICE_STEPPER_MAX][ECU_DEVICE_STEPPER_WRITE_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_tle6240_params_write[ECU_DEVICE_OUTPUT_MAX][ECU_DEVICE_OUTPUT_WRITE_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_l9960_params_write[ECU_DEVICE_MOTOR_MAX][ECU_DEVICE_MOTOR_WRITE_PARAM_MAX] = {0};
//static ecu_core_runtime_value_ctx_t ecu_config_qspi_params_write[ECU_DEVICE_FLASH_MAX][ECU_DEVICE_FLASH_WRITE_PARAM_MAX] = {0};

static ecu_config_devices_t ecu_config_devices = {
    .interfaces = {
        {
            //.loop_main = (ecu_device_loop_func_t)pulsedadc_loop_main,
            //.loop_slow = (ecu_device_loop_func_t)pulsedadc_loop_slow,
            //.loop_fast = (ecu_device_loop_func_t)pulsedadc_loop_fast,
            .instance_max = ECU_DEVICE_PULSEDADC_MAX,
            .params_read_count = 0,
            .params_write_count = 0,
        }, //ECU_DEVICE_TYPE_TCS
        {
            .loop_main = (ecu_device_loop_func_t)NULL,
            .loop_slow = (ecu_device_loop_func_t)max31855_loop_slow,
            .loop_fast = (ecu_device_loop_func_t)NULL,
            .instance_max = ECU_DEVICE_TCS_MAX,
            .params_read_count = 0,
            .params_write_count = 0,
        }, //ECU_DEVICE_TYPE_TCS
        {
            .loop_main = (ecu_device_loop_func_t)NULL,
            .loop_slow = (ecu_device_loop_func_t)NULL,
            .loop_fast = (ecu_device_loop_func_t)l9966_loop_fast,
            .instance_max = ECU_DEVICE_FLEXIO_MAX,
            .params_read_count = 0,
            .params_write_count = 0,
        }, //ECU_DEVICE_TYPE_FLEXIO
        {
            .loop_main = (ecu_device_loop_func_t)NULL,
            .loop_slow = (ecu_device_loop_func_t)cj125_loop_slow,
            .loop_fast = (ecu_device_loop_func_t)cj125_loop_fast,
            .instance_max = ECU_DEVICE_WBLS_MAX,
            .params_read_count = ECU_DEVICE_WBLS_READ_PARAM_MAX,
            .params_write_count = ECU_DEVICE_WBLS_WRITE_PARAM_MAX,
        }, //ECU_DEVICE_TYPE_LAMBDA
        {
            .loop_main = (ecu_device_loop_func_t)NULL,
            .loop_slow = (ecu_device_loop_func_t)tle4729_loop_slow,
            .loop_fast = (ecu_device_loop_func_t)tle4729_loop_fast,
            .instance_max = ECU_DEVICE_STEPPER_MAX,
            .params_read_count = ECU_DEVICE_STEPPER_READ_PARAM_MAX,
            .params_write_count = ECU_DEVICE_STEPPER_WRITE_PARAM_MAX,
        }, //ECU_DEVICE_TYPE_STEPPER
        {
            .loop_main = (ecu_device_loop_func_t)NULL,
            .loop_slow = (ecu_device_loop_func_t)NULL,
            .loop_fast = (ecu_device_loop_func_t)tle6240_loop_fast,
            .instance_max = ECU_DEVICE_OUTPUT_MAX,
            .params_read_count = 0,
            .params_write_count = 0,
        }, //ECU_DEVICE_TYPE_OUTPUT
        {
            .loop_main = (ecu_device_loop_func_t)NULL,
            .loop_slow = (ecu_device_loop_func_t)NULL,
            .loop_fast = (ecu_device_loop_func_t)l9960_loop_fast,
            .instance_max = ECU_DEVICE_MOTOR_MAX,
            .params_read_count = 0,
            .params_write_count = 0,
        }, //ECU_DEVICE_TYPE_MOTOR
        {
            .loop_main = (ecu_device_loop_func_t)NULL,
            .loop_slow = (ecu_device_loop_func_t)NULL,
            .loop_fast = (ecu_device_loop_func_t)qspi_loop_fast,
            .instance_max = ECU_DEVICE_FLASH_MAX,
            .params_read_count = 0,
            .params_write_count = 0,
        }, //ECU_DEVICE_TYPE_FLASH
    },
    .devices = {
        {
            .type = ECU_DEVICE_TYPE_PULSEDADC,
            .instance = ECU_DEVICE_PULSEDADC_1,
            .ctx = &ecu_config_pulsedadc_ctx[ECU_DEVICE_PULSEDADC_1],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_PULSEDADC,
            .instance = ECU_DEVICE_PULSEDADC_2,
            .ctx = &ecu_config_pulsedadc_ctx[ECU_DEVICE_PULSEDADC_2],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_TCS,
            .instance = ECU_DEVICE_TCS_1,
            .ctx = &ecu_config_max31855_ctx[ECU_DEVICE_TCS_1],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_TCS,
            .instance = ECU_DEVICE_TCS_2,
            .ctx = &ecu_config_max31855_ctx[ECU_DEVICE_TCS_2],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_FLEXIO,
            .instance = ECU_DEVICE_FLEXIO_1,
            .ctx = &ecu_config_l9966_ctx[ECU_DEVICE_FLEXIO_1],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_FLEXIO,
            .instance = ECU_DEVICE_FLEXIO_2,
            .ctx = &ecu_config_l9966_ctx[ECU_DEVICE_FLEXIO_2],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_WBLS,
            .instance = ECU_DEVICE_WBLS_1,
            .ctx = &ecu_config_cj125_ctx[ECU_DEVICE_WBLS_1],
            .params_read_ptr = ecu_config_cj125_params_read[ECU_DEVICE_WBLS_1],
            .params_write_ptr = ecu_config_cj125_params_write[ECU_DEVICE_WBLS_1],
        },
        {
            .type = ECU_DEVICE_TYPE_WBLS,
            .instance = ECU_DEVICE_WBLS_2,
            .ctx = &ecu_config_cj125_ctx[ECU_DEVICE_WBLS_2],
            .params_read_ptr = ecu_config_cj125_params_read[ECU_DEVICE_WBLS_2],
            .params_write_ptr = ecu_config_cj125_params_write[ECU_DEVICE_WBLS_2],
        },
        {
            .type = ECU_DEVICE_TYPE_STEPPER,
            .instance = ECU_DEVICE_STEPPER_1,
            .ctx = &ecu_config_tle4729_ctx[ECU_DEVICE_STEPPER_1],
            .params_read_ptr = ecu_config_tle4729_params_read[ECU_DEVICE_STEPPER_1],
            .params_write_ptr = ecu_config_tle4729_params_write[ECU_DEVICE_STEPPER_1],
        },
        {
            .type = ECU_DEVICE_TYPE_OUTPUT,
            .instance = ECU_DEVICE_OUTPUT_1,
            .ctx = &ecu_config_tle6240_ctx[ECU_DEVICE_OUTPUT_1],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_OUTPUT,
            .instance = ECU_DEVICE_OUTPUT_2,
            .ctx = &ecu_config_tle6240_ctx[ECU_DEVICE_OUTPUT_2],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_MOTOR,
            .instance = ECU_DEVICE_MOTOR_1,
            .ctx = &ecu_config_l9960_ctx[ECU_DEVICE_MOTOR_1],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_MOTOR,
            .instance = ECU_DEVICE_MOTOR_2,
            .ctx = &ecu_config_l9960_ctx[ECU_DEVICE_MOTOR_2],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
        {
            .type = ECU_DEVICE_TYPE_FLASH,
            .instance = ECU_DEVICE_FLASH_1,
            .ctx = &ecu_config_qspi_ctx[ECU_DEVICE_FLASH_1],
            .params_read_ptr = NULL,
            .params_write_ptr = NULL,
        },
    },
};

error_t ecu_devices_init(void)
{
  error_t err = E_OK;
  ecu_config_device_if_instance_t *interface;
  ecu_config_device_instance_t *device;

  for(int i = 0; i < ITEMSOF(ecu_config_devices.devices); i++) {
    device = &ecu_config_devices.devices[i];
    device->initialized = false;

    BREAK_IF_ACTION(device->type >= ECU_DEVICE_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(device->ctx == NULL, err = E_FAULT);

    interface = &ecu_config_devices.interfaces[device->type];
    BREAK_IF_ACTION(device->instance >= interface->instance_max, err = E_FAULT);
  }

  for(int i = 0; i < ITEMSOF(ecu_config_devices.interfaces); i++) {
    interface = &ecu_config_devices.interfaces[i];

    BREAK_IF_ACTION(interface->instance_max > ECU_DEVICE_INSTANCE_MAX, err = E_FAULT);
    interface->instance_first = NULL;
    for(int n = 0; n < ITEMSOF(ecu_config_devices.devices); n++) {
      device = &ecu_config_devices.devices[n];
      if(device->type == i) {
        BREAK_IF_ACTION(device->instance != 0, err = E_FAULT);
        interface->instance_first = device;
        break;
      }
    }
    BREAK_IF(err != E_OK);
  }

  return err;
}

ITCM_FUNC static void ecu_devices_loop(ecu_config_device_loop_type_t loop_type)
{
  ecu_config_device_if_instance_t *interface;
  ecu_config_device_instance_t *device;
  ecu_device_type_t if_type;

  switch(loop_type) {
    case ECU_DEVICE_LOOP_TYPE_FAST:
      for(int i = 0; i < ITEMSOF(ecu_config_devices.devices); i++) {
        device = &ecu_config_devices.devices[i];
        if(device->initialized == true) {
          if_type = device->type;
          if(if_type < ECU_DEVICE_TYPE_MAX) {
            interface = &ecu_config_devices.interfaces[if_type];
            if(interface->loop_fast != NULL && device->ctx != NULL) {
              interface->loop_fast(device->ctx);
            }
          }
        }
      }
      break;
    case ECU_DEVICE_LOOP_TYPE_SLOW:
      for(int i = 0; i < ITEMSOF(ecu_config_devices.devices); i++) {
        device = &ecu_config_devices.devices[i];
        if(device->initialized == true) {
          if_type = device->type;
          if(if_type < ECU_DEVICE_TYPE_MAX) {
            interface = &ecu_config_devices.interfaces[if_type];
            if(interface->loop_slow != NULL && device->ctx != NULL) {
              interface->loop_slow(device->ctx);
            }
          }
        }
      }
      break;
    case ECU_DEVICE_LOOP_TYPE_MAIN:
      for(int i = 0; i < ITEMSOF(ecu_config_devices.devices); i++) {
        device = &ecu_config_devices.devices[i];
        if(device->initialized == true) {
          if_type = device->type;
          if(if_type < ECU_DEVICE_TYPE_MAX) {
            interface = &ecu_config_devices.interfaces[if_type];
            if(interface->loop_main != NULL && device->ctx != NULL) {
              interface->loop_main(device->ctx);
            }
          }
        }
      }
      break;
    default:
      break;
  }
}

void ecu_devices_loop_main(void)
{
  ecu_devices_loop(ECU_DEVICE_LOOP_TYPE_MAIN);
}

void ecu_devices_loop_slow(void)
{
  ecu_devices_loop(ECU_DEVICE_LOOP_TYPE_SLOW);
}

ITCM_FUNC void ecu_devices_loop_fast(void)
{
  ecu_devices_loop(ECU_DEVICE_LOOP_TYPE_FAST);
}

error_t ecu_devices_get_device_ctx(ecu_device_type_t type, ecu_device_instance_t instance, void **ctx)
{
  error_t err = E_FAULT;
  ecu_config_device_instance_t *device;

  if(ctx == NULL) {
    err = E_PARAM;
  } else {
    for(int i = 0; i < ITEMSOF(ecu_config_devices.devices); i++) {
      device = &ecu_config_devices.devices[i];
      if(device->type == type && device->instance == instance) {
        *ctx = device->ctx;
        err = E_OK;
        break;
      }
    }
  }

  return err;
}

error_t ecu_devices_set_device_initialized(ecu_device_type_t type, ecu_device_instance_t instance, bool initialized)
{
  error_t err = E_FAULT;
  ecu_config_device_instance_t *ctx;
  ecu_config_device_if_instance_t *interface;

  do {
    BREAK_IF(type >= ECU_DEVICE_TYPE_MAX);
    interface = &ecu_config_devices.interfaces[type];
    BREAK_IF(instance >= interface->instance_max);

    ctx = ecu_config_devices.interfaces[type].instance_first;
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

error_t ecu_devices_get_device_initialized(ecu_device_type_t type, ecu_device_instance_t instance, bool *initialized)
{
  error_t err = E_FAULT;
  const ecu_config_device_instance_t *ctx;
  const ecu_config_device_if_instance_t *interface;

  do {
    BREAK_IF(type >= ECU_DEVICE_TYPE_MAX);
    interface = &ecu_config_devices.interfaces[type];
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

error_t ecu_devices_set_device_enabled(ecu_device_type_t type, ecu_device_instance_t instance, bool enabled)
{
  error_t err = E_FAULT;
  ecu_config_device_instance_t *ctx;
  ecu_config_device_if_instance_t *interface;

  do {
    BREAK_IF(type >= ECU_DEVICE_TYPE_MAX);
    interface = &ecu_config_devices.interfaces[type];
    BREAK_IF(instance >= interface->instance_max);

    ctx = ecu_config_devices.interfaces[type].instance_first;
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

error_t ecu_devices_get_device_enabled(ecu_device_type_t type, ecu_device_instance_t instance, bool *enabled)
{
  error_t err = E_FAULT;
  const ecu_config_device_instance_t *ctx;
  const ecu_config_device_if_instance_t *interface;

  do {
    BREAK_IF(type >= ECU_DEVICE_TYPE_MAX);
    interface = &ecu_config_devices.interfaces[type];
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

error_t ecu_devices_get_type_max(ecu_device_type_t *type_max)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(type_max == NULL, err = E_PARAM);

    *type_max = ECU_DEVICE_TYPE_MAX;

  } while(0);

  return err;
}

error_t ecu_devices_get_instance_max(ecu_device_type_t type, ecu_device_instance_t *instance_max)
{
  error_t err = E_OK;
  const ecu_config_device_if_instance_t *interface;

  do {
    BREAK_IF_ACTION(type >= ECU_DEVICE_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(instance_max == NULL, err = E_PARAM);

    interface = &ecu_config_devices.interfaces[type];
    *instance_max = interface->instance_max;

  } while(0);

  return err;
}

error_t ecu_devices_get_instance_parameters_read(ecu_device_type_t type, ecu_device_instance_t instance, ecu_core_runtime_value_ctx_t **read, ecu_runtime_param_index_t *count)
{
  error_t err = E_OK;
  ecu_config_device_instance_t *ctx;
  const ecu_config_device_if_instance_t *interface;

  do {
    BREAK_IF_ACTION(type >= ECU_DEVICE_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(read == NULL, err = E_PARAM);
    BREAK_IF_ACTION(count == NULL, err = E_PARAM);

    interface = &ecu_config_devices.interfaces[type];
    BREAK_IF_ACTION(instance >= interface->instance_max, err = E_PARAM);

    ctx = interface->instance_first;
    BREAK_IF_ACTION(ctx == NULL, err = E_NOTRDY);
    ctx = &ctx[instance];

    *count = interface->params_read_count;
    *read = &ctx->params_read_ptr[instance];

  } while(0);

  return err;
}

error_t ecu_devices_get_instance_parameters_write(ecu_device_type_t type, ecu_device_instance_t instance, ecu_core_runtime_value_ctx_t **write, ecu_runtime_param_index_t *count)
{
  error_t err = E_OK;
  ecu_config_device_instance_t *ctx;
  const ecu_config_device_if_instance_t *interface;

  do {
    BREAK_IF_ACTION(type >= ECU_DEVICE_TYPE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(write == NULL, err = E_PARAM);
    BREAK_IF_ACTION(count == NULL, err = E_PARAM);

    interface = &ecu_config_devices.interfaces[type];
    BREAK_IF_ACTION(instance >= interface->instance_max, err = E_PARAM);

    ctx = interface->instance_first;
    BREAK_IF_ACTION(ctx == NULL, err = E_NOTRDY);
    ctx = &ctx[instance];

    *count = interface->params_write_count;
    *write = &ctx->params_write_ptr[instance];

  } while(0);

  return err;
}

error_t ecu_devices_get_pulsedadc_ctx(ecu_device_pulsedadc_t instance, pulsedadc_ctx_t **ctx)
{
  return ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_PULSEDADC, instance, (void**)ctx);
}

error_t ecu_devices_get_tcs_ctx(ecu_device_tcs_t instance, max31855_ctx_t **ctx)
{
  return ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_TCS, instance, (void**)ctx);
}

error_t ecu_devices_get_flexio_ctx(ecu_device_flexio_t instance, l9966_ctx_t **ctx)
{
  return ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_FLEXIO, instance, (void**)ctx);
}

error_t ecu_devices_get_wbls_ctx(ecu_device_wbls_t instance, cj125_ctx_t **ctx)
{
  return ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_WBLS, instance, (void**)ctx);
}

error_t ecu_devices_get_stepper_ctx(ecu_device_stepper_t instance, tle4729_ctx_t **ctx)
{
  return ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_STEPPER, instance, (void**)ctx);
}

error_t ecu_devices_get_output_ctx(ecu_device_output_t instance, tle6240_ctx_t **ctx)
{
  return ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_OUTPUT, instance, (void**)ctx);
}

error_t ecu_devices_get_motor_ctx(ecu_device_motor_t instance, l9960_ctx_t **ctx)
{
  return ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_MOTOR, instance, (void**)ctx);
}

error_t ecu_devices_get_flash_ctx(ecu_device_flash_t instance, qspi_ctx_t **ctx)
{
  return ecu_devices_get_device_ctx(ECU_DEVICE_TYPE_FLASH, instance, (void**)ctx);
}
