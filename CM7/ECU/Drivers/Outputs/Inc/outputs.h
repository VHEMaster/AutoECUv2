/*
 * outputs.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_OUTPUTS_INC_OUTPUTS_H_
#define DRIVERS_OUTPUTS_INC_OUTPUTS_H_

#include <stdint.h>
#include "errors.h"
#include "gpio.h"
#include "time.h"

#define OUTPUTS_IFS_MAX    16u
#define OUTPUTS_CHS_MAX    64u

typedef int32_t output_value_t;
typedef int32_t output_if_id_t;
typedef int32_t output_ch_id_t;
typedef int32_t output_id_t;

typedef enum {
  OUTPUT_DEST_FUNC = 0,
  OUTPUT_DEST_GPIO,
  OUTPUT_DEST_PTR,
  OUTPUT_DEST_VOID,
  OUTPUT_DEST_MAX
}output_dest_t;

typedef enum {
  OUTPUT_POLLING_MODE_SLOW = 0,
  OUTPUT_POLLING_MODE_FAST,
  OUTPUT_POLLING_MODE_MAIN,
  OUTPUT_POLLING_MODE_MAX
}output_polling_mode_t;

typedef error_t (*output_func_if_init_t)(output_if_id_t interface_id, void *usrdata);
typedef error_t (*output_func_if_configure_t)(output_if_id_t interface_id, const void *configuration, void *usrdata);
typedef void (*output_func_if_periodic_t)(output_if_id_t interface_id, time_delta_us_t deltatime, void *usrdata);

typedef error_t (*output_func_ch_init_t)(output_if_id_t interface_id, output_ch_id_t channel_id, void *usrdata);
typedef error_t (*output_func_ch_configure_t)(output_if_id_t interface_id, output_ch_id_t channel_id, const void *configuration, void *usrdata);

typedef error_t (*output_func_ch_set_t)(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata);
typedef error_t (*output_func_ch_get_t)(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t *value, void *usrdata);

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
}output_if_cfg_t;

error_t outputs_init(void);
void outputs_loop_main(void);
void outputs_loop_slow(void);
void outputs_loop_fast(void);

output_if_id_t output_if_register(const output_if_cfg_t *cfg, void *usrdata);
error_t output_if_configure(output_if_id_t interface_id, const void *configuration);

output_id_t output_ch_register(output_if_id_t interface_id, output_ch_id_t channel_id, void *usrdata);
error_t output_ch_configure(output_id_t channel_id, const void *configuration);

error_t output_ch_dest_gpio(output_id_t channel_id, const gpio_t *gpio, bool invert);
error_t output_ch_dest_func(output_id_t channel_id, output_func_ch_set_t func);
error_t output_ch_dest_ptr(output_id_t channel_id, volatile output_value_t *pointer);
error_t output_ch_dest_void(output_id_t channel_id);

error_t output_ch_periodic_polling_mode(output_id_t channel_id, output_polling_mode_t polling_mode);
error_t output_set_periodic(output_id_t channel_id, bool once, output_value_t value_on, output_value_t value_off, time_delta_us_t time_on, time_delta_us_t time_off);
error_t output_reset_periodic(output_id_t channel_id, output_value_t value);

error_t output_set_value(output_id_t channel_id, output_value_t value);
error_t output_get_value(output_id_t channel_id, output_value_t *value);

#endif /* DRIVERS_OUTPUTS_INC_OUTPUTS_H_ */
