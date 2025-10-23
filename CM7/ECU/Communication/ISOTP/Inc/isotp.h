/*
 * iso-tp.h
 *
 *  Created on: Oct 7, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_ISOTP_INC_ISOTP_H_
#define COMMUNICATION_ISOTP_INC_ISOTP_H_

#include "isotp_types.h"

error_t isotp_init(isotp_ctx_t *ctx, const isotp_init_ctx_t *init);
error_t isotp_configure(isotp_ctx_t *ctx, const isotp_config_t *config);
void isotp_loop(isotp_ctx_t *ctx);

error_t isotp_frame_write_upstream(isotp_ctx_t *ctx, const isotp_frame_t *frame);
error_t isotp_frame_read_downstream(isotp_ctx_t *ctx, isotp_frame_t *frame);

error_t isotp_data_write_downstream(isotp_ctx_t *ctx, const uint8_t *payload, uint16_t length);
error_t isotp_data_read_upstream(isotp_ctx_t *ctx, uint8_t *payload, uint16_t *length);

error_t isotp_data_get_error(isotp_ctx_t *ctx, isotp_error_code_t *code);

error_t isotp_reset(isotp_ctx_t *ctx);

#endif /* COMMUNICATION_ISOTP_INC_ISOTP_H_ */
