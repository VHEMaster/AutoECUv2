/*
 * fuelpump.h
 *
 *  Created on: Jun 21, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_FUELPUMP_INC_FUELPUMP_H_
#define MODULES_FUELPUMP_INC_FUELPUMP_H_

#include "common.h"
#include "versioned_fuelpump.h"
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
}fuelpump_diag_t;

typedef struct {
    bool enabled;
    bool working;

    bool manual_enabled;
    bool force_engaged;
    bool force_enabled;

    bool ignition_on;
    bool trigger_on;
    bool ckp_triggered;
}fuelpump_data_t;

typedef struct {
    uint32_t dummy;
}fuelpump_init_ctx_t;

typedef enum {
  FUELPUMP_FSM_INIT = 0,
  FUELPUMP_FSM_STARTUP,
  FUELPUMP_FSM_PROCESS,
}fuelpump_fsm_t;

typedef struct {
    fuelpump_init_ctx_t init;
    fuelpump_config_t config;
    bool ready;
    bool configured;

    bool output_pin_locked;
    bool control_pin_locked;
    bool trigger_pin_locked;

    bool working_prev;
    bool control_working_status;
    bool control_working_valid;
    float control_voltage;

    ecu_gpio_input_type_t control_pin_mode;

    fuelpump_diag_t diag;
    fuelpump_data_t data;

    output_id_t output_drive_pin;
    input_id_t input_control_pin;
    input_id_t input_trigger_pin;

    fuelpump_fsm_t fsm_state;
    time_us_t fsm_state_time;
    time_us_t ckp_last_time;
    time_us_t control_time;

}fuelpump_ctx_t;

error_t fuelpump_init(fuelpump_ctx_t *ctx, const fuelpump_init_ctx_t *init_ctx);
error_t fuelpump_configure(fuelpump_ctx_t *ctx, const fuelpump_config_t *config);
error_t fuelpump_reset(fuelpump_ctx_t *ctx);

void fuelpump_ckp_signal_update(fuelpump_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag);
void fuelpump_ignition_update(fuelpump_ctx_t *ctx, bool ignition_on);

error_t fuelpump_get_data(fuelpump_ctx_t *ctx, fuelpump_data_t *data);
error_t fuelpump_get_diag(fuelpump_ctx_t *ctx, fuelpump_diag_t *diag);

error_t fuelpump_manual_set(fuelpump_ctx_t *ctx, bool manual_enabled);

error_t fuelpump_force_reset(fuelpump_ctx_t *ctx);
error_t fuelpump_force_set(fuelpump_ctx_t *ctx, bool force_enabled);

void fuelpump_loop_main(fuelpump_ctx_t *ctx);
void fuelpump_loop_slow(fuelpump_ctx_t *ctx);
void fuelpump_loop_fast(fuelpump_ctx_t *ctx);


#endif /* MODULES_FUELPUMP_INC_FUELPUMP_H_ */
