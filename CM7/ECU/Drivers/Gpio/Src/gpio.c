/*
 * gpio.c
 *
 *  Created on: Mar 22, 2024
 *      Author: VHEMaster
 */

#include "gpio.h"
#include "compiler.h"

void gpio_init(gpio_t *ctx, GPIO_TypeDef *port, uint16_t pin)
{

}

void gpio_clear(gpio_t *ctx)
{
  ctx->port = NULL;
  ctx->pin = 0;
}

ITCM_FUNC INLINE bool gpio_valid(const gpio_t *ctx)
{
  return ctx->port != NULL && ctx->pin != 0;
}

ITCM_FUNC INLINE void gpio_set(gpio_t *ctx)
{
  ctx->port->BSRR |= ctx->pin;
}

ITCM_FUNC INLINE void gpio_reset(gpio_t *ctx)
{
  ctx->port->BSRR |= ctx->pin << 16u;
}

ITCM_FUNC INLINE bool gpio_read(const gpio_t *ctx)
{
  return (ctx->port->IDR & ctx->pin) > 0 ? true : false;
}

ITCM_FUNC INLINE void gpio_write(gpio_t *ctx, bool value)
{
  if(value) {
    gpio_set(ctx);
  } else {
    gpio_reset(ctx);
  }
}

ITCM_FUNC INLINE void gpio_toggle(gpio_t *ctx)
{
  uint32_t odr = ctx->port->ODR;

  ctx->port->BSRR = ((odr & ctx->pin) << 16U) | (~odr & ctx->pin);
}
