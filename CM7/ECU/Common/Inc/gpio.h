/*
 * gpio.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef COMMON_INC_GPIO_H_
#define COMMON_INC_GPIO_H_

#include "main.h"
#include "bool.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
}gpio_t;

void gpio_init(gpio_t *ctx, GPIO_TypeDef *port, uint16_t pin);
void gpio_clear(gpio_t *ctx);

bool gpio_valid(const gpio_t *ctx);

void gpio_set(gpio_t *ctx);
void gpio_reset(gpio_t *ctx);

bool gpio_read(const gpio_t *ctx);
void gpio_write(gpio_t *ctx, bool value);
void gpio_toggle(gpio_t *ctx);

#endif /* COMMON_INC_GPIO_H_ */
