/*
 * outputs_internal.h
 *
 *  Created on: Mar 24, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_OUTPUTS_INC_OUTPUTS_INTERNAL_H_
#define DRIVERS_OUTPUTS_INC_OUTPUTS_INTERNAL_H_

#include "outputs.h"

typedef enum {
  OUTPUT_PERIODIC_IDLE = 0,
  OUTPUT_PERIODIC_ONCE_ON,
  OUTPUT_PERIODIC_ONCE_OFF,
  OUTPUT_PERIODIC_ACTIVE_ON,
  OUTPUT_PERIODIC_ACTIVE_OFF,
  OUTPUT_PERIODIC_MAX,
}output_periodic_mode_t;

typedef struct output_ch_ctx_tag output_ch_ctx_t;
typedef struct output_if_ctx_tag output_if_ctx_t;

typedef struct output_ch_ctx_tag {
    output_ch_id_t id;
    output_if_id_t if_id;
    output_if_ctx_t *if_ptr;

    output_value_t value;
    output_dest_t dest;

    gpio_t gpio;
    bool gpio_invert;
    volatile output_value_t *val_pointer;

    output_func_ch_set_t func_set;

    output_periodic_mode_t periodic_mode;
    time_delta_us_t periodic_time;
    output_polling_mode_t periodic_polling_mode;
    output_value_t periodic_value_on;
    output_value_t periodic_value_off;
    time_delta_us_t periodic_time_on;
    time_delta_us_t periodic_time_off;


    void *usrdata;
}output_ch_ctx_t;

typedef struct output_if_ctx_tag {
    output_if_id_t id;
    uint32_t chs_count;
    output_ch_ctx_t *chs_ptrs[OUTPUTS_CHS_MAX];

    output_func_if_periodic_t func_if_periodic_main;
    output_func_if_periodic_t func_if_periodic_slow;
    output_func_if_periodic_t func_if_periodic_fast;

    time_us_t time_main_last;
    time_us_t time_slow_last;
    time_us_t time_fast_last;

    output_func_if_init_t func_if_init;
    output_func_if_configure_t func_if_configure;

    output_func_ch_init_t func_ch_init;
    output_func_ch_configure_t func_ch_configure;

    output_func_ch_set_t func_ch_set;
    output_func_ch_get_t func_ch_get;
    void *usrdata;
}output_if_ctx_t;

typedef struct {
    output_if_ctx_t ifs[OUTPUTS_IFS_MAX];
    output_ch_ctx_t chs[OUTPUTS_CHS_MAX];

    uint32_t ifs_count;
    uint32_t chs_count;

}output_ctx_t;

void outputs_internal_loop(output_ctx_t *ctx, output_polling_mode_t polling_mode);
error_t outputs_internal_channel_poll(output_ch_ctx_t *ch, output_polling_mode_t polling_mode);

error_t outputs_internal_set_value(output_ch_ctx_t *ch, output_value_t value);
error_t outputs_internal_get_value(output_ch_ctx_t *ch, output_value_t *value);

#endif /* DRIVERS_OUTPUTS_INC_OUTPUTS_INTERNAL_H_ */
