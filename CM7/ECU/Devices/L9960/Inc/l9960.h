/*
 * l9960.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_L9960_INC_L9960_H_
#define DRIVERS_L9960_INC_L9960_H_

#include "gpio.h"
#include "main.h"
#include "errors.h"

#include "spi.h"

typedef struct {
    spi_slave_t spi;
    gpio_t nrst_pin;
}l9960_init_ctx_t;

typedef struct {
    l9960_init_ctx_t init;

}l9960_ctx_t;

error_t l9960_init(l9960_ctx_t *ctx, const l9960_init_ctx_t *init_ctx);
void l9960_loop_main(l9960_ctx_t *ctx);
void l9960_loop_slow(l9960_ctx_t *ctx);
void l9960_loop_fast(l9960_ctx_t *ctx);

#endif /* DRIVERS_L9960_INC_L9960_H_ */
