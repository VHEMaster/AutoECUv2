/*
 * can_private.h
 *
 *  Created on: Oct 22, 2025
 *      Author: VHEMaster
 */

#ifndef DRIVERS_CAN_INC_CAN_PRIVATE_H_
#define DRIVERS_CAN_INC_CAN_PRIVATE_H_

#include "can.h"

error_t can_private_rx_get_msg(can_ctx_t *ctx, uint32_t rxindex);
error_t can_private_tx_send_msg(can_ctx_t *ctx, const can_message_t *txmessage);
error_t can_private_validate_message_length(can_frame_format_t format, uint32_t length);
error_t can_private_minimum_message_length(can_frame_format_t format, uint32_t req_length, uint32_t *length);

#endif /* DRIVERS_CAN_INC_CAN_PRIVATE_H_ */
