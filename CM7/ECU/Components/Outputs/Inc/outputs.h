/*
 * outputs.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef INC_OUTPUTS_H_
#define INC_OUTPUTS_H_

#include <stdint.h>
#include "time.h"

#define OUTPUTS_IF_MAX          4
#define OUTPUTS_CH_PER_IF_MAX   16

typedef int32_t output_value_t;
typedef int32_t output_if_id_t;
typedef int32_t output_ch_id_t;
typedef int32_t output_id_t;

typedef void (*output_func_if_init_t)(output_if_id_t interface_id, void *usrdata);
typedef void (*output_func_if_configure_t)(output_if_id_t interface_id, const void *configuration, void *usrdata);
typedef void (*output_func_if_periodic_t)(output_if_id_t interface_id, time_delta_us_t deltatime, void *usrdata);

typedef void (*output_func_ch_init_t)(output_if_id_t interface_id, output_ch_id_t channel_id, void *usrdata);
typedef void (*output_func_ch_configure_t)(output_if_id_t interface_id, output_ch_id_t channel_id, const void *configuration, void *usrdata);

typedef void (*output_func_ch_set_t)(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata);
typedef void (*output_func_ch_get_t)(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t *value, void *usrdata);

typedef struct {
    output_func_if_init_t if_init;
    output_func_if_configure_t if_configure;
    output_func_if_periodic_t if_periodic_main;
    output_func_if_periodic_t if_periodic_slow;
    output_func_if_periodic_t if_periodic_fast;

    output_func_ch_init_t ch_init;
    output_func_ch_configure_t ch_configure;

    output_func_ch_set_t ch_set;
    output_func_ch_get_t ch_get;

    void *usrdata;
}output_if_ctx_t;

void outputs_init(void);
void outputs_loop_main(void);
void outputs_loop_slow(void);
void outputs_loop_fast(void);

output_if_id_t output_if_register(const output_if_ctx_t *interface, const void *configuration);
void output_if_reconfigure(output_if_id_t interface_id, const void *configuration);

output_id_t output_ch_register(output_if_id_t interface_id, output_ch_id_t channel, const void *configuration);
void output_ch_reconfigure(output_id_t channel_id, const void *configuration);

void output_set_value(output_id_t channel_id, output_value_t value);
void output_get_value(output_id_t channel_id, output_value_t *value, time_delta_us_t *time);

void output_set_periodic(output_id_t channel_id, output_value_t value_on, output_value_t value_off, time_delta_us_t time_on, time_delta_us_t time_off);
void output_set_periodic_once(output_id_t channel_id, output_value_t value_initial, output_value_t value_post, time_delta_us_t time_delay);

#endif /* INC_OUTPUTS_H_ */
