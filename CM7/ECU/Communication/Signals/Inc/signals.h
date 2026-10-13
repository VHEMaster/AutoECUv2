/*
 * signals.h
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_SIGNALS_INC_SIGNALS_H_
#define COMMUNICATION_SIGNALS_INC_SIGNALS_H_

#include "signals_types.h"

error_t signals_init(signals_ctx_t *ctx, const signals_init_ctx_t *init);
error_t signals_configure(signals_ctx_t *ctx, const signals_config_t *config);
void signals_loop_main(signals_ctx_t *ctx);
void signals_loop_slow(signals_ctx_t *ctx);
void signals_loop_comm(signals_ctx_t *ctx);

error_t signals_reset(signals_ctx_t *ctx);

error_t signals_data_write_upstream(signals_ctx_t *ctx, const signals_runtime_messages_fifo_item_t *message);
error_t signals_data_read_downstream(signals_ctx_t *ctx, signals_runtime_messages_fifo_item_t *message);

#endif /* COMMUNICATION_SIGNALS_INC_SIGNALS_H_ */
