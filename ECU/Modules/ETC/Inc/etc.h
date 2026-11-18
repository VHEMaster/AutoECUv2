/*
 * etc.h
 *
 *  Created on: May 16, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_ETC_INC_ETC_H_
#define MODULES_ETC_INC_ETC_H_

#include "common.h"
#include "versioned_etc.h"
#include "config_tps.h"
#include "config_motor.h"
#include "time.h"

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
        bool tps_handle_error : 1;
        bool tps_data_validity_error : 1;
        bool tps_pos_validity_error : 1;
        bool tps_diag_error : 1;
        bool motor_handle_error : 1;
        bool motor_pwm_error : 1;
        bool motor_error : 1;
        bool position_reach_failure : 1;
        bool hwsc_failed : 1;
        bool diagoff_failed : 1;
    }bits;
}etc_diag_t;

typedef struct {
    bool enabled;
    bool active;
    float target_position;
    float current_position;
    float default_position;
    float dutycycle;
    float output_voltage;
}etc_data_t;

typedef struct {
    uint32_t dummy;
}etc_init_ctx_t;

typedef enum {
  ETC_ADAPTATION_FSM_PARK = 0,
  ETC_ADAPTATION_FSM_SEEK,
  ETC_ADAPTATION_FSM_PROBE,
  ETC_ADAPTATION_FSM_STOP = 4,
  ETC_ADAPTATION_FSM_FAST_SEEK,
  ETC_ADAPTATION_FSM_FAST_STEP
}etc_adaptation_fsm_t;

typedef enum {
  ETC_ADAPTATION_FAILURE_NONE = 0,
  ETC_ADAPTATION_FAILURE_REST_RANGE,
  ETC_ADAPTATION_FAILURE_POSITION_RANGE,
  ETC_ADAPTATION_FAILURE_SUPPLY_RANGE,
  ETC_ADAPTATION_FAILURE_PROBE_DIRECTION,
  ETC_ADAPTATION_FAILURE_PROBE_LIMIT,
  ETC_ADAPTATION_FAILURE_PROBE_THRESHOLDS,
  ETC_ADAPTATION_FAILURE_MODEL,
  ETC_ADAPTATION_FAILURE_PID_EFFORT,
  ETC_ADAPTATION_FAILURE_PID_NO_CANDIDATE,
  ETC_ADAPTATION_FAILURE_STATE_TIMEOUT,
  ETC_ADAPTATION_FAILURE_CANCELLED,
  ETC_ADAPTATION_FAILURE_SETUP,
  ETC_ADAPTATION_FAILURE_CKP,
  ETC_ADAPTATION_FAILURE_ENGINE_MOVING,
  ETC_ADAPTATION_FAILURE_SAMPLE,
  ETC_ADAPTATION_FAILURE_MOTOR_COMMAND,
  ETC_ADAPTATION_FAILURE_STATE,
  ETC_ADAPTATION_FAILURE_STOP_OUTPUT,
  ETC_ADAPTATION_FAILURE_PID_FAST_NO_CANDIDATE,
  ETC_ADAPTATION_FAILURE_PID_FAST_SPEED,
  ETC_ADAPTATION_FAILURE_PID_FAST_SETTLE,
  ETC_ADAPTATION_FAILURE_PID_FAST_OSCILLATION,
  ETC_ADAPTATION_FAILURE_PID_FAST_DWELL
}etc_adaptation_failure_t;

typedef struct {
    etc_adaptation_failure_t reason;
    etc_adaptation_fsm_t fsm;
    error_t error;
    error_t stop_error;
    time_delta_us_t elapsed;
    float position;
    float position_raw;
    float speed;
    float power_voltage;
    float dutycycle;
    float effort_limit;
    uint8_t point;
    uint8_t trial;
    uint8_t step;
    int8_t direction;
    bool fast_pid;
    uint8_t oscillation_rejections;
}etc_adaptation_failure_data_t;

#define ETC_ADAPTATION_PID_FINALISTS (4u)

typedef struct {
    float fast_score;
    float min_peak_speed;
    float peak_dutycycle;
    time_delta_us_t max_near_error_time;
    etc_adaptation_failure_t fast_rejection;
    uint8_t trial;
    uint8_t fast_steps_started;
    uint8_t fast_steps_passed;
}etc_adaptation_pid_finalist_t;

typedef struct {
    float band;
    float rebound;
    float settled_undershoot;
    float peak_speed;
    float previous_speed;
    bool reached_target;
    bool command_sent;
    bool near_target_seen;
    float position_extremum;
    float previous_extremum;
    float previous_excursion;
    time_delta_us_t settle_since;
    time_delta_us_t near_error_since;
    time_delta_us_t max_near_error_time;
    int8_t near_error_side;
    int8_t motion_direction;
    uint8_t nondecaying_peaks;
}etc_adaptation_fast_step_t;

typedef struct {
    etc_adaptation_setup_t setup;
    etc_adaptation_config_t model;
    etc_adaptation_config_t trial;
    etc_adaptation_config_t result;
    etc_adaptation_failure_data_t failure;
    etc_adaptation_pid_finalist_t finalists[ETC_ADAPTATION_PID_FINALISTS];
    etc_adaptation_fast_step_t fast_step;

    etc_adaptation_fsm_t fsm;
    error_t status;
    error_t stop_status;
    volatile error_t apply_status; /* Write E_AGAIN to apply the result when ready on the slow loop. */
    bool started;
    bool initialized;
    bool cancel;
    bool motion_detected;
    bool probe_limit_reached;
    bool park_sample_valid;
    bool seek_holding;
    bool seek_reversal_ready;
    bool fast_pid;

    time_us_t state_time;
    time_us_t stable_time;
    time_us_t sample_time;
    time_us_t probe_limit_time;
    float park_position_min;
    float park_position_max;
    float start_position;
    float seek_integral;
    float seek_effort;
    float seek_speed;
    float seek_hold_position;
    float seek_reversal_position;
    float start_effort;
    float opening_effort;
    float motion_effort;
    float effort;
    float error_integral;
    float residual_raw_prev;
    float overshoot;
    float trial_score;
    float worst_step_score;
    float best_score;
    uint8_t point;
    uint8_t trial_index;
    uint8_t finalist_count;
    uint8_t finalist_index;
    uint8_t fast_route_offset;
    uint8_t selected_trial;
    uint8_t oscillation_rejections;
    uint8_t step;
    int8_t direction;
}etc_adaptation_ctx_t;

