/*
 * max31855.h
 *
 *  Created on: Mar 19, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_MAX31855_INC_MAX31855_H_
#define DRIVERS_MAX31855_INC_MAX31855_H_

#include "common.h"
#include "time.h"
#include "gpio.h"

#include "spi.h"

#define MAX31855_DEFAULT_POLL_PERIOD_US   (100 * TIME_US_IN_MS)
#define MAX31855_SPI_MODE           (SPI_MODE_0)

typedef union {
    uint8_t byte;
    struct {
        bool comm : 1;
        bool scv : 1;
        bool scg : 1;
        bool oc : 1;
    }bits;
}max31855_diag_t;

typedef struct {
    float temperature;
    float reference;
}max31855_data_t;

typedef union {
    uint32_t dword;
    struct {
        bool diag_fault_oc : 1;
        bool diag_fault_scg : 1;
        bool diag_fault_scv : 1;
        bool always_zero_0 : 1;
        uint16_t junction_reference : 12;
        bool diag_fault : 1;
        bool always_zero_1 : 1;
        uint16_t temperature_data : 14;
    }bits;
}max31855_payload_t;

typedef struct {
    spi_slave_t *spi_slave;
    float reference;
    float temperature;
    max31855_payload_t data;
    uint32_t payload_rx;
    uint32_t payload_tx;
    time_us_t time_last;
    bool update_triggered;
    time_delta_us_t poll_period;
    error_t comm_errorcode;
    bool comm_busy;
    bool ready;
}max31855_ctx_t;

error_t max31855_init(max31855_ctx_t *ctx, spi_slave_t *spi_slave);
void max31855_loop_main(max31855_ctx_t *ctx);
void max31855_loop_slow(max31855_ctx_t *ctx);
void max31855_loop_fast(max31855_ctx_t *ctx);

error_t max31855_set_poll_period(max31855_ctx_t *ctx, time_delta_us_t period);

error_t max31855_trigger_update(max31855_ctx_t *ctx);

error_t max31855_get_diag(max31855_ctx_t *ctx, max31855_diag_t *diag);
error_t max31855_get_data(max31855_ctx_t *ctx, max31855_data_t *data);


#endif /* DRIVERS_MAX31855_INC_MAX31855_H_ */
