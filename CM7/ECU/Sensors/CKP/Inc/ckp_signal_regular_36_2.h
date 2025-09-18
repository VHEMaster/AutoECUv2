/*
 * ckp_signal_regular_36_2.h
 *
 *  Created on: Sep 18, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_CKP_INC_CKP_SIGNAL_REGULAR_36_2_H_
#define SENSORS_CKP_INC_CKP_SIGNAL_REGULAR_36_2_H_

#include "ckp.h"

error_t ckp_signal_regular_36_2_init(ckp_ctx_t *ctx, ckp_instance_t instance_index, void **usrdata);
void ckp_signal_regular_36_2_signal(ckp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata);
void ckp_signal_regular_36_2_loop_main(ckp_ctx_t *ctx, void *usrdata);
void ckp_signal_regular_36_2_loop_slow(ckp_ctx_t *ctx, void *usrdata);
void ckp_signal_regular_36_2_loop_fast(ckp_ctx_t *ctx, void *usrdata);

#endif /* SENSORS_CKP_INC_CKP_SIGNAL_REGULAR_36_2_H_ */
