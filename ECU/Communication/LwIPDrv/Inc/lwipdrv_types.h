/*
 * lwipdrv_types.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_LWIPDRV_INC_LWIPDRV_TYPES_H_
#define COMMUNICATION_LWIPDRV_INC_LWIPDRV_TYPES_H_

#include "common.h"
#include "versioned_lwipdrv.h"

typedef struct lwipdrv_ctx_tag lwipdrv_ctx_t;

typedef struct {
    uint32_t dummy;
}lwipdrv_init_ctx_t;

typedef struct lwipdrv_ctx_tag {
    lwipdrv_config_t config;
    lwipdrv_init_ctx_t init;
    bool initialized;
    bool configured;

    bool reset_trigger;

}lwipdrv_ctx_t;

#endif /* COMMUNICATION_LWIPDRV_INC_LWIPDRV_TYPES_H_ */
