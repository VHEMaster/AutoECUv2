/*
 * l9960.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_L9960_INC_L9960_H_
#define DRIVERS_L9960_INC_L9960_H_

#include "common.h"
#include "gpio.h"
#include "time.h"
#include "spi.h"

#include "l9960_reg.h"
#include "versioned_motor.h"

#define L9966_ELECTRONIC_ID           (0x0E4)
#define L9960_SPI_MODE                (SPI_MODE_1)
#define L9960_STATUS_POLL_PERIOD_UD   (10 * TIME_US_IN_MS)
#define L9966_RESET_WAIT_US           (1 * TIME_US_IN_MS)

#define L9966_HWSC_WAIT_US            (5 * TIME_US_IN_MS)
#define L9966_HWSC_TIMEOUT            (200 * TIME_US_IN_MS)

#define L9966_DIAGOFF_WAIT_US         (5 * TIME_US_IN_MS)
#define L9966_DIAGOFF_TIMEOUT         (500 * TIME_US_IN_MS)

typedef enum {
  L9960_STATUS_OC_OK = 0,
  L9960_STATUS_OC_SHORT = 1,
  L9960_STATUS_OC_OVERCURRENT = 2,
}l9960_status_oc_t;

typedef enum {
  L9960_STATUS_OL_OK = 0,
  L9960_STATUS_OL_OPENLOAD = 1,
  L9960_STATUS_OL_NOTDONE = 2,
  L9960_STATUS_OL_DISABLED = 3,
}l9960_status_openload_t;

typedef enum {
  L9960_STATUS_CC_OK = 0,
  L9960_STATUS_CC_FAIL
}l9960_status_cc_t;

typedef enum {
  L9960_STATUS_OV_OK = 0,
  L9960_STATUS_OV_FAIL
}l9960_status_ov_t;

typedef enum {
  L9960_STATUS_UV_OK = 0,
  L9960_STATUS_UV_FAIL
}l9960_status_uv_t;

typedef enum {
  L9960_STATUS_OT_OK = 0,
  L9960_STATUS_OT_FAIL
}l9960_status_ot_t;

typedef enum {
  L9960_STATUS_DIAGOFF_OK = 0,
  L9960_STATUS_DIAGOFF_OPENLOAD,
  L9960_STATUS_DIAGOFF_SHORTBATT,
  L9960_STATUS_DIAGOFF_SHORTGND,
  L9960_STATUS_DIAGOFF_TIMEOUT,
}l9960_status_diagoff_t;

typedef enum {
  L9960_STATUS_HWSC_OK = 0,
  L9960_STATUS_HWSC_FAIL,
  L9960_STATUS_HWSC_FAIL_LBIST_PASS,
  L9960_STATUS_HWSC_TIMEOUT,
}l9960_status_hwsc_t;

typedef struct {
    uint16_t electronic_id;
    uint8_t silicon_version;
    uint8_t logic_hw_version;
    uint8_t supplier;
    uint16_t wafer_x;
    uint16_t wafer_y;
    uint16_t wafer_number;
}l9960_version_t;

typedef struct {
    l9960_resp_oc_t overcurrent;
    l9960_resp_config5_t config5;
    l9960_resp_states1_t states1;
    l9960_resp_states2_t states2;
    l9960_resp_states3_t states3;
}l9960_status_t;

typedef union {
    uint32_t data;
    struct {
        l9960_status_oc_t ocl0 : 2;
        l9960_status_oc_t ocl1 : 2;
        l9960_status_oc_t och0 : 2;
        l9960_status_oc_t och1 : 2;
        l9960_status_openload_t ol : 2;
        l9960_status_cc_t cc : 1;
        l9960_status_ov_t ov_5v: 1;
        l9960_status_uv_t uv_5v : 1;
        l9960_status_uv_t uv_12v : 1;
        l9960_status_ot_t ot_warn : 1;
        l9960_status_ot_t ot_shutdown : 1;
        l9960_status_diagoff_t diagoff : 3;
        l9960_status_hwsc_t hwsc : 3;


    }bits;
}l9960_diag_t;

typedef enum {
  L9960_RESET_CONDITION = 0,
  L9960_RESET_VERSION_REQUEST1,
  L9960_RESET_VERSION_REQUEST2,
  L9960_RESET_VERSION_REQUEST3,
  L9960_RESET_VERSION_REQUEST4,
  L9960_RESET_VERSION_REQUEST5,
  L9960_RESET_TRIGGER,
  L9960_RESET_VERSION_CHECK,
  L9960_RESET_VERSION_VERIFY,
  L9960_RESET_MAX
}l9960_reset_fsm_t;

typedef enum {
  L9960_DIAGOFF_CONDITION = 0,
  L9960_DIAGOFF_INITIAL_WAIT,
  L9960_DIAGOFF_INITIAL,
  L9960_DIAGOFF_WAIT,
  L9960_DIAGOFF_REQUEST,
  L9960_DIAGOFF_DISABLE,
  L9960_DIAGOFF_DISABLE_REQUEST,
  L9960_DIAGOFF_MAX
}l9960_diagoff_fsm_t;

typedef enum {
  L9960_HWSC_CONDITION = 0,
  L9960_HWSC_INITIAL,
  L9960_HWSC_STATUS,
  L9960_HWSC_DISABLE,
  L9960_HWSC_DISABLE_REQUEST,
  L9960_HWSC_MAX
}l9960_hwsc_fsm_t;

typedef enum {
  L9960_STATUS_CONDITION = 0,
  L9960_STATUS_REQUEST_OC,
  L9960_STATUS_REQUEST1,
  L9960_STATUS_REQUEST2,
  L9960_STATUS_REQUEST3,
  L9960_STATUS_REQUEST4,
  L9960_STATUS_REQUEST5,
  L9960_STATUS_MAX
}l9960_status_fsm_t;

typedef enum {
  L9960_PROCESS_RESET = 0,
  L9960_PROCESS_CHECK_STATUS,
  L9960_PROCESS_CONFIGURE,
  L9960_PROCESS_DIAGOFF,
  L9960_PROCESS_HWSC,
  L9960_PROCESS_MAX
}l9960_process_fsm_t;

typedef enum {
  L9960_CONFIG_CONDITION = 0,
  L9960_CONFIG_TRANSLATE,
  L9960_CONFIG_REQUEST1,
  L9960_CONFIG_REQUEST2,
  L9960_CONFIG_REQUEST3,
  L9960_CONFIG_REQUEST4,
  L9960_CONFIG_MAX
}l9960_config_fsm_t;

typedef struct {
    spi_slave_t *spi_slave;
    gpio_t dis_pin;
    gpio_t ndis_pin;
}l9960_init_ctx_t;

typedef struct {
    l9960_init_ctx_t init;
    bool ready;
    bool spi_busy;
    bool initialized;
    bool configured;

    bool reset_request;
    error_t reset_errcode;

    bool hwsc_request;
    error_t hwsc_errcode;
    l9960_reg_hwsc_t hwsc_status;

    bool diagoff_request;
    error_t diagoff_errcode;
    l9960_reg_diagoff_t diagoff_status;

    bool config_request;
    error_t config_errcode;

    bool version_valid;
    bool status_valid;
    bool diag_valid;
    l9960_version_t version;
    l9960_status_t status;
    l9960_diag_t diag;

    l9960_config_t config;
    l9960_reg_cfg_t reg_cfg;

    l9960_process_fsm_t process_fsm;
    l9960_reset_fsm_t reset_fsm_state;
    l9960_status_fsm_t status_fsm_state;
    l9960_diagoff_fsm_t diagoff_fsm_state;
    l9960_hwsc_fsm_t hwsc_fsm_state;
    l9960_config_fsm_t config_fsm_state;


    l9960_request_payload_t request;
    l9960_response_payload_t response;

    uint16_t tx_payload;
    uint16_t rx_payload;
    time_us_t reset_timestamp;
    time_us_t status_timestamp;
    time_us_t diagoff_timestamp;

    time_us_t hwsc_started;
    time_us_t diagoff_started;

    bool status_hwsc_update;

}l9960_ctx_t;

error_t l9960_init(l9960_ctx_t *ctx, const l9960_init_ctx_t *init_ctx);
void l9960_loop_main(l9960_ctx_t *ctx);
void l9960_loop_slow(l9960_ctx_t *ctx);
void l9960_loop_fast(l9960_ctx_t *ctx);

error_t l9960_configure(l9960_ctx_t *ctx, const l9960_config_t *config);

error_t l9960_reset(l9960_ctx_t *ctx);
error_t l9960_hwsc(l9960_ctx_t *ctx);
error_t l9960_diagoff(l9960_ctx_t *ctx);

error_t l9960_set_enabled(l9960_ctx_t *ctx, bool enabled);

error_t l9960_get_version(l9960_ctx_t *ctx, l9960_version_t *ver);
error_t l9960_get_status(l9960_ctx_t *ctx, l9960_status_t *status);
error_t l9960_get_diagnostic(l9960_ctx_t *ctx, l9960_diag_t *diag);



#endif /* DRIVERS_L9960_INC_L9960_H_ */
