/*
 * l9966_internal.h
 *
 *  Created on: Apr 8, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_L9966_INC_L9966_INTERNAL_H_
#define DEVICES_L9966_INC_L9966_INTERNAL_H_

#include "l9966.h"

error_t l9966_reg_read(l9966_ctx_t *ctx, uint8_t reg, uint16_t *data);
error_t l9966_reg_write(l9966_ctx_t *ctx, uint8_t reg, uint16_t data);

error_t l9966_burst_reg_read(l9966_ctx_t *ctx, uint8_t reg, uint16_t *data, uint8_t len);
error_t l9966_burst_reg_write(l9966_ctx_t *ctx, uint8_t reg, const uint16_t *data, uint8_t len);

#endif /* DEVICES_L9966_INC_L9966_INTERNAL_H_ */
