/*
 * signals_private.h
 *
 *  Created on: 13 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_SIGNALS_INC_SIGNALS_PRIVATE_H_
#define COMMUNICATION_SIGNALS_INC_SIGNALS_PRIVATE_H_

#include "signals_types.h"

error_t signals_config_build(signals_ctx_t *ctx);
void signals_handle_upstream(signals_ctx_t *ctx);
void signals_handle_downstream(signals_ctx_t *ctx);

error_t signals_frame_fifo_push(signals_runtime_messages_fifo_t *fifo, const signals_runtime_messages_fifo_item_t *message);
error_t signals_frame_fifo_pull(signals_runtime_messages_fifo_t *fifo, signals_runtime_messages_fifo_item_t *message);

#endif /* COMMUNICATION_SIGNALS_INC_SIGNALS_PRIVATE_H_ */
