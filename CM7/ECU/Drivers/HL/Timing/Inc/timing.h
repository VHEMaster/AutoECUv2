/*
 * timing.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_HL_TIMING_INC_TIMING_H_
#define DRIVERS_HL_TIMING_INC_TIMING_H_

#include "versioned_timing.h"
#include "config_tps.h"
#include "config_motor.h"
#include "errors.h"
#include "bool.h"
#include "time.h"
#include <stdint.h>

#define TIMING_TPS_TIMEOUT_US                          (200 * TIME_US_IN_MS)
#define TIMING_TPS_DEFAULT_POSITION_SAMPLES            (8)
#define TIMING_TPS_DEFAULT_POSITION_SAMPLE_PERIOD_US   (5 * TIME_US_IN_MS)

typedef enum {
  TIMING_FSM_CONDITION = 0,
  TIMING_FSM_DISABLE,
  TIMING_FSM_HWSC,
  TIMING_FSM_DIAGOFF,
  TIMING_FSM_RECONFIGURE,
  TIMING_FSM_TPS_WAIT,
  TIMING_FSM_DEFAULT_SAMPLING,
  TIMING_FSM_MAX
}timing_fsm_t;

typedef union {
    uint32_t data;
    struct {
        bool tps_error : 1;
        bool motor_error : 1;
        bool position_reach_failure : 1;
        bool pwr_undervoltage;
        bool pwr_overvoltage;
        bool hwsc_failed;
        bool diagoff_failed;
    }bits;
}timing_diag_t;

typedef struct {
    bool enabled;
    bool active;
    float target_position;
    float current_position;
    float default_position;
    float dutycycle;
    float output_voltage;
}timing_data_t;

typedef struct {
    uint32_t dummy;
}timing_init_ctx_t;

typedef struct {
    timing_init_ctx_t init;
    timing_config_t config;
    bool ready;
    bool configured;

    timing_fsm_t fsm_process;

    timing_diag_t diag;
    timing_data_t data;
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
    float current_speed;
    float target_speed;

    time_us_t reset_time;
    time_us_t process_time;
    time_us_t pos_reach_time;

}timing_ctx_t;

error_t timing_init(timing_ctx_t *ctx, const timing_init_ctx_t *init_ctx);
error_t timing_configure(timing_ctx_t *ctx, const timing_config_t *config);
error_t timing_reset(timing_ctx_t *ctx);

error_t timing_get_data(timing_ctx_t *ctx, timing_data_t *data);
error_t timing_get_diag(timing_ctx_t *ctx, timing_diag_t *diag);

void timing_loop_main(timing_ctx_t *ctx);
void timing_loop_slow(timing_ctx_t *ctx);
void timing_loop_fast(timing_ctx_t *ctx);


#endif /* DRIVERS_HL_TIMING_INC_TIMING_H_ */
