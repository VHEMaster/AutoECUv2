/*
 * lwip_types.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_LWIP_INC_LWIP_TYPES_H_
#define COMMUNICATION_LWIP_INC_LWIP_TYPES_H_

#include "common.h"
#include "versioned_lwip.h"

typedef struct lwip_ctx_tag lwip_ctx_t;

typedef struct {
    uint32_t dummy;
}lwip_init_ctx_t;

typedef struct lwip_ctx_tag {
    lwip_config_t config;
    lwip_init_ctx_t init;
    bool initialized;
    bool configured;

    bool reset_trigger;

}lwip_ctx_t;

#endif /* COMMUNICATION_LWIP_INC_LWIP_TYPES_H_ */
