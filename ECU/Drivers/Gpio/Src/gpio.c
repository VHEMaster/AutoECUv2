/*
 * gpio.c
 *
 *  Created on: Mar 22, 2024
 *      Author: VHEMaster
 */

#include "gpio.h"
#include "compiler.h"
#include "errors.h"

void gpio_init(gpio_t *ctx, GPIO_TypeDef *port, uint16_t pin)
{

}

void gpio_configure_direct_output(gpio_t *ctx, bool initial_value)
{
  GPIO_InitTypeDef gpio_init = {0};

  gpio_write(ctx, initial_value);

  gpio_init.Pin = ctx->pin;
  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(ctx->port, &gpio_init);
}

void gpio_configure_pwm_output(gpio_t *ctx, uint32_t alternate)
{
  GPIO_InitTypeDef gpio_init = {0};

  gpio_init.Pin = ctx->pin;
  gpio_init.Mode = GPIO_MODE_AF_PP;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = alternate;
  HAL_GPIO_Init(ctx->port, &gpio_init);
}
