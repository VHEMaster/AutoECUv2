/*
 * inputs.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_INPUTS_INC_INPUTS_H_
#define DRIVERS_INPUTS_INC_INPUTS_H_

#include <stdint.h>
#include "errors.h"
#include "gpio.h"
#include "time.h"

#define INPUTS_IFS_MAX    8u
#define INPUTS_CHS_MAX    64u
#define INPUTS_ANALOG_MULTIPLIER   (1000.0f)
#define INPUTS_ANALOG_MULTIPLIER_R (0.001f)

typedef int32_t input_value_t;
typedef int32_t input_if_id_t;
typedef int32_t input_ch_id_t;
typedef int32_t input_id_t;

typedef enum {
  INPUT_POLLING_MODE_DIRECT = 0,
  INPUT_POLLING_MODE_SLOW,
  INPUT_POLLING_MODE_FAST,
  INPUT_POLLING_MODE_MAIN,
  INPUT_POLLING_MODE_MAX
}input_polling_mode_t;

typedef enum {
  INPUT_SOURCE_FUNC = 0,
  INPUT_SOURCE_GPIO,
  INPUT_SOURCE_PTR,
  INPUT_SOURCE_DIRECT,
  INPUT_SOURCE_MAX
}input_source_t;

typedef error_t (*input_func_if_init_t)(input_if_id_t interface_id, void *usrdata);
typedef error_t (*input_func_if_configure_t)(input_if_id_t interface_id, const void *configuration, void *usrdata);
typedef void (*input_func_if_periodic_t)(input_if_id_t interface_id, time_delta_us_t deltatime, void *usrdata);

typedef error_t (*input_func_ch_init_t)(input_if_id_t interface_id, input_ch_id_t channel_id, void *usrdata);
typedef error_t (*input_func_ch_configure_t)(input_if_id_t interface_id, input_ch_id_t channel_id, const void *configuration, void *usrdata);

typedef error_t (*input_func_ch_get_t)(input_if_id_t interface_id, input_ch_id_t channel_id, input_value_t *value, void *usrdata);
typedef void (*input_func_ch_irq_cb_t)(input_if_id_t interface_id, input_ch_id_t channel_id, input_value_t value, void *usrdata);

typedef struct {
    input_func_if_init_t if_init;
    input_func_if_configure_t if_configure;
    input_func_if_periodic_t if_periodic_main;
    input_func_if_periodic_t if_periodic_slow;
    input_func_if_periodic_t if_periodic_fast;

    input_func_ch_init_t ch_init;
    input_func_ch_configure_t ch_configure;

    input_func_ch_get_t ch_get;
    input_func_ch_irq_cb_t ch_irq_cb;
}input_if_cfg_t;

void inputs_init(void);
void inputs_loop_main(void);
void inputs_loop_slow(void);
void inputs_loop_fast(void);

input_if_id_t input_if_register(const input_if_cfg_t *cfg, void *usrdata);
error_t input_if_configure(input_if_id_t interface_id, const void *configuration);

input_id_t input_ch_register(input_if_id_t interface_id, input_ch_id_t channel_id, void *usrdata);
error_t input_ch_configure(input_id_t channel_id, const void *configuration);
error_t input_ch_register_irq_cb(input_id_t channel_id, input_func_ch_irq_cb_t irq_callback);
error_t input_ch_debounce(input_id_t channel_id, time_delta_us_t debounce_time);
error_t input_ch_polling_mode(input_id_t channel_id, input_polling_mode_t polling_mode);
error_t input_ch_source_gpio(input_id_t channel_id, const gpio_t *gpio, bool invert);
error_t input_ch_source_func(input_id_t channel_id, input_func_ch_get_t func);
error_t input_ch_source_ptr(input_id_t channel_id, volatile const input_value_t *pointer);
error_t input_ch_source_direct(input_id_t channel_id, input_value_t initial_value);

error_t input_get_value(input_id_t channel_id, input_value_t *value, time_delta_us_t *time);
error_t input_set_value(input_id_t channel_id, input_value_t value, bool force_irq);

#endif /* DRIVERS_INPUTS_INC_INPUTS_H_ */
