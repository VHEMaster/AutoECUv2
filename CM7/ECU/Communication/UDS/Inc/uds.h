/*
 * uds.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef COMMUNICATION_UDS_INC_UDS_H_
#define COMMUNICATION_UDS_INC_UDS_H_

#include "uds_types.h"

error_t uds_init(uds_ctx_t *ctx, const uds_init_ctx_t *init);
error_t uds_configure(uds_ctx_t *ctx, const uds_config_t *config);
void uds_loop(uds_ctx_t *ctx);

error_t uds_reset(uds_ctx_t *ctx);

error_t uds_message_write_upstream(uds_ctx_t *ctx, const uint8_t *payload, uint16_t length);
error_t uds_message_read_downstream(uds_ctx_t *ctx, uint8_t *payload, uint16_t *length);

#endif /* COMMUNICATION_UDS_INC_UDS_H_ */
