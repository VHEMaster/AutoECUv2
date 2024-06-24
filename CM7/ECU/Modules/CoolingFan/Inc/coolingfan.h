/*
 * coolingfan.h
 *
 *  Created on: Jun 21, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_COOLINGFAN_INC_COOLINGFAN_H_
#define MODULES_COOLINGFAN_INC_COOLINGFAN_H_

#include "common.h"
#include "versioned_coolingfan.h"
#include "config_gpio.h"
#include "config_sensors.h"
#include "time.h"

typedef union {
    uint32_t data;
    struct {
        bool output_drive_error : 1;
        bool input_control_error : 1;
        bool input_trigger_error : 1;
        bool control_on_error : 1;
        bool control_off_error : 1;
    }bits;
}coolingfan_diag_t;

typedef struct {
    bool enabled;
    bool working;

    bool activate_trigger;
    bool emergency_trigger;
    bool force_engaged;
    bool force_enabled;

    bool ignpower_on;
    bool ckp_detected;
    bool ckp_triggered;
}coolingfan_data_t;

typedef struct {
    uint32_t dummy;
}coolingfan_init_ctx_t;

typedef struct {
    coolingfan_init_ctx_t init;
    coolingfan_config_t config;
    bool ready;
    bool configured;

    bool output_pin_locked;
    bool control_pin_locked;
    bool force_pin_locked;

    bool working_prev;
    bool ckp_work_accept;
    bool control_working_status;
    bool control_working_valid;
    float control_voltage;

    ecu_gpio_input_type_t control_pin_mode;

    coolingfan_diag_t diag;
    coolingfan_data_t data;
    float ckp_rpm;

    output_id_t output_drive_pin;
    input_id_t input_control_pin;
    input_id_t input_force_pin;

    time_us_t ckp_last_time;
    time_us_t control_time;

}coolingfan_ctx_t;

error_t coolingfan_init(coolingfan_ctx_t *ctx, const coolingfan_init_ctx_t *init_ctx);
error_t coolingfan_configure(coolingfan_ctx_t *ctx, const coolingfan_config_t *config);
error_t coolingfan_reset(coolingfan_ctx_t *ctx);

void coolingfan_ckp_signal_update(coolingfan_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag);
void coolingfan_ignpower_update(coolingfan_ctx_t *ctx, bool ignpower_on);

error_t coolingfan_get_data(coolingfan_ctx_t *ctx, coolingfan_data_t *data);
error_t coolingfan_get_diag(coolingfan_ctx_t *ctx, coolingfan_diag_t *diag);

error_t coolingfan_activate_trigger(coolingfan_ctx_t *ctx, bool activate_trigger);
error_t coolingfan_emergency_trigger(coolingfan_ctx_t *ctx, bool emergency_trigger);

error_t coolingfan_force_reset(coolingfan_ctx_t *ctx);
error_t coolingfan_force_set(coolingfan_ctx_t *ctx, bool force_enabled);

void coolingfan_loop_main(coolingfan_ctx_t *ctx);
void coolingfan_loop_slow(coolingfan_ctx_t *ctx);
void coolingfan_loop_fast(coolingfan_ctx_t *ctx);


#endif /* MODULES_COOLINGFAN_INC_COOLINGFAN_H_ */
