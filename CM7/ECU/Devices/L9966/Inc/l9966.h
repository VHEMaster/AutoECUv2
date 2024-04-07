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
#include "l9966_config.h"

#define L9966_DEFAULT_DIGITAL_POLL_PERIOD     (10 * TIME_US_IN_MS)
#define L9966_RESET_TIMEOUT                   (100 * TIME_US_IN_MS)
#define L9966_SPI_MODE                        (SPI_MODE_1)

typedef struct {
    spi_slave_t *spi_slave;
    gpio_t wake_pin;
    gpio_t sync_pin;
    gpio_t nrst_pin;
    uint8_t chip_address;
}l9966_init_ctx_t;

typedef enum {
  L9966_RESET_IDLE = 0,
  L9966_RESET_HARD_RESET,
  L9966_RESET_HARD_WAIT_RESET,
  L9966_RESET_HARD_SET,
  L9966_RESET_HARD_WAIT_SET,
  L9966_RESET_SOFT_VERSION_REQ,
  L9966_RESET_SOFT_VERSION_WAIT,
  L9966_RESET_SOFT_VERSION_VERIFY,
  L9966_RESET_SOFT_STEP_1_REQ,
  L9966_RESET_SOFT_STEP_1_WAIT,
  L9966_RESET_SOFT_STEP_2_REQ,
  L9966_RESET_SOFT_STEP_2_WAIT,
  L9966_RESET_SOFT_STEP_3_REQ,
  L9966_RESET_SOFT_STEP_3_WAIT,
  L9966_RESET_FAILURE,
  L9966_RESET_DONE
}l9966_reset_step_t;

typedef struct {
    l9966_init_ctx_t init;

    float rrx[L9966_RRx_COUNT];
    time_delta_us_t digital_poll_period;
    bool initialized;
    bool configured;

    l9966_ctrl_ver_t version;
    l9966_ctrl_gs_t status;
    l9966_config_t config;

    bool sc_enabled;
    bool eu_enabled[L9966_EU_COUNT];

    l9966_reset_step_t reset_step;

    float sqncr_cmd_results[L9966_CHANNELS];
    float sc_result;

    uint16_t sqncr_cmd_ready_mask_temp;
    uint16_t sqncr_cmd_ready_mask;
    bool sc_ready;

    l9966_ctrl_dig_inputs_t digital_inputs;
    time_us_t digital_poll_last;
    time_us_t reset_timestamp;

}l9966_ctx_t;

error_t l9966_init(l9966_ctx_t *ctx, const l9966_init_ctx_t *init_ctx);
void l9966_loop_main(l9966_ctx_t *ctx);
void l9966_loop_slow(l9966_ctx_t *ctx);
void l9966_loop_fast(l9966_ctx_t *ctx);

error_t l9966_write_config(l9966_ctx_t *ctx, const l9966_config_t *config);
error_t l9966_set_rrx_value(l9966_ctx_t *ctx, uint8_t rrx, float value);
error_t l9966_set_dig_poll_period(l9966_ctx_t *ctx, time_delta_us_t period);

error_t l9966_reset(l9966_ctx_t *ctx);

error_t l9966_read_version(l9966_ctx_t *ctx, l9966_ctrl_ver_t *ver);
error_t l9966_read_status(l9966_ctx_t *ctx, l9966_ctrl_gs_t *status);
error_t l9966_read_inputs(l9966_ctx_t *ctx, l9966_ctrl_dig_inputs_t *dig_inputs);

error_t l9966_start_sqncr(l9966_ctx_t *ctx);
error_t l9966_read_sqncr_output(l9966_ctx_t *ctx, uint8_t cmd_index, float *sqncr_output);

error_t l9966_start_sc(l9966_ctx_t *ctx, const l9966_ctrl_sc_conf_t *cfg);
error_t l9966_poll_sc(l9966_ctx_t *ctx);
error_t l9966_read_sc(l9966_ctx_t *ctx, float *sc_output);

void l9966_int_irq_handler(l9966_ctx_t *ctx);

#endif /* DRIVERS_L9966_INC_L9966_H_ */
