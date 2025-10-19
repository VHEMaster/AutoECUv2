/*
 * middlelayer_can.c
 *
 *  Created on: Apr 3, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "middlelayer_can.h"
#include "compiler.h"
#include "config_can.h"

typedef struct {
    can_ctx_t can_if;
    can_cfg_t can_cfg;
}middlelayer_can_if_t;

typedef struct {
    middlelayer_can_if_t ifs[ECU_CAN_IF_MAX];
}middlelayer_can_ctx_t;

static middlelayer_can_ctx_t middlelayer_can_ctx = {0};

static error_t middlelayer_can_if_init(ecu_can_if_enum_t interface)
{
  error_t err = E_OK;
  middlelayer_can_if_t *if_ctx = NULL;

  do {
    if(interface >= ECU_CAN_IF_MAX) {
      err = E_PARAM;
      break;
    }

    if_ctx = &middlelayer_can_ctx.ifs[interface];

    err = ecu_config_can_init();
    if(err != E_OK) {
      break;
    }

    err = ecu_config_can_get_if_cfg(&if_ctx->can_cfg, interface);
    if(err != E_OK) {
      break;
    }

    err = ecu_config_can_register_callbacks(&if_ctx->can_cfg, interface);
    if(err != E_OK) {
      break;
    }

    err = can_init(&if_ctx->can_if, &if_ctx->can_cfg);
    if(err != E_OK) {
      break;
    }

  } while(0);

  return err;
}

void middlelayer_can_init(void)
{
  error_t err = E_OK;

  //TODO: Create DTC of CAN init
  (void)err;

  memset(&middlelayer_can_ctx, 0u, sizeof(middlelayer_can_ctx));

  for(int i = 0; i < ECU_CAN_IF_MAX; i++) {
    err = middlelayer_can_if_init(i);
  }
}

error_t middlelayer_can_get_if(can_ctx_t **can_if, ecu_can_if_enum_t interface)
{
  error_t err = E_OK;

  do {
    if(interface >= ECU_CAN_IF_MAX || can_if == NULL) {
      err = E_PARAM;
      break;
    }

    *can_if = &middlelayer_can_ctx.ifs[interface].can_if;

  } while(0);

  return err;
}

void middlelayer_can_loop_main(void)
{
  for(int i = 0; i < ECU_CAN_IF_MAX; i++) {
    if(middlelayer_can_ctx.ifs[i].can_if.config.handle != NULL) {
      can_loop_main(&middlelayer_can_ctx.ifs[i].can_if);
    }
  }
}

void middlelayer_can_loop_slow(void)
{
  for(int i = 0; i < ECU_CAN_IF_MAX; i++) {
    if(middlelayer_can_ctx.ifs[i].can_if.config.handle != NULL) {
      can_loop_slow(&middlelayer_can_ctx.ifs[i].can_if);
    }
  }
}

ITCM_FUNC void middlelayer_can_loop_fast(void)
{
  for(int i = 0; i < ECU_CAN_IF_MAX; i++) {
    if(middlelayer_can_ctx.ifs[i].can_if.config.handle != NULL) {
      can_loop_fast(&middlelayer_can_ctx.ifs[i].can_if);
    }
  }
}
