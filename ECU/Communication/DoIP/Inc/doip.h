/*
 * doip.h
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_DOIP_INC_DOIP_H_
#define COMMUNICATION_DOIP_INC_DOIP_H_

#include "doip_types.h"

error_t doip_init(doip_ctx_t *ctx, const doip_init_ctx_t *init);
error_t doip_configure(doip_ctx_t *ctx, const doip_config_t *config);
void doip_loop(doip_ctx_t *ctx);
error_t doip_reset(doip_ctx_t *ctx);
error_t doip_data_get_error(doip_ctx_t *ctx, doip_error_code_t *code);

/* Transport APIs run in the lwIP main-loop context only. Length is in/out. */
error_t doip_connection_open(doip_ctx_t *ctx, uint8_t *connection);
error_t doip_connection_close(doip_ctx_t *ctx, uint8_t connection);
error_t doip_connection_get_state(doip_ctx_t *ctx, uint8_t connection, doip_state_t *state);
error_t doip_stream_write_upstream(doip_ctx_t *ctx, uint8_t connection, const uint8_t *payload, uint16_t *length);
error_t doip_stream_peek_downstream(doip_ctx_t *ctx, uint8_t connection, uint8_t *payload, uint16_t *length);
error_t doip_stream_consume_downstream(doip_ctx_t *ctx, uint8_t connection, uint16_t length);
error_t doip_datagram_process(doip_ctx_t *ctx, const uint8_t *payload, uint16_t length, uint8_t *response, uint16_t *response_length);
error_t doip_vehicle_announcement(doip_ctx_t *ctx, uint8_t *payload, uint16_t *length);

/* Router APIs retain connection generation and logical addresses for replies. */
error_t doip_data_read_upstream(doip_ctx_t *ctx, doip_message_t *message, uint8_t *payload, uint16_t *length);
error_t doip_data_write_downstream(doip_ctx_t *ctx, const doip_message_t *message, const uint8_t *payload, uint16_t length);
error_t doip_message_is_active(doip_ctx_t *ctx, const doip_message_t *message);

#endif /* COMMUNICATION_DOIP_INC_DOIP_H_ */
