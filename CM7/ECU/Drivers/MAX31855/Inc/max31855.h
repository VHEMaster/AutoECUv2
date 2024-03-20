/*
 * max31855.h
 *
 *  Created on: Mar 19, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_MAX31855_INC_MAX31855_H_
#define DRIVERS_MAX31855_INC_MAX31855_H_

#include "errors.h"
#include "time.h"
#include "gpio.h"

#include "../../../Drivers/Spi/Inc/spi.h"

#define MAX31855_DEFAULT_POLL_PERIOD_US   (100 * TIME_MS_TO_US)

typedef struct {
    union {
        uint8_t byte;
        struct {
            bool comm : 1;
            bool scv : 1;
            bool scg : 1;
            bool oc : 1;
        }bits;
    }u;
}max31855_diag_t;

typedef struct {
    spi_slave_t spi;
    float reference;
    float temperature;
    max31855_diag_t diag;
}max31855_ctx_t;

error_t max31855_init(max31855_ctx_t *ctx);
error_t max31855_loop_main(max31855_ctx_t *ctx);
error_t max31855_loop_slow(max31855_ctx_t *ctx);
error_t max31855_loop_fast(max31855_ctx_t *ctx);

error_t max31855_diagnostics(max31855_ctx_t *ctx, max31855_diag_t *diag);
error_t max31855_set_poll_period(max31855_ctx_t *ctx, time_delta_us_t period);

error_t max31855_update(tle6240_ctx_t *ctx);
error_t max31855_get_temperature(tle6240_ctx_t *ctx, float *temperature);


#endif /* DRIVERS_MAX31855_INC_MAX31855_H_ */
