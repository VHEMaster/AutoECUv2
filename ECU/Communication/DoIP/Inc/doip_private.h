/*
 * doip_private.h
 *
 *  Created on: Nov 18, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_DOIP_INC_DOIP_PRIVATE_H_
#define COMMUNICATION_DOIP_INC_DOIP_PRIVATE_H_

#include "doip_types.h"

uint16_t doip_read_u16(const uint8_t *buffer);
uint32_t doip_read_u32(const uint8_t *buffer);
void doip_write_u16(uint8_t *buffer, uint16_t value);
void doip_write_u32(uint8_t *buffer, uint32_t value);
void doip_header_write(uint8_t *buffer, uint8_t version, uint16_t type, uint32_t length);
void doip_error_set(doip_ctx_t *ctx, doip_error_code_t code);
void doip_connection_clear(doip_connection_t *connection);
error_t doip_frame_push(doip_connection_t *connection, uint16_t type, const uint8_t *payload, uint16_t length);
void doip_receive_header(doip_ctx_t *ctx, doip_connection_t *connection);
void doip_receive_complete(doip_ctx_t *ctx, doip_connection_t *connection);
void doip_routing_response(doip_ctx_t *ctx, doip_connection_t *connection, uint16_t address, uint8_t code);
uint16_t doip_identification_write(doip_ctx_t *ctx, uint8_t version, uint8_t *buffer);

#endif /* COMMUNICATION_DOIP_INC_DOIP_PRIVATE_H_ */
