/*
 * tunerstudio_protocol.h
 *
 *  Created on: Nov 27, 2026
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_PROTOCOL_H_
#define COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_PROTOCOL_H_

#include "tunerstudio_types.h"

/* Called from tunerstudio_loop_main with ctx->lock held. */
void tunerstudio_protocol_loop(tunerstudio_ctx_t *ctx);

#endif /* COMMUNICATION_TUNERSTUDIO_INC_TUNERSTUDIO_PROTOCOL_H_ */
