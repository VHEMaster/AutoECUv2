/*
 * l9966.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_L9966_INC_L9966_H_
#define DRIVERS_L9966_INC_L9966_H_

#include "main.h"
#include "errors.h"
#include "gpio.h"
#include "time.h"
#include "bool.h"
#include "spi.h"

#include "l9966_reg.h"
#include "l9966_ctrl.h"
#include "versioned_flexio.h"

#define L9966_DEFAULT_DIGITAL_POLL_PERIOD     (10 * TIME_US_IN_MS)
#define L9966_SQNCR_RESULT_POLL_PERIOD        (50 * TIME_US_IN_MS)
#define L9966_STATUS_POLL_PERIOD              (100 * TIME_US_IN_MS)
#define L9966_SPI_MODE                        (SPI_MODE_1)

#define L9966_RESET_HARD_WAIT_SET_US          (1 * TIME_US_IN_MS)
#define L9966_RESET_SOFT_WAIT_US              (1 * TIME_US_IN_MS)
#define L9966_SYNC_WAIT_US                    (10)

typedef struct {
    spi_slave_t *spi_slave;
    gpio_t wake_pin;
    gpio_t sync_pin;
    gpio_t nrst_pin;
    gpio_t int_pin;
    uint16_t chip_address;
}l9966_init_ctx_t;

typedef enum {
  L9966_RESET_CONDITION = 0,
  L9966_RESET_HARD_RESET,
  L9966_RESET_HARD_WAIT_RESET,
  L9966_RESET_HARD_SET,
  L9966_RESET_HARD_WAIT_SET,
  L9966_RESET_SOFT_STEP_PREPARE,
  L9966_RESET_SOFT_STEP_1_REQ,
  L9966_RESET_SOFT_STEP_2_REQ,
  L9966_RESET_SOFT_STEP_3_REQ,
  L9966_RESET_SOFT_WAIT,
  L9966_RESET_SOFT_VERSION_DEV_V_REQ,
  L9966_RESET_SOFT_VERSION_HW_REV_REQ,
  L9966_RESET_SOFT_VERSION_DEV_ID_REQ,
  L9966_RESET_SOFT_VERSION_VERIFY
}l9966_reset_fsm_state_t;

typedef enum {
  L9966_CHECK_STATUS_CONDITION = 0,
  L9966_CHECK_STATUS_SPI_REQ,
}l9966_check_status_fsm_state_t;

typedef enum {
  L9966_READ_DIG_IN_CONDITION = 0,
  L9966_READ_DIG_IN_SPI_REQ,
}l9966_read_dig_in_fsm_state_t;

typedef enum {
  L9966_CHECK_IRQ_FLAGS_CONDITION = 0,
  L9966_CHECK_IRQ_FLAGS_SPI_REQ,
}l9966_check_irq_flags_fsm_state_t;

typedef enum {
  L9966_READ_SQNCR_CONDITION = 0,
  L9966_READ_SQNCR_SPI_COPY_REQ,
  L9966_READ_SQNCR_SPI_READ_REQ,
}l9966_read_sqncr_fsm_state_t;

typedef enum {
  L9966_READ_SC_CONDITION = 0,
  L9966_READ_SC_SPI_READ_REQ,
}l9966_read_sc_fsm_state_t;

typedef enum {
  L9966_MAINTAIN_SC_CONDITION = 0,
  L9966_MAINTAIN_SC_SPI_WRITE_REQ,
}l9966_maintain_sc_fsm_state_t;

typedef enum {
  L9966_MAINTAIN_EU_CONDITION = 0,
  L9966_MAINTAIN_EU_SPI_WRITE_REQ,
}l9966_maintain_eu_fsm_state_t;

typedef enum {
  L9966_CONFIGURE_CONDITION = 0,
  L9966_CONFIGURE_SC_WAIT,
  L9966_CONFIGURE_EU_DISABLE,
  L9966_CONFIGURE_REG_TRANSLATE,
  L9966_CONFIGURE_CMD_DEFINE,
  L9966_CONFIGURE_SPI_WRITE_REQ,
  L9966_CONFIGURE_EU_RESTORE,
}l9966_configure_fsm_state_t;

typedef enum {
  L9966_PROCESS_RESET = 0,
  L9966_PROCESS_CHECK_STATUS,
  L9966_PROCESS_CHECK_IRQ_FLAGS,
  L9966_PROCESS_READ_SC,
  L9966_PROCESS_READ_SQNCR,
  L9966_PROCESS_READ_DIG_IN,
  L9966_PROCESS_CONFIGURE,
  L9966_PROCESS_SC_MAINTAIN,
  L9966_PROCESS_EU_MAINTAIN,
  L9966_PROCESS_MAX,
}l9966_fsm_state_t;

typedef struct {
    l9966_init_ctx_t init;

    bool ready;
    bool initialized;
    bool configured;
    bool configure_request;
    uint8_t configure_cmd_index;
    error_t configure_errcode;
    bool spi_busy;
    uint32_t bad_frames_reported_cnt;
    uint32_t bad_responses_cnt;
    uint32_t configuration_lost_cnt;


    bool version_valid;
    bool status_valid;
    l9966_ctrl_ver_t version;
    l9966_ctrl_gs_t status;
    l9966_config_t config;
    l9966_ctrl_sc_conf_t sc_control;
    l9966_reg_cfg_map_t register_map;

    uint16_t fsm_tx_addr;
    uint16_t fsm_tx_payload;
    uint16_t fsm_rx_payload;
    uint16_t fsm_rx_burst_payload[L9966_CHANNELS];

    bool reset_request;
    error_t reset_errcode;
    bool sc_enabled;
    bool eu_enabled[L9966_EU_COUNT];

    bool sc_enabled_accept;
    bool eu_enabled_accept[L9966_EU_COUNT];

    uint16_t tx_payload[16];
    uint16_t rx_payload[16];

    l9966_reset_fsm_state_t reset_fsm_state;
    l9966_check_status_fsm_state_t check_status_fsm_state;
    l9966_check_irq_flags_fsm_state_t check_irq_flags_fsm_state;
    l9966_read_sqncr_fsm_state_t read_sqncr_fsm_state;
    l9966_read_sc_fsm_state_t read_sc_fsm_state;
    l9966_read_dig_in_fsm_state_t read_dig_in_fsm_state;
    l9966_maintain_sc_fsm_state_t sc_maintain_fsm_state;
    l9966_maintain_eu_fsm_state_t eu_maintain_fsm_state;
    l9966_configure_fsm_state_t configure_fsm_state;

    l9966_fsm_state_t process_fsm;

    time_us_t sqncr_last;
    time_delta_us_t sqncr_diff;
    float sqncr_cmd_results[L9966_CHANNELS];
    uint16_t sqncr_cmd_results_raw[L9966_CHANNELS];
    uint16_t sqncr_cmd_ready_mask;

    float sc_result;

    bool sc_ready;

    bool digital_inputs_valid;
    l9966_ctrl_dig_inputs_t digital_inputs;
    time_us_t status_check_last;
    time_us_t read_dig_in_last;
    time_us_t sqncr_result_poll_last;
    time_us_t reset_action_timestamp;
    time_us_t sync_timestamp;

    bool sc_int;
    bool eu_int[L9966_EU_COUNT];

    bool int_triggered;

}l9966_ctx_t;

error_t l9966_init(l9966_ctx_t *ctx, const l9966_init_ctx_t *init_ctx);
void l9966_loop_main(l9966_ctx_t *ctx);
void l9966_loop_slow(l9966_ctx_t *ctx);
void l9966_loop_fast(l9966_ctx_t *ctx);

error_t l9966_configure(l9966_ctx_t *ctx, const l9966_config_t *config);

error_t l9966_reset(l9966_ctx_t *ctx);

error_t l9966_get_version(l9966_ctx_t *ctx, l9966_ctrl_ver_t *ver);
error_t l9966_get_status(l9966_ctx_t *ctx, l9966_ctrl_gs_t *status);
error_t l9966_get_inputs(l9966_ctx_t *ctx, l9966_ctrl_dig_inputs_t *dig_inputs);

error_t l9966_get_sqncr_output(l9966_ctx_t *ctx, uint8_t cmd_index, float *sqncr_output);

error_t l9966_start_sc(l9966_ctx_t *ctx, const l9966_ctrl_sc_conf_t *cfg);
error_t l9966_poll_sc(l9966_ctx_t *ctx);
error_t l9966_get_sc(l9966_ctx_t *ctx, float *sc_output);

void l9966_int_irq_handler(l9966_ctx_t *ctx);

#endif /* DRIVERS_L9966_INC_L9966_H_ */
