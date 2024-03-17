/*
 * inputs.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef INC_INPUTS_H_
#define INC_INPUTS_H_

#include <stdint.h>
#include "time.h"

#define INPUTS_IF_MAX          4
#define INPUTS_CH_PER_IF_MAX   16

typedef int32_t input_value_t;
typedef int32_t input_if_id_t;
typedef int32_t input_ch_id_t;
typedef int32_t input_id_t;

typedef void (*input_func_if_init_t)(input_if_id_t interface_id, void *usrdata);
typedef void (*input_func_if_configure_t)(input_if_id_t interface_id, const void *configuration, void *usrdata);
typedef void (*input_func_if_periodic_t)(input_if_id_t interface_id, time_delta_us_t deltatime, void *usrdata);

typedef void (*input_func_ch_init_t)(input_if_id_t interface_id, input_ch_id_t channel_id, void *usrdata);
typedef void (*input_func_ch_configure_t)(input_if_id_t interface_id, input_ch_id_t channel_id, const void *configuration, void *usrdata);

typedef void (*input_func_ch_get_t)(input_if_id_t interface_id, input_ch_id_t channel_id, input_value_t *value, void *usrdata);

typedef struct {
    input_func_if_init_t if_init;
    input_func_if_configure_t if_configure;
    input_func_if_periodic_t if_periodic_main;
    input_func_if_periodic_t if_periodic_slow;
    input_func_if_periodic_t if_periodic_fast;

    input_func_ch_init_t ch_init;
    input_func_ch_configure_t ch_configure;

    input_func_ch_get_t ch_get;

    void *usrdata;
}input_if_ctx_t;

void inputs_init(void);
void inputs_loop_main(void);
void inputs_loop_slow(void);
void inputs_loop_fast(void);

input_if_id_t input_if_register(const input_if_ctx_t *interface, const void *configuration);
void input_if_reconfigure(input_if_id_t interface_id, const void *configuration);

input_id_t input_ch_register(input_if_id_t interface_id, input_ch_id_t channel, const void *configuration);
void input_ch_reconfigure(input_id_t channel_id, const void *configuration);
void input_ch_debounce(input_id_t channel_id, time_delta_us_t debounce_time);

void input_get_value(input_id_t channel_id, input_value_t *value, time_delta_us_t *time);

#endif /* INC_INPUTS_H_ */
