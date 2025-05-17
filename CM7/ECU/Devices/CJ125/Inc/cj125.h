/*
 * cj125.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_CJ125_INC_CJ125_H_
#define DRIVERS_CJ125_INC_CJ125_H_

#include "common.h"
#include "gpio.h"
#include "spi.h"
#include "pid.h"

#include "cj125_reg.h"
#include "versioned_wbls.h"

#define CJ125_SPI_MODE                          (SPI_MODE_1)

#define CJ125_DIAG_POLL_PERIOD_US               (20 * TIME_US_IN_MS)
#define CJ125_PID_CB_DEFAULT_PERIOD_US          (5 * TIME_US_IN_MS)

#define CJ125_CALIBRATION_SAMPLES_TO_IGNORE     (10)
#define CJ125_CALIBRATION_MIN_SAMPLES           (20)
#define CJ125_CALIBRATION_MIN_PERIOD_US         (50 * TIME_US_IN_MS)
#define CJ125_CALIBRATION_TIMEOUT_US            (200 * TIME_US_IN_MS)
#define CJ125_VOLTAGES_TIMEOUT_US               (500 * TIME_US_IN_MS)

#define CJ125_HEATER_OPERATING_UR_LIMIT_H       (3.0f)
#define CJ125_HEATER_MINIMUM_POWER_VOLTAGE      (5.0f)
#define CJ125_HEATER_FREQ                       (200)

#define CJ125_CALIBRATION_UR_THRESHOLD_L        (0.7f)
#define CJ125_CALIBRATION_UR_THRESHOLD_H        (1.2f)
#define CJ125_CALIBRATION_UA_THRESHOLD_L        (1.3f)
#define CJ125_CALIBRATION_UA_THRESHOLD_H        (1.7f)
#define CJ125_CALIBRATION_REF_THRESHOLD_L       (4.7f)
#define CJ125_CALIBRATION_REF_THRESHOLD_H       (5.3f)
#define CJ125_CALIBRATION_RADJ_DEFAULT          (0.3f)

typedef enum {
  CJ125_DIAG_OK = 0,
  CJ125_DIAG_SHORTBATT = 1,
  CJ125_DIAG_NOPOWER = 2,
  CJ125_DIAG_OPENLOAD = 2,
  CJ125_DIAG_SHORTGND = 3,
}cj125_diag_enum_t;

typedef enum {
  CJ125_HEATUP_TYPE_OFF = 0,
  CJ125_HEATUP_TYPE_PREHEAT,
  CJ125_HEATUP_TYPE_OPERATING,
  CJ125_HEATUP_TYPE_MAX
}cj125_heatup_type_t;

typedef enum {
  CJ125_OPERATING_STATUS_IDLE = 0,
  CJ125_OPERATING_STATUS_PREHEAT,
  CJ125_OPERATING_STATUS_HEATUP,
  CJ125_OPERATING_STATUS_OPERATING,
  CJ125_OPERATING_STATUS_ERROR,
}cj125_operating_status_t;

typedef enum {
  CJ125_PROCESS_RESET = 0,
  CJ125_PROCESS_CONFIGURE,
  CJ125_PROCESS_DIAG,
  CJ125_PROCESS_CALIBRATE,
  CJ125_PROCESS_MAX,
}cj125_process_fsm_t;

typedef enum {
  CJ125_HEATER_RESET = 0,
  CJ125_HEATER_PREHEAT,
  CJ125_HEATER_HEATUP,
  CJ125_HEATER_HEATUP_WAITING,
  CJ125_HEATER_OPERATING,
  CJ125_HEATER_ERROR,
  CJ125_HEATER_MAX,
}cj125_heater_fsm_t;

typedef enum {
  CJ125_RESET_CONDITION = 0,
  CJ125_RESET_IDENT,
  CJ125_RESET_CHECK,
  CJ125_RESET_REQUEST,
  CJ125_RESET_CALIB_INIT_READ,
  CJ125_RESET_CALIB_INIT_WRITE,
  CJ125_RESET_CALIB_SAMPLE,
  CJ125_RESET_CALIB_INIT_RESTORE,
  CJ125_RESET_CALIB_CALCULATE,
  CJ125_RESET_MAX,
}cj125_reset_fsm_t;

typedef enum {
  CJ125_DIAG_CONDITION = 0,
  CJ125_DIAG_REQUEST,
  CJ125_DIAG_MAX,
}cj125_diag_fsm_t;

typedef enum {
  CJ125_CONFIG_CONDITION = 0,
  CJ125_CONFIG_REQUEST_INIT1,
  CJ125_CONFIG_REQUEST_INIT2,
  CJ125_CONFIG_MAX,
}cj125_config_fsm_t;

typedef struct cj125_ctx_tag cj125_ctx_t;
typedef void (*cj125_cb_t)(cj125_ctx_t *ctx, void *usrdata);

typedef union {
    uint8_t byte;
    struct {
        cj125_diag_enum_t vm : 2;
        cj125_diag_enum_t un : 2;
        cj125_diag_enum_t iaip : 2;
        cj125_diag_enum_t diahgd : 2;
    }bits;
}cj125_diag_t;

typedef struct {
    spi_slave_t *spi_slave;
    gpio_t nrst_pin;
}cj125_init_ctx_t;

typedef struct {
    float pwr;
    float ref;
    float ua;
    float ur;
}cj125_voltages_t;

typedef struct {
    TIM_HandleTypeDef *heater_pwm;
    uint32_t heater_pwm_ch;
    gpio_t heater_en_pin;
    gpio_t heater_nen_pin;

    uint32_t tim_base_freq;
    uint32_t tim_prescaler;
    uint32_t tim_period;
    __IO uint32_t *tim_pulse;
}cj125_heater_t;

typedef struct {
    float lambda_value;
    float temp_value;

    float heat_ref_voltage;
    float heat_ref_temp;
    float heat_ref_resistance;
    float heat_resistance;

    cj125_af_t ampfactor;
    float lambda_ref_voltage;
    float lambda_radj;
    float lambda_current;
    float lambda_current_tc;

    float heater_voltage;
    float heater_dutycycle;

    float pwr_voltage;
    float ref_voltage;
    float ur_voltage;
    float ua_voltage;

    cj125_operating_status_t operating_status;

}cj125_data_t;

typedef union {
    uint16_t word;
    uint8_t bytes[2];
}cj125_payload_t;

typedef struct cj125_ctx_tag {
    cj125_init_ctx_t init;
    cj125_config_t config;
    cj125_heater_t heater;
    cj125_regs_t regs;
    bool spi_busy;
    bool ready;
    bool heater_ready;
    bool initialized;
    bool configured;

    bool reset_request;
    error_t reset_errcode;

    bool config_request;
    error_t config_errcode;

    bool calib_ok;

    time_us_t calib_timestamp;
    uint16_t calib_ignored;
    uint16_t calib_samples;
    float calib_ur_voltage;
    float calib_ua_voltage;
    float calib_ref_voltage;
    uint8_t calib_init1_byte;

    bool ampfactor_request;
    error_t ampfactor_errcode;
    cj125_af_t ampfactor_req_data;
    cj125_heatup_type_t heatup_type;

    cj125_data_t data;
    cj125_diag_t diag;
    math_pid_ctx_t heater_pid;

    cj125_heater_fsm_t heater_fsm;
    cj125_process_fsm_t process_fsm;
    cj125_reset_fsm_t reset_fsm;
    cj125_diag_fsm_t diag_fsm;
    cj125_config_fsm_t config_fsm;

    cj125_payload_t request;
    cj125_payload_t response;

    uint16_t tx_payload;
    uint16_t rx_payload;

    time_us_t diag_timestamp;
    time_us_t pid_cb_timestamp;
    time_us_t voltages_timestamp;
    time_us_t heater_fsm_last;

    bool voltages_updated;
    bool data_lambda_valid;
    bool data_temp_valid;
    bool diag_valid;

}cj125_ctx_t;

error_t cj125_init(cj125_ctx_t *ctx, const cj125_init_ctx_t *init_ctx);
error_t cj125_heater_init(cj125_ctx_t *ctx, const cj125_heater_t *heater_cfg);
void cj125_loop_main(cj125_ctx_t *ctx);
void cj125_loop_slow(cj125_ctx_t *ctx);
void cj125_loop_fast(cj125_ctx_t *ctx);

error_t cj125_reset(cj125_ctx_t *ctx);
error_t cj125_configure(cj125_ctx_t *ctx, const cj125_config_t *config);
error_t cj125_set_ampfactor(cj125_ctx_t *ctx, cj125_af_t ampfactor);

error_t cj125_update_voltages(cj125_ctx_t *ctx, const cj125_voltages_t *voltages);
error_t cj125_get_data(cj125_ctx_t *ctx, cj125_data_t *data);
error_t cj125_get_version(cj125_ctx_t *ctx, uint8_t *data);
error_t cj125_get_diag(cj125_ctx_t *ctx, cj125_diag_t *diag);
error_t cj125_set_heatup(cj125_ctx_t *ctx, cj125_heatup_type_t type);

#endif /* DRIVERS_CJ125_INC_CJ125_H_ */
