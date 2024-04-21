/*
 * cj125.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_CJ125_INC_CJ125_H_
#define DRIVERS_CJ125_INC_CJ125_H_

#include "gpio.h"
#include "main.h"
#include "errors.h"
#include "spi.h"

#include "cj125_reg.h"

#define CJ125_SPI_MODE                          (SPI_MODE_1)

#define CJ125_RELATION_ITEMS_MAX                (32)
#define CJ125_DIAG_POLL_PERIOD_US               (100 * TIME_US_IN_MS)
#define CJ125_PID_CB_DEFAULT_PERIOD_US          (5 * TIME_US_IN_MS)

#define CJ125_CALIBRATION_SAMPLES_TO_IGNORE     (3)
#define CJ125_CALIBRATION_MIN_SAMPLES           (10)
#define CJ125_CALIBRATION_MIN_PERIOD_US         (50 * TIME_US_IN_MS)
#define CJ125_CALIBRATION_TIMEOUT_US            (200 * TIME_US_IN_MS)

typedef enum {
  CJ125_AF_8 = 0,
  CJ125_AF_17,
  CJ125_AF_MAX
}cj125_af_t;

typedef enum {
  CJ125_DIAG_OK = 0,
  CJ125_DIAG_SHORTBATT,
  CJ125_DIAG_NOPOWER,
  CJ125_DIAG_SHORTGND,
}cj125_diag_enum_t;

typedef enum {
  CJ125_CONFIG_PRC_OFF = 0,
  CJ125_CONFIG_PRC_10UA = 1,
  CJ125_CONFIG_PRC_20UA = 2,
  CJ125_CONFIG_PRC_40UA = 4,
  CJ125_CONFIG_PRC_80UA = 8,
  CJ125_CONFIG_PRC_MAX = 16
}cj125_config_prc_t;

typedef enum {
  CJ125_PROCESS_RESET = 0,
  CJ125_PROCESS_CONFIGURE,
  CJ125_PROCESS_DIAG,
  CJ125_PROCESS_CALIBRATE,
  CJ125_PROCESS_MAX,
}cj125_process_fsm_t;

typedef enum {
  CJ125_RESET_CONDITION = 0,
  CJ125_RESET_IDENT,
  CJ125_RESET_CHECK,
  CJ125_RESET_REQUEST,
  CJ125_RESET_MAX,
}cj125_reset_fsm_t;

typedef enum {
  CJ125_DIAG_CONDITION = 0,
  CJ125_DIAG_MAX,
}cj125_diag_fsm_t;

typedef enum {
  CJ125_CONFIG_CONDITION = 0,
  CJ125_CONFIG_MAX,
}cj125_config_fsm_t;

typedef enum {
  CJ125_CALIB_CONDITION = 0,
  CJ125_CALIB_INIT_READ,
  CJ125_CALIB_INIT_WRITE,
  CJ125_CALIB_SAMPLE,
  CJ125_CALIB_INIT_RESTORE,
  CJ125_CALIB_CALCULATE,
  CJ125_CALIB_MAX,
}cj125_calib_fsm_t;

typedef struct cj125_ctx_tag cj125_ctx_t;
typedef void (*cj125_pid_cb_t)(cj125_ctx_t *ctx, time_us_t timestamp, void *usrdata);

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
    uint8_t items;
    float input[CJ125_RELATION_ITEMS_MAX];
    float output[CJ125_RELATION_ITEMS_MAX];
}cj125_config_relation_t;

typedef struct {
    cj125_config_relation_t res_to_temp_relation;
    cj125_config_relation_t curr_to_lambda_relation;
    bool temp_ref_resistance_override;
    float temp_ref_resistance;
    float temp_ref_res_max_deviation;
    float shunt_resistance;
    float pushpull_resistance;
    cj125_af_t ampfactor;
    cj125_config_prc_t pump_ref_current;
    time_delta_us_t pid_cb_period;
    cj125_pid_cb_t pid_cb;
    void *pid_cb_usrdata;
}cj125_config_t;

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

    float ref_voltage;
    float ur_voltage;
    float ua_voltage;
}cj125_data_t;

typedef union {
    uint16_t word;
    uint8_t bytes[2];
}cj125_payload_t;

typedef struct cj125_ctx_tag {
    cj125_init_ctx_t init;
    cj125_config_t config;
    cj125_regs_t regs;
    bool spi_busy;
    bool ready;
    bool initialized;
    bool configured;
    bool calibrated;

    bool reset_request;
    error_t reset_errcode;

    bool config_request;
    error_t config_errcode;

    bool calib_request;
    error_t calib_errcode;
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
    cj125_af_t ampfactor;

    cj125_data_t data;

    cj125_process_fsm_t process_fsm;
    cj125_reset_fsm_t reset_fsm;
    cj125_diag_fsm_t diag_fsm;
    cj125_config_fsm_t config_fsm;
    cj125_calib_fsm_t calib_fsm;

    cj125_payload_t request;
    cj125_payload_t response;

    uint16_t tx_payload;
    uint16_t rx_payload;

    time_us_t diag_timestamp;
    time_us_t pid_cb_timestamp;

    bool ua_updated;
    bool ur_updated;
    bool ref_ua_updated;
    bool ref_ur_updated;
    bool data_lambda_valid;
    bool data_temp_valid;

}cj125_ctx_t;

error_t cj125_init(cj125_ctx_t *ctx, const cj125_init_ctx_t *init_ctx);
void cj125_loop_main(cj125_ctx_t *ctx);
void cj125_loop_slow(cj125_ctx_t *ctx);
void cj125_loop_fast(cj125_ctx_t *ctx);

error_t cj125_reset(cj125_ctx_t *ctx);
error_t cj125_write_config(cj125_ctx_t *ctx, cj125_config_t *config);
error_t cj125_set_ampfactor(cj125_ctx_t *ctx, cj125_af_t ampfactor);
error_t cj125_calib_mode(cj125_ctx_t *ctx, bool enabled);


error_t cj125_update_ref(cj125_ctx_t *ctx, float ref_voltage);
error_t cj125_update_ur(cj125_ctx_t *ctx, float ur_voltage);
error_t cj125_update_ua(cj125_ctx_t *ctx, float ua_voltage);
error_t cj125_get_data(cj125_ctx_t *ctx, cj125_data_t *data);
error_t cj125_get_version(cj125_ctx_t *ctx, uint8_t *data);

#endif /* DRIVERS_CJ125_INC_CJ125_H_ */
