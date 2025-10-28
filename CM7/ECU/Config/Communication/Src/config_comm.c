/*
 * config_comm.c
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#include "config_comm.h"
#include "compiler.h"
#include "bool.h"

#define ECU_COMM_MAX (      \
    ECU_COMM_ROUTER_MAX   + \
    ECU_COMM_CAN_MAX      + \
    ECU_COMM_KWP_MAX      + \
    ECU_COMM_ISOTP_MAX    + \
    ECU_COMM_UDS_MAX      + \
    ECU_COMM_OBD2_MAX)

typedef enum {
  ECU_COMM_LOOP_TYPE_MAIN = 0,
  ECU_COMM_LOOP_TYPE_SLOW,
  ECU_COMM_LOOP_TYPE_COMM
}ecu_config_comm_loop_type_t;

typedef struct {
    ecu_comm_loop_func_t loop_main;
    ecu_comm_loop_func_t loop_slow;
    ecu_comm_loop_func_t loop_comm;
    ecu_comm_instance_t instance_max;
}ecu_config_comm_if_instance_t;

typedef struct {
    ecu_comm_type_t type;
    ecu_comm_instance_t instance;
    void *ctx;
}ecu_config_comm_instance_t;

typedef struct {
    bool initialized;
}ecu_config_comm_instance_runtime_t;

typedef struct {
    ecu_config_comm_if_instance_t interfaces[ECU_COMM_TYPE_MAX];
    ecu_config_comm_instance_t comm[ECU_COMM_MAX];
}ecu_config_comm_t;

typedef struct {
    ecu_config_comm_instance_runtime_t comm[ECU_COMM_MAX];
}ecu_config_comm_runtime_t;

static RAM_SECTION router_ctx_t ecu_config_router_ctx[ECU_COMM_ROUTER_MAX] = {0};
static RAM_SECTION can_ctx_t ecu_config_can_ctx[ECU_COMM_CAN_MAX] = {0};
static RAM_SECTION kwp_ctx_t ecu_config_kwp_ctx[ECU_COMM_KWP_MAX] = {0};
static RAM_SECTION isotp_ctx_t ecu_config_isotp_ctx[ECU_COMM_ISOTP_MAX] = {0};
static RAM_SECTION uds_ctx_t ecu_config_uds_ctx[ECU_COMM_UDS_MAX] = {0};
static RAM_SECTION obd2_ctx_t ecu_config_obd2_ctx[ECU_COMM_OBD2_MAX] = {0};

static RAM_SECTION ecu_config_comm_runtime_t ecu_config_comm_runtime = {0};

static const ecu_config_comm_t ecu_config_comm = {
    .interfaces = {
        {
            .loop_slow = (ecu_comm_loop_func_t)router_loop_slow,
            .loop_main = (ecu_comm_loop_func_t)router_loop_main,
            .loop_comm = (ecu_comm_loop_func_t)router_loop_comm,
            .instance_max = ECU_COMM_ROUTER_MAX,
        }, //ECU_COMM_TYPE_router
        {
            .loop_slow = (ecu_comm_loop_func_t)can_loop_slow,
            .loop_main = (ecu_comm_loop_func_t)can_loop_main,
            .loop_comm = (ecu_comm_loop_func_t)can_loop_comm,
            .instance_max = ECU_COMM_CAN_MAX,
        }, //ECU_COMM_TYPE_CAN
        {
            .loop_slow = (ecu_comm_loop_func_t)kwp_loop_slow,
            .loop_main = (ecu_comm_loop_func_t)kwp_loop_main,
            .loop_comm = (ecu_comm_loop_func_t)kwp_loop_comm,
            .instance_max = ECU_COMM_KWP_MAX,
        }, //ECU_COMM_TYPE_KWP
        {
            .loop_slow = (ecu_comm_loop_func_t)NULL,
            .loop_main = (ecu_comm_loop_func_t)isotp_loop,
            .loop_comm = (ecu_comm_loop_func_t)NULL,
            .instance_max = ECU_COMM_ISOTP_MAX,
        }, //ECU_COMM_TYPE_ISOTP
        {
            .loop_slow = (ecu_comm_loop_func_t)NULL,
            .loop_main = (ecu_comm_loop_func_t)uds_loop,
            .loop_comm = (ecu_comm_loop_func_t)NULL,
            .instance_max = ECU_COMM_UDS_MAX,
        }, //ECU_COMM_TYPE_UDS
        {
            .loop_slow = (ecu_comm_loop_func_t)NULL,
            .loop_main = (ecu_comm_loop_func_t)obd2_loop,
            .loop_comm = (ecu_comm_loop_func_t)NULL,
            .instance_max = ECU_COMM_OBD2_MAX,
        }, //ECU_COMM_TYPE_OBD2
    },
    .comm = {
        {
            .type = ECU_COMM_TYPE_ROUTER,
            .instance = ECU_COMM_ROUTER_1,
            .ctx = &ecu_config_router_ctx[ECU_COMM_ROUTER_1],
        },
        {
            .type = ECU_COMM_TYPE_CAN,
            .instance = ECU_COMM_CAN_1,
            .ctx = &ecu_config_can_ctx[ECU_COMM_CAN_1],
        },
        {
            .type = ECU_COMM_TYPE_KWP,
            .instance = ECU_COMM_KWP_1,
            .ctx = &ecu_config_kwp_ctx[ECU_COMM_KWP_1],
        },
        {
            .type = ECU_COMM_TYPE_ISOTP,
            .instance = ECU_COMM_ISOTP_1,
            .ctx = &ecu_config_isotp_ctx[ECU_COMM_ISOTP_1],
        },
        {
            .type = ECU_COMM_TYPE_ISOTP,
            .instance = ECU_COMM_ISOTP_2,
            .ctx = &ecu_config_isotp_ctx[ECU_COMM_ISOTP_2],
        },
        {
            .type = ECU_COMM_TYPE_UDS,
            .instance = ECU_COMM_UDS_1,
            .ctx = &ecu_config_uds_ctx[ECU_COMM_UDS_1],
        },
        {
            .type = ECU_COMM_TYPE_OBD2,
            .instance = ECU_COMM_OBD2_1,
            .ctx = &ecu_config_obd2_ctx[ECU_COMM_OBD2_1],
        },
    },
};

error_t ecu_comm_init(void)
{
  error_t err = E_OK;
  const ecu_config_comm_if_instance_t *interface;
  const ecu_config_comm_instance_t *comm;
  ecu_config_comm_instance_runtime_t *comm_runtime;

  for(int i = 0; i < ITEMSOF(ecu_config_comm.comm); i++) {
    comm = &ecu_config_comm.comm[i];
    comm_runtime = &ecu_config_comm_runtime.comm[i];
    comm_runtime->initialized = false;

    BREAK_IF_ACTION(comm->type >= ECU_COMM_TYPE_MAX, err = E_FAULT);
    BREAK_IF_ACTION(comm->ctx == NULL, err = E_FAULT);

    interface = &ecu_config_comm.interfaces[comm->type];
    BREAK_IF_ACTION(comm->instance >= interface->instance_max, err = E_FAULT);
  }

  return err;
}

ITCM_FUNC static void ecu_comm_loop(ecu_config_comm_loop_type_t loop_type)
{
  const ecu_config_comm_if_instance_t *interface;
  const ecu_config_comm_instance_t *comm;
  ecu_config_comm_instance_runtime_t *comm_runtime;
  ecu_comm_type_t if_type;

  switch(loop_type) {
    case ECU_COMM_LOOP_TYPE_COMM:
      for(int i = 0; i < ITEMSOF(ecu_config_comm.comm); i++) {
        comm = &ecu_config_comm.comm[i];
        comm_runtime = &ecu_config_comm_runtime.comm[i];
        if(comm_runtime->initialized == true) {
          if_type = comm->type;
          if(if_type < ECU_COMM_TYPE_MAX) {
            interface = &ecu_config_comm.interfaces[if_type];
            if(interface->loop_comm != NULL && comm->ctx != NULL) {
              interface->loop_comm(comm->ctx);
            }
          }
        }
      }
      break;
    case ECU_COMM_LOOP_TYPE_SLOW:
      for(int i = 0; i < ITEMSOF(ecu_config_comm.comm); i++) {
        comm = &ecu_config_comm.comm[i];
        comm_runtime = &ecu_config_comm_runtime.comm[i];
        if(comm_runtime->initialized == true) {
          if_type = comm->type;
          if(if_type < ECU_COMM_TYPE_MAX) {
            interface = &ecu_config_comm.interfaces[if_type];
            if(interface->loop_slow != NULL && comm->ctx != NULL) {
              interface->loop_slow(comm->ctx);
            }
          }
        }
      }
      break;
    case ECU_COMM_LOOP_TYPE_MAIN:
      for(int i = 0; i < ITEMSOF(ecu_config_comm.comm); i++) {
        comm = &ecu_config_comm.comm[i];
        comm_runtime = &ecu_config_comm_runtime.comm[i];
        if(comm_runtime->initialized == true) {
          if_type = comm->type;
          if(if_type < ECU_COMM_TYPE_MAX) {
            interface = &ecu_config_comm.interfaces[if_type];
            if(interface->loop_main != NULL && comm->ctx != NULL) {
              interface->loop_main(comm->ctx);
            }
          }
        }
      }
      break;
    default:
      break;
  }
}

void ecu_comm_loop_main(void)
{
  ecu_comm_loop(ECU_COMM_LOOP_TYPE_MAIN);
}

void ecu_comm_loop_slow(void)
{
  ecu_comm_loop(ECU_COMM_LOOP_TYPE_SLOW);
}

void ecu_comm_loop_comm(void)
{
  ecu_comm_loop(ECU_COMM_LOOP_TYPE_COMM);
}

error_t ecu_comm_get_comm_ctx(ecu_comm_type_t type, ecu_comm_instance_t instance, void **ctx)
{
  error_t err = E_FAULT;
  const ecu_config_comm_instance_t *comm;

  if(ctx == NULL) {
    err = E_PARAM;
  } else {
    for(int i = 0; i < ITEMSOF(ecu_config_comm.comm); i++) {
      comm = &ecu_config_comm.comm[i];
      if(comm->type == type && comm->instance == instance) {
        *ctx = comm->ctx;
        err = E_OK;
        break;
      }
    }
  }

  return err;
}

error_t ecu_comm_set_comm_initialized(ecu_comm_type_t type, ecu_comm_instance_t instance, bool initialized)
{
  error_t err = E_FAULT;
  const ecu_config_comm_instance_t *comm;
  ecu_config_comm_instance_runtime_t *comm_runtime;

  for(int i = 0; i < ITEMSOF(ecu_config_comm.comm); i++) {
    comm = &ecu_config_comm.comm[i];
    comm_runtime = &ecu_config_comm_runtime.comm[i];
    if(comm->type == type && comm->instance == instance) {
      comm_runtime->initialized = initialized;
      err = E_OK;
      break;
    }
  }

  return err;
}

error_t ecu_comm_get_router_ctx(ecu_comm_router_t instance, router_ctx_t **ctx)
{
  return ecu_comm_get_comm_ctx(ECU_COMM_TYPE_ROUTER, instance, (void**)ctx);
}

error_t ecu_comm_get_can_ctx(ecu_comm_can_t instance, can_ctx_t **ctx)
{
  return ecu_comm_get_comm_ctx(ECU_COMM_TYPE_CAN, instance, (void**)ctx);
}

error_t ecu_comm_get_kwp_ctx(ecu_comm_kwp_t instance, kwp_ctx_t **ctx)
{
  return ecu_comm_get_comm_ctx(ECU_COMM_TYPE_ISOTP, instance, (void**)ctx);
}

error_t ecu_comm_get_isotp_ctx(ecu_comm_isotp_t instance, isotp_ctx_t **ctx)
{
  return ecu_comm_get_comm_ctx(ECU_COMM_TYPE_ISOTP, instance, (void**)ctx);
}

error_t ecu_comm_get_uds_ctx(ecu_comm_uds_t instance, uds_ctx_t **ctx)
{
  return ecu_comm_get_comm_ctx(ECU_COMM_TYPE_UDS, instance, (void**)ctx);
}

error_t ecu_comm_get_obd2_ctx(ecu_comm_obd2_t instance, obd2_ctx_t **ctx)
{
  return ecu_comm_get_comm_ctx(ECU_COMM_TYPE_OBD2, instance, (void**)ctx);
}
