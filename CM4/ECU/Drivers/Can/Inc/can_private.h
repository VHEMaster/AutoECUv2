/*
 * can_private.h
 *
 *  Created on: Oct 22, 2025
 *      Author: VHEMaster
 */

#ifndef DRIVERS_CAN_INC_CAN_PRIVATE_H_
#define DRIVERS_CAN_INC_CAN_PRIVATE_H_

#include "can.h"

error_t can_rx_get_msg(can_ctx_t *ctx, uint32_t rxindex);
error_t can_tx_send_msg(can_ctx_t *ctx, const can_message_t *txmessage);

#endif /* DRIVERS_CAN_INC_CAN_PRIVATE_H_ */
