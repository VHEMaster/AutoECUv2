/*
 * isotp_private.h
 *
 *  Created on: Oct 9, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_ISOTP_INC_ISOTP_PRIVATE_H_
#define COMMUNICATION_ISOTP_INC_ISOTP_PRIVATE_H_

#include "isotp.h"

error_t isotp_frame_fifo_push(isotp_frame_fifo_t *fifo, const isotp_frame_t *frame);
error_t isotp_frame_fifo_pull(isotp_frame_fifo_t *fifo, isotp_frame_t *frame);

bool isotp_frame_is_empty(isotp_frame_fifo_t *fifo);
bool isotp_frame_has_data(isotp_frame_fifo_t *fifo);

uint32_t isotp_frame_get_items_count(isotp_frame_fifo_t *fifo);
uint32_t isotp_frame_get_free_space(isotp_frame_fifo_t *fifo);

#endif /* COMMUNICATION_ISOTP_INC_ISOTP_PRIVATE_H_ */
