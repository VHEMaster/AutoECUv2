/*
 * ckp_signal_regular_60_2.h
 *
 *  Created on: May 20, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_CKP_INC_CKP_SIGNAL_REGULAR_60_2_H_
#define SENSORS_CKP_INC_CKP_SIGNAL_REGULAR_60_2_H_

#include "ckp.h"

typedef struct {

}ckp_signal_regular_60_2_ctx_t;

void ckp_signal_regular_60_2_loop_main(ckp_ctx_t *ctx, ckp_signal_regular_60_2_ctx_t *signal_ctx);
void ckp_signal_regular_60_2_loop_slow(ckp_ctx_t *ctx, ckp_signal_regular_60_2_ctx_t *signal_ctx);
void ckp_signal_regular_60_2_loop_fast(ckp_ctx_t *ctx, ckp_signal_regular_60_2_ctx_t *signal_ctx);

#endif /* SENSORS_CKP_INC_CKP_SIGNAL_REGULAR_60_2_H_ */
