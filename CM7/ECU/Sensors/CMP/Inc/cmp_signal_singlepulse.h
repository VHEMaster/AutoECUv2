/*
 * cmp_signal_singlepulse.h
 *
 *  Created on: May 27, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_CMP_INC_CMP_SIGNAL_SINGLEPULSE_H_
#define SENSORS_CMP_INC_CMP_SIGNAL_SINGLEPULSE_H_

#include "cmp.h"

error_t cmp_signal_singlepulse_init(cmp_ctx_t *ctx, cmp_instance_t instance_index, void **usrdata);
void cmp_signal_singlepulse_signal(cmp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata);
void cmp_signal_singlepulse_loop_main(cmp_ctx_t *ctx, void *usrdata);
void cmp_signal_singlepulse_loop_slow(cmp_ctx_t *ctx, void *usrdata);
void cmp_signal_singlepulse_loop_fast(cmp_ctx_t *ctx, void *usrdata);

#endif /* SENSORS_CMP_INC_CMP_SIGNAL_SINGLEPULSE_H_ */
