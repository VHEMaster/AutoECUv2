/*
 * etc.h
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_HL_ETC_INC_ETC_H_
#define DRIVERS_HL_ETC_INC_ETC_H_

#include "versioned_etc.h"
#include "config_tps.h"
#include "config_motor.h"
#include "errors.h"
#include "bool.h"
#include "time.h"
#include <stdint.h>

#define ETC_TPS_TIMEOUT_US                          (200 * TIME_US_IN_MS)
#define ETC_TPS_DEFAULT_POSITION_SAMPLES            (8)
#define ETC_TPS_DEFAULT_POSITION_SAMPLE_PERIOD_US   (5 * TIME_US_IN_MS)

typedef enum {
  ETC_FSM_CONDITION = 0,
  ETC_FSM_DISABLE,
  ETC_FSM_HWSC,
  ETC_FSM_DIAGOFF,
  ETC_FSM_RECONFIGURE,
  ETC_FSM_TPS_WAIT,
  ETC_FSM_DEFAULT_SAMPLING,
  ETC_FSM_MAX
}etc_fsm_t;

typedef union {
    uint32_t data;
    struct {
        bool tps_error : 1;
        bool motor_error : 1;
        bool position_reach_failure : 1;
        bool pwr_undervoltage;
        bool pwr_overvoltage;
    }bits;
}etc_diag_t;

typedef struct {
    bool enabled;
    bool active;
    float target_position;
    float current_position;
    float default_position;
    float dutycycle;
}etc_data_t;

typedef struct {
    uint32_t dummy;
}etc_init_ctx_t;

typedef struct {
    etc_init_ctx_t init;
    etc_config_t config;
    bool ready;
    bool configured;

    etc_fsm_t fsm_process;

    etc_diag_t diag;
    etc_data_t data;
    tps_data_t tps_data;

    tps_diag_t tps_diag;
    l9960_diag_t motor_diag;

    float default_position_sampling;
    uint8_t default_position_samples;

    math_pid_ctx_t pid_position;
    math_pid_ctx_t pid_speed;

    bool reset_request;
    error_t reset_errcode;
    input_id_t power_voltage_pin;
    float position_current;
    float position_prev;
    float power_voltage;
    float output_voltage;
    float current_speed;
    float target_speed;

    time_us_t reset_time;
    time_us_t process_time;
    time_us_t pos_reach_time;

}etc_ctx_t;

error_t etc_init(etc_ctx_t *ctx, const etc_init_ctx_t *init_ctx);
error_t etc_configure(etc_ctx_t *ctx, const etc_config_t *config);
error_t etc_reset(etc_ctx_t *ctx);

error_t etc_get_data(etc_ctx_t *ctx, etc_data_t *data);
error_t etc_get_diag(etc_ctx_t *ctx, etc_diag_t *diag);

void etc_loop_main(etc_ctx_t *ctx);
void etc_loop_slow(etc_ctx_t *ctx);
void etc_loop_fast(etc_ctx_t *ctx);


#endif /* DRIVERS_HL_ETC_INC_ETC_H_ */
