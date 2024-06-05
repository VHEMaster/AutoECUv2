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
#include "time.h"
#include "spi.h"

#define TLE6240_DEFAULT_POLL_PERIOD_US      (10 * TIME_US_IN_MS)
#define TLE6240_SPI_MODE                    (SPI_MODE_1)
#define TLE6240_ECHO_INCREMENT_VALUE        (0xF15A)

#define TLE6240_CTRL_CH1_CH8_FULL_DIAG      (0x00)
#define TLE6240_CTRL_CH1_CH8_STATE_DIAG     (0xC0)
#define TLE6240_CTRL_CH1_CH8_ECHO           (0xA0)
#define TLE6240_CTRL_CH1_CH8_OR_WRITE_OR    (0x30)
#define TLE6240_CTRL_CH1_CH8_OR_WRITE_AND   (0xF0)

#define TLE6240_CTRL_CH9_CH16_FULL_DIAG     (0x0F)
#define TLE6240_CTRL_CH9_CH16_STATE_DIAG    (0xCF)
#define TLE6240_CTRL_CH9_CH16_ECHO          (0xAF)
#define TLE6240_CTRL_CH9_CH16_OR_WRITE_OR   (0x3F)
#define TLE6240_CTRL_CH9_CH16_OR_WRITE_AND  (0xFF)

typedef enum {
  TLE6240_DIAG_NORMAL = 0,
  TLE6240_DIAG_OVERLOAD = 1,
  TLE6240_DIAG_OPENLOAD = 2,
  TLE6240_DIAG_SHORTGND = 3,
}tle6240_diag_codes_t;

typedef enum {
  TLE6240_PROCESS_RESET = -1,
  TLE6240_PROCESS_CONDITION,
  TLE6240_PROCESS_WRITE_1_TO_8,
  TLE6240_PROCESS_WRITE_9_TO_16,
  TLE6240_PROCESS_ECHO_REQUEST,
  TLE6240_PROCESS_ECHO_CHECK,
  TLE6240_PROCESS_MAX,
}tle6240_process_fsm_t;

typedef struct {
    error_t comm_status;
    union {
        uint32_t dword;
        uint16_t hword[2];
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
    }diag;
}tle6240_diag_t;

typedef struct {
    spi_slave_t *spi_slave;
    tle6240_diag_t diag;
    gpio_t input_pins[8];
    gpio_t reset_pin;
    gpio_t fault_pin;
}tle6240_init_ctx_t;

typedef struct {
    tle6240_init_ctx_t init;
    tle6240_diag_t diag;
    tle6240_process_fsm_t process_fsm;
    uint16_t output_temp;
    uint16_t output_state;
    bool output_updated;
    time_delta_us_t poll_period;
    time_us_t poll_last;
    bool spi_busy;
    bool ready;
    uint16_t echo_value;

    uint8_t ctrl;
    uint8_t data;
    uint16_t diag_hword;

    uint16_t tx_payload;
    uint16_t rx_payload;
}tle6240_ctx_t;

error_t tle6240_init(tle6240_ctx_t *ctx, const tle6240_init_ctx_t *init_ctx);
void tle6240_loop_main(tle6240_ctx_t *ctx);
void tle6240_loop_slow(tle6240_ctx_t *ctx);
void tle6240_loop_fast(tle6240_ctx_t *ctx);

error_t tle6240_write_all_masked(tle6240_ctx_t *ctx, uint16_t mask, uint16_t value);
error_t tle6240_write_all(tle6240_ctx_t *ctx, uint16_t value);

error_t tle6240_update(tle6240_ctx_t *ctx);
error_t tle6240_diagnostics(tle6240_ctx_t *ctx, tle6240_diag_t *diag);
error_t tle6240_set_poll_period(tle6240_ctx_t *ctx, time_delta_us_t period);

error_t tle6240_ch_write(tle6240_ctx_t *ctx, uint8_t channel, bool value);
error_t tle6240_ch_read(tle6240_ctx_t *ctx, uint8_t channel, bool *value);

bool tle6240_ch_is_serial(tle6240_ctx_t *ctx, uint8_t channel);
bool tle6240_ch_is_direct(tle6240_ctx_t *ctx, uint8_t channel);

#endif /* DRIVERS_TLE6240GP_INC_TLE6240_H_ */