typedef struct {
    float samples[ETC_ADAPTATION_TPS_AVERAGE_MAX];
    float sum;
    uint8_t window;
    uint8_t count;
    uint8_t index;
}etc_tps_average_ctx_t;

typedef enum {
  ETC_MOTION_IDLE = 0,
  ETC_MOTION_ACCELERATE,
  ETC_MOTION_BRAKE,
  ETC_MOTION_SETTLE
}etc_motion_state_t;

typedef struct {
    float position;
    float start_position;
    float effort;
    float threshold;
    time_us_t time;
    int8_t direction;
    int8_t last_direction;
    int8_t release_direction;
}etc_reversal_ctx_t;

typedef struct {
    float target;
    float effort;
    float previous_position;
    float position_sum;
    uint16_t samples;
    time_us_t window_time;
    bool previous_valid;
}etc_settle_ctx_t;

typedef struct {
    etc_init_ctx_t init;
    etc_config_t config;
    bool ready;
    bool configured;
    bool runtime_model_active;
    bool runtime_base_pid;

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

    etc_adaptation_ctx_t adaptation;
    etc_tps_average_ctx_t tps_average;

    bool reset_request;
    error_t reset_errcode;
    input_id_t power_voltage_pin;
    float position_current;
    float position_prev;
    float power_voltage;
    float current_speed;
    float model_speed;
    float target_speed;
    float motion_target;
    float motion_direction;
    etc_motion_state_t motion_state;
    etc_reversal_ctx_t reversal;
    etc_settle_ctx_t settle;
    float pos_reach_position;
    float model_handover_effort;
    bool model_handover_active;

    time_us_t reset_time;
    time_us_t process_time;
    time_us_t pos_reach_time;
    time_us_t model_handover_time;

}etc_ctx_t;

error_t etc_init(etc_ctx_t *ctx, const etc_init_ctx_t *init_ctx);
error_t etc_configure(etc_ctx_t *ctx, const etc_config_t *config);
error_t etc_reset(etc_ctx_t *ctx);

/* Run in CM7 service context with cranking inhibited. Start uses config.adaptation_setup.
 * Position limits are TPS percent and dutycycle_max is absolute PWM duty (0..1).
 * The slow loop runs the adaptation. Process reports its current or final status.
 * After E_OK, copy the result into a staging config and use the normal configuration path.
 */
error_t etc_adaptation_start(etc_ctx_t *ctx);
error_t etc_adaptation_process(etc_ctx_t *ctx);
error_t etc_adaptation_result(etc_ctx_t *ctx, etc_adaptation_config_t *result);
error_t etc_adaptation_cancel(etc_ctx_t *ctx);

error_t etc_get_data(etc_ctx_t *ctx, etc_data_t *data);
error_t etc_get_diag(etc_ctx_t *ctx, etc_diag_t *diag);

void etc_loop_main(etc_ctx_t *ctx);
void etc_loop_slow(etc_ctx_t *ctx);
void etc_loop_fast(etc_ctx_t *ctx);


#endif /* MODULES_ETC_INC_ETC_H_ */
