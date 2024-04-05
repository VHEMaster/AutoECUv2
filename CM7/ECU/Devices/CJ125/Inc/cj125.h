/*
 * cj125.h
 *
 *  Created on: Apr 5, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_CJ125_INC_CJ125_H_
#define DRIVERS_CJ125_INC_CJ125_H_

#include "gpio.h"
#include "main.h"
#include "errors.h"

#include "spi.h"

typedef struct {
    spi_slave_t spi;
    gpio_t nrst_pin;
}cj125_init_ctx_t;

typedef struct {
    cj125_init_ctx_t init;

}cj125_ctx_t;

error_t cj125_init(cj125_ctx_t *ctx, const cj125_init_ctx_t *init_ctx);
void cj125_loop_main(cj125_ctx_t *ctx);
void cj125_loop_slow(cj125_ctx_t *ctx);
void cj125_loop_fast(cj125_ctx_t *ctx);

#endif /* DRIVERS_CJ125_INC_CJ125_H_ */
