/*
 * router_types.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_ROUTER_INC_ROUTER_TYPES_H_
#define COMMUNICATION_ROUTER_INC_ROUTER_TYPES_H_

#include "common.h"
#include "time.h"
#include "versioned_router.h"

typedef enum {
  ROUTER_OK = 0,
  ROUTER_MAX
}router_error_code_t;

typedef struct router_ctx_tag router_ctx_t;

typedef void (*router_error_callback_t)(router_ctx_t *ctx, router_error_code_t code, void *userdata);

typedef struct {
    router_error_callback_t error_callback;
    void *callback_userdata;
}router_init_ctx_t;

typedef struct router_ctx_tag {
    router_config_t config;
    router_init_ctx_t init;
    bool initialized;
    bool configured;

    router_error_code_t error_code;
    bool reset_trigger;

}router_ctx_t;

#endif /* COMMUNICATION_ROUTER_INC_ROUTER_TYPES_H_ */
