/*
 * tunerstudio_rs422.h
 *
 *  Created on: Nov 26, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_RS422_H_
#define COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_RS422_H_

#include "tunerstudio_types.h"

/* All functions execute in the CM4 main-loop context only. */
void tunerstudio_rs422_init(tunerstudio_rs422_ctx_t *ctx, tunerstudio_ctx_t *tunerstudio_ctx);
void tunerstudio_rs422_select(tunerstudio_rs422_ctx_t *ctx);
void tunerstudio_rs422_loop(tunerstudio_rs422_ctx_t *ctx);

#endif /* COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_RS422_H_ */
