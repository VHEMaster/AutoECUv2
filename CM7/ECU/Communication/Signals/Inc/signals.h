/*
 * signals.h
 *
 *  Periodic runtime parameter CAN signals
 */

#ifndef COMMUNICATION_SIGNALS_INC_SIGNALS_H_
#define COMMUNICATION_SIGNALS_INC_SIGNALS_H_

#include "signals_types.h"

error_t signals_init(signals_ctx_t *ctx, const signals_init_ctx_t *init);
error_t signals_configure(signals_ctx_t *ctx, const signals_config_t *config);
void signals_loop_comm(signals_ctx_t *ctx);

#endif /* COMMUNICATION_SIGNALS_INC_SIGNALS_H_ */
