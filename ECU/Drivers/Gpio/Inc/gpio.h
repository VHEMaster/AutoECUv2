/*
 * gpio.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_GPIO_INC_GPIO_H_
#define DRIVERS_GPIO_INC_GPIO_H_

#include "common.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
}gpio_t;

void gpio_init(gpio_t *ctx, GPIO_TypeDef *port, uint16_t pin);

void gpio_configure_direct_output(gpio_t *ctx, bool initial_value);
void gpio_configure_pwm_output(gpio_t *ctx, uint32_t alternate);

STATIC_INLINE void gpio_clear(gpio_t *ctx)
{
  ctx->port = NULL;
  ctx->pin = 0;
}

STATIC_INLINE bool gpio_valid(const gpio_t *ctx)
{
  return ctx->port != NULL && ctx->pin != 0;
}

STATIC_INLINE void gpio_set(gpio_t *ctx)
{
  ctx->port->BSRR |= ctx->pin;
}

STATIC_INLINE void gpio_reset(gpio_t *ctx)
{
  ctx->port->BSRR |= ctx->pin << 16u;
}

STATIC_INLINE bool gpio_read(const gpio_t *ctx)
{
  return (ctx->port->IDR & ctx->pin) > 0 ? true : false;
}

STATIC_INLINE void gpio_write(gpio_t *ctx, bool value)
{
  if(value) {
    gpio_set(ctx);
  } else {
    gpio_reset(ctx);
  }
}

STATIC_INLINE void gpio_toggle(gpio_t *ctx)
{
  uint32_t odr = ctx->port->ODR;

  ctx->port->BSRR = ((odr & ctx->pin) << 16U) | (~odr & ctx->pin);
}

#endif /* DRIVERS_GPIO_INC_GPIO_H_ */
