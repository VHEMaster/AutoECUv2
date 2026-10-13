/*
 * signals_types.h
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_
#define COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_

#include "common.h"
#include "time.h"
#include "versioned_signals.h"

#include "signals.h"

typedef struct signals_ctx_tag signals_ctx_t;

typedef struct {

}signals_init_ctx_t;

typedef struct {


}signals_diag_ctx_t;

typedef struct signals_ctx_tag {
    signals_config_t config;
    signals_init_ctx_t init;
    bool initialized;
    bool configured;

    bool reset_trigger;

}signals_ctx_t;

#endif /* COMMUNICATION_SIGNALS_INC_SIGNALS_TYPES_H_ */
