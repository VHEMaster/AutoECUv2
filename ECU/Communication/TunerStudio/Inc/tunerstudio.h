/*
 * tunerstudio.h
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_H_
#define COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_H_

#include "tunerstudio_types.h"

error_t tunerstudio_init(tunerstudio_ctx_t *ctx, const tunerstudio_init_ctx_t *init);
error_t tunerstudio_configure(tunerstudio_ctx_t *ctx, const tunerstudio_config_t *config);
error_t tunerstudio_reset(tunerstudio_ctx_t *ctx);

void tunerstudio_loop_main(tunerstudio_ctx_t *ctx);

#endif /* COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_H_ */
