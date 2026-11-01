/*
 * l9966_cfg_reg.h
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_L9966_INC_L9966_CFG_REG_H_
#define DEVICES_L9966_INC_L9966_CFG_REG_H_

#include "l9966.h"

error_t l9966_cfg_reg_translate_all(l9966_ctx_t *ctx);
error_t l9966_cfg_reg_translate_sqncr_pudivsel(l9966_ctx_t *ctx, uint8_t channel);

#endif /* DEVICES_L9966_INC_L9966_CFG_REG_H_ */
