/*
 * l9966.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef COMPONENTS_L9966_INC_L9966_H_
#define COMPONENTS_L9966_INC_L9966_H_

#include "gpio.h"
#include "main.h"
#include "errors.h"
#include "spi.h"

typedef struct {
    spi_slave_t spi;
    gpio_t wake_pin;
    gpio_t sync_pin;
    gpio_t nrst_pin;
    uint8_t chip_address;
}l9966_init_ctx_t;

typedef struct {
    l9966_init_ctx_t init;

}l9966_ctx_t;

error_t l9966_init(l9966_ctx_t *ctx, const l9966_init_ctx_t *init_ctx);
void l9966_loop_main(l9966_ctx_t *ctx);
void l9966_loop_slow(l9966_ctx_t *ctx);
void l9966_loop_fast(l9966_ctx_t *ctx);

error_t l9966_reg_read(l9966_ctx_t *ctx, uint8_t reg, uint16_t *data);
error_t l9966_reg_write(l9966_ctx_t *ctx, uint8_t reg, uint16_t data);

error_t l9966_burst_reg_read(l9966_ctx_t *ctx, uint8_t reg, uint16_t *data, uint8_t len);
error_t l9966_burst_reg_write(l9966_ctx_t *ctx, uint8_t reg, const uint16_t *data, uint8_t len);


#endif /* COMPONENTS_L9966_INC_L9966_H_ */
