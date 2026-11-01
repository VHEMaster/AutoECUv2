/*
 * kwp_types.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_KWP_INC_KWP_TYPES_H_
#define COMMUNICATION_KWP_INC_KWP_TYPES_H_

#include "common.h"
#include "time.h"
#include "versioned_kwp.h"

typedef enum {
  KWP_OK = 0,
  KWP_MAX
}kwp_error_code_t;

typedef struct kwp_ctx_tag kwp_ctx_t;

typedef void (*kwp_error_callback_t)(kwp_ctx_t *ctx, kwp_error_code_t code, void *userdata);

typedef struct {
    kwp_error_callback_t error_callback;
    void *callback_userdata;
}kwp_init_ctx_t;

typedef struct kwp_ctx_tag {
    kwp_config_t config;
    kwp_init_ctx_t init;
    bool initialized;
    bool configured;

    kwp_error_code_t error_code;
    bool reset_trigger;

}kwp_ctx_t;

#endif /* COMMUNICATION_KWP_INC_KWP_TYPES_H_ */
