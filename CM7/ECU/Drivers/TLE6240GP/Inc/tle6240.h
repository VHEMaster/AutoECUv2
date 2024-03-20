/*
 * tle6240.h
 *
 *  Created on: Mar 19, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_TLE6240GP_INC_TLE6240_H_
#define DRIVERS_TLE6240GP_INC_TLE6240_H_

#include "errors.h"
#include "gpio.h"

#include "../../../Drivers/Spi/Inc/spi.h"

#define MAX31855_DEFAULT_POLL_PERIOD_US   (1 * TIME_MS_TO_US)

typedef enum {
  TLE6240_DIAG_NORMAL = 0,
  TLE6240_DIAG_OVERLOAD = 1,
  TLE6240_DIAG_OPENLOAD = 2,
  TLE6240_DIAG_SHORTGND = 3,
}tle6240_diag_codes_t;

typedef struct {
    union {
        uint32_t word;
        uint8_t bytes[4];
        struct {
            tle6240_diag_codes_t ch1 : 2;
            tle6240_diag_codes_t ch2 : 2;
            tle6240_diag_codes_t ch3 : 2;
            tle6240_diag_codes_t ch4 : 2;
            tle6240_diag_codes_t ch5 : 2;
            tle6240_diag_codes_t ch6 : 2;
            tle6240_diag_codes_t ch7 : 2;
            tle6240_diag_codes_t ch8 : 2;
            tle6240_diag_codes_t ch9 : 2;
            tle6240_diag_codes_t ch10 : 2;
            tle6240_diag_codes_t ch11 : 2;
            tle6240_diag_codes_t ch12 : 2;
            tle6240_diag_codes_t ch13 : 2;
            tle6240_diag_codes_t ch14 : 2;
            tle6240_diag_codes_t ch15 : 2;
            tle6240_diag_codes_t ch16 : 2;
        }data;
    }u;
}tle6240_diag_t;

typedef struct {
    spi_slave_t spi;
    gpio_t input_pins[8];
    gpio_t reset_pin;
    gpio_t fault_pin;
    tle6240_diag_t diag;
    uint16_t output_state;
}tle6240_ctx_t;

error_t tle6240_init(tle6240_ctx_t *ctx);
error_t tle6240_loop_main(tle6240_ctx_t *ctx);
error_t tle6240_loop_slow(tle6240_ctx_t *ctx);
error_t tle6240_loop_fast(tle6240_ctx_t *ctx);

error_t tle6240_write_all_masked(tle6240_ctx_t *ctx, uint16_t mask, uint16_t value);
error_t tle6240_write_all(tle6240_ctx_t *ctx, uint16_t value);

error_t tle6240_update(tle6240_ctx_t *ctx);
error_t tle6240_diagnostics(tle6240_ctx_t *ctx, tle6240_diag_t *diag);
error_t tle6240_set_poll_period(tle6240_ctx_t *ctx, time_delta_us_t period);

void tle6240_ch_write(tle6240_ctx_t *ctx, uint8_t channel, bool value);
bool tle6240_ch_read(tle6240_ctx_t *ctx, uint8_t channel);


bool tle6240_ch_is_serial(tle6240_ctx_t *ctx, uint8_t channel);
bool tle6240_ch_is_direct(tle6240_ctx_t *ctx, uint8_t channel);





#endif /* DRIVERS_TLE6240GP_INC_TLE6240_H_ */
