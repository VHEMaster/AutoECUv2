/*
 * cmp_signal_patterned.h
 *
 * Cyclic CMP wheel decoder with CKP-phase and VVT candidate matching.
 */
#ifndef SENSORS_CMP_INC_CMP_SIGNAL_PATTERNED_H_
#define SENSORS_CMP_INC_CMP_SIGNAL_PATTERNED_H_

#include "cmp.h"

error_t cmp_signal_patterned_init(cmp_ctx_t *ctx, cmp_instance_t instance_index, void **usrdata);
void cmp_signal_patterned_signal(cmp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata);
void cmp_signal_patterned_loop_main(cmp_ctx_t *ctx, void *usrdata);
void cmp_signal_patterned_loop_slow(cmp_ctx_t *ctx, void *usrdata);
void cmp_signal_patterned_loop_fast(cmp_ctx_t *ctx, void *usrdata);
void cmp_signal_patterned_ckp_update(cmp_ctx_t *ctx, void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);

#endif /* SENSORS_CMP_INC_CMP_SIGNAL_PATTERNED_H_ */
