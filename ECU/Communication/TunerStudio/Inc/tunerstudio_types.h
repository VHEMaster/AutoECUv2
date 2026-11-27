/*
 * tunerstudio_types.h
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_TYPES_H_
#define COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_TYPES_H_

#include "common.h"
#include "versioned_tunerstudio.h"
#include "tunerstudio_arbiter.h"
#include "rs422.h"

typedef struct tunerstudio_ctx_tag tunerstudio_ctx_t;

typedef void (*tunerstudio_error_callback_t)(tunerstudio_ctx_t *ctx, error_t error, void *userdata);

typedef struct {
    tunerstudio_error_callback_t error_callback;
    void *callback_userdata;
    rs422_ctx_t *rs422_ctx[ECU_COMM_RS422_MAX];
}tunerstudio_init_ctx_t;

typedef struct {
    tunerstudio_ctx_t *tunerstudio_ctx;
    rs422_ctx_t *rs422_ctx;
    uint32_t generation;

    bool available;
    error_t error_code;
}tunerstudio_rs422_ctx_t;

typedef struct tunerstudio_ctx_tag {
    tunerstudio_config_t config;
    tunerstudio_init_ctx_t init;
    bool initialized;
    bool configured;
    ATOMIC_LOCK_TYPE lock;

    uint32_t generation;
    tunerstudio_arbiter_t arbiter;
    tunerstudio_rs422_ctx_t rs422;

}tunerstudio_ctx_t;

#endif /* COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_TYPES_H_ */
