/*
 * tpic8101.h
 *
 *  Created on: Mar 19, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_TPIC8101_INC_TPIC8101_H_
#define DRIVERS_TPIC8101_INC_TPIC8101_H_

#include "errors.h"
#include "time.h"
#include "gpio.h"

#include "../../../Drivers/Spi/Inc/spi.h"

typedef struct {
    spi_slave_t spi;
    gpio_t int_pin;

    uint8_t int_time;
    uint8_t filter_freq;
    uint8_t gain;
    uint8_t osc_freq;
    uint8_t channel;

    uint16_t integrator_value;
}tpic8101_ctx_t;

error_t tpic8101_init(tpic8101_ctx_t *ctx);
error_t tpic8101_loop_main(tpic8101_ctx_t *ctx);
error_t tpic8101_loop_slow(tpic8101_ctx_t *ctx);
error_t tpic8101_loop_fast(tpic8101_ctx_t *ctx);

void tpic8101_int_hold(tpic8101_ctx_t *ctx);
void tpic8101_int_integrate(tpic8101_ctx_t *ctx);

error_t tpic8101_set_int_time(tpic8101_ctx_t *ctx, uint8_t value);
error_t tpic8101_set_filter_freq(tpic8101_ctx_t *ctx, uint8_t value);
error_t tpic8101_set_gain(tpic8101_ctx_t *ctx, uint8_t value);
error_t tpic8101_set_osc_freq(tpic8101_ctx_t *ctx, uint8_t value);
error_t tpic8101_set_channel(tpic8101_ctx_t *ctx, uint8_t value);
error_t tpic8101_cfg_update(tpic8101_ctx_t *ctx);

error_t tpic8101_get_integrator_value(tpic8101_ctx_t *ctx, uint16_t *value);

#endif /* DRIVERS_TPIC8101_INC_TPIC8101_H_ */
