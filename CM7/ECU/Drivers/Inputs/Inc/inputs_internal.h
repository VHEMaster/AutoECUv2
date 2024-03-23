/*
 * inputs_internal.h
 *
 *  Created on: Mar 23, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_INPUTS_INC_INPUTS_INTERNAL_H_
#define DRIVERS_INPUTS_INC_INPUTS_INTERNAL_H_

#include "inputs.h"

typedef struct input_ch_ctx_tag input_ch_ctx_t;
typedef struct input_if_ctx_tag input_if_ctx_t;

typedef struct input_ch_ctx_tag {
    input_ch_id_t id;
    input_if_id_t if_id;
    input_if_ctx_t *if_ptr;

    input_value_t value;
    time_delta_us_t debounce_time;
    time_us_t value_time;
    time_us_t value_change_time;
    input_polling_mode_t polling_mode;
    input_source_t source;

    gpio_t gpio;
    bool gpio_invert;
    volatile input_value_t *val_pointer;

    input_func_ch_get_t func_get;
    input_func_ch_irq_cb_t func_irq_cb;

    void *usrdata;
}input_ch_ctx_t;

typedef struct input_if_ctx_tag {
    input_if_id_t id;
    uint32_t chs_count;
    input_ch_ctx_t *chs_ptrs[INPUTS_CHS_MAX];

    input_func_if_periodic_t func_if_periodic_main;
    input_func_if_periodic_t func_if_periodic_slow;
    input_func_if_periodic_t func_if_periodic_fast;

    time_us_t time_main_last;
    time_us_t time_slow_last;
    time_us_t time_fast_last;

    input_func_if_init_t func_if_init;
    input_func_if_configure_t func_if_configure;

    input_func_ch_init_t func_ch_init;
    input_func_ch_configure_t func_ch_configure;

    input_func_ch_get_t func_ch_get;
    input_func_ch_irq_cb_t func_ch_irq_cb;
    void *usrdata;
}input_if_ctx_t;

typedef struct {
    input_if_ctx_t ifs[INPUTS_IFS_MAX];
    input_ch_ctx_t chs[INPUTS_CHS_MAX];

    uint32_t ifs_count;
    uint32_t chs_count;

}input_ctx_t;

void inputs_internal_loop(input_ctx_t *ctx, input_polling_mode_t polling_mode);
error_t inputs_internal_channel_poll(input_ch_ctx_t *ch, input_polling_mode_t polling_mode);

#endif /* DRIVERS_INPUTS_INC_INPUTS_INTERNAL_H_ */
