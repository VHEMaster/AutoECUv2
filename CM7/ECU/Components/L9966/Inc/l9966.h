/*
 * l9966.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef COMPONENTS_L9966_INC_L9966_H_
#define COMPONENTS_L9966_INC_L9966_H_

#include "main.h"
#include "errors.h"
#include "gpio_pin.h"

typedef struct {
    SPI_HandleTypeDef *hspi;
    gpio_pin_t spi_nss_gpio_pin;
    gpio_pin_t wake_gpio_pin;
    gpio_pin_t sync_gpio_pin;
    gpio_pin_t nrst_gpio_pin;
    uint8_t chip_address;
}l9966_init_ctx_t;

typedef struct {
    l9966_init_ctx_t init;

}l9966_ctx_t;

error_t l9966_init(l9966_ctx_t *ctx, const l9966_init_ctx_t *init_ctx);
void l9966_loop_main(void);
void l9966_loop_slow(void);
void l9966_loop_fast(void);

#endif /* COMPONENTS_L9966_INC_L9966_H_ */
