/*
 * obd2.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef COMMUNICATION_OBD2_INC_OBD2_H_
#define COMMUNICATION_OBD2_INC_OBD2_H_

#include "obd2_types.h"

error_t obd2_init(obd2_ctx_t *ctx, const obd2_init_ctx_t *init);
error_t obd2_configure(obd2_ctx_t *ctx, const obd2_config_t *config);
void obd2_loop(obd2_ctx_t *ctx);

error_t obd2_reset(obd2_ctx_t *ctx);

error_t obd2_message_write_upstream(obd2_ctx_t *ctx, const uint8_t *payload, uint16_t length);
error_t obd2_message_read_downstream(obd2_ctx_t *ctx, uint8_t *payload, uint16_t *length);

#endif /* COMMUNICATION_OBD2_INC_OBD2_H_ */
