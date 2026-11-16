/*
 * rs422_private.h
 *
 *  Created on: 16 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef DRIVERS_RS422_SRC_RS422_PRIVATE_H_
#define DRIVERS_RS422_SRC_RS422_PRIVATE_H_

#include "rs422.h"

error_t rs422_private_tx_copy(rs422_ctx_t *ctx, void *dst, const void *src, uint32_t size);
void rs422_private_tx_process(rs422_ctx_t *ctx);
error_t rs422_private_tx_write(rs422_ctx_t *ctx, const void *buffer, uint32_t length);

#endif /* DRIVERS_RS422_SRC_RS422_PRIVATE_H_ */
