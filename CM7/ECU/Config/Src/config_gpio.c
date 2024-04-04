/*
 * config_gpio.c
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include <limits.h>
#include <math.h>
#include "config_gpio.h"
#include "config_extern.h"
#include "config_rcc.h"
#include "compiler.h"
#include "gpio.h"

typedef struct {
    gpio_t pin;
    ecu_gpio_output_if_t if_id;
    output_ch_id_t output_ch_id;
    output_id_t output_id;
    uint32_t tim_alternate;
    uint32_t tim_channel;
    bool gpio_invert;
    ecu_gpio_output_type_t type;
    void *usrdata;
}ecu_config_gpio_output_t;

typedef struct {
    gpio_t pin;
    ecu_gpio_input_if_t if_id;
    input_ch_id_t input_ch_id;
    input_id_t input_id;
    TIM_HandleTypeDef *htim;
    uint32_t tim_alternate;
    uint32_t tim_channel_rising;
    uint32_t tim_channel_falling;
    uint32_t tim_channel_rising_falling;
    ecu_config_gpio_input_cb_t irq_cb;
    bool gpio_invert;
    void *usrdata;
}ecu_config_gpio_input_t;

typedef struct {
    input_if_cfg_t cfg;
    input_if_id_t input_if_id;
    ecu_config_gpio_input_t *channels[ECU_IN_MAX];
    void *usrdata;
}ecu_config_gpio_inputs_if_t;

typedef struct {
    output_if_id_t output_if_id;
    output_if_cfg_t cfg;
    ecu_config_gpio_output_if_pwm_cfg_t pwm_cfg;
    TIM_HandleTypeDef *htim;
    ecu_config_gpio_output_t *channels[ECU_OUT_MAX];
    void *usrdata;
}ecu_config_gpio_outputs_if_t;

typedef struct {
    ecu_config_gpio_outputs_if_t outputs_if[ECU_OUT_IF_MAX];
    ecu_config_gpio_inputs_if_t inputs_if[ECU_IN_IF_MAX];
    ecu_config_gpio_output_t outputs[ECU_OUT_MAX];
    ecu_config_gpio_input_t inputs[ECU_IN_MAX];
}ecu_config_gpio_t;

static error_t ecu_config_gpio_ch_set(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata);
static error_t ecu_config_gpio_spi_ch_set(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata);

static ecu_config_gpio_t ecu_config_gpio = {
    .outputs_if = {
        {
            .htim = &htim1,
            .cfg = {. ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS1_PWM1
        {
            .htim = &htim13,
            .cfg = {. ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS1_PWM2
        {
            .htim = &htim14,
            .cfg = {. ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS1_PWM3
        {
            .htim = &htim16,
            .cfg = {. ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS1_PWM4
        {
            .htim = &htim17,
            .cfg = {. ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS1_PWM5
        {
            .cfg = {. ch_set = ecu_config_gpio_spi_ch_set, },
        }, //ECU_OUT_IF_OUTS1_SPI
        {
            .htim = &htim3,
            .cfg = {. ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS2_PWM1
        {
            .htim = &htim4,
            .cfg = {. ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS2_PWM2
        {
            .cfg = {. ch_set = ecu_config_gpio_spi_ch_set, },
        }, //ECU_OUT_IF_OUTS2_SPI
        {
            .cfg = {. ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_IGN
    },
    .inputs_if = {
        {

        }, //ECU_IN_IF_FLEXIO1
        {

        }, //ECU_IN_IF_FLEXIO2
        {

        }, //ECU_IN_IF_EGT1
        {

        }, //ECU_IN_IF_EGT2
        {

        }, //ECU_IN_IF_ADC1
    },
    .outputs = {
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM1,
            .output_ch_id = 0,
            .pin = { .port = OUTS1_CH1_GPIO_Port, .pin = OUTS1_CH1_Pin },
            .tim_alternate = GPIO_AF1_TIM1,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = false,
        }, //ECU_OUT_PORT1_PIN1
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM1,
            .output_ch_id = 1,
            .pin = { .port = OUTS1_CH2_GPIO_Port, .pin = OUTS1_CH2_Pin },
            .tim_alternate = GPIO_AF1_TIM1,
            .tim_channel = TIM_CHANNEL_2,
            .gpio_invert = false,
        }, //ECU_OUT_PORT1_PIN2
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM1,
            .output_ch_id = 2,
            .pin = { .port = OUTS1_CH3_GPIO_Port, .pin = OUTS1_CH3_Pin },
            .tim_alternate = GPIO_AF1_TIM1,
            .tim_channel = TIM_CHANNEL_3,
            .gpio_invert = false,
        }, //ECU_OUT_PORT1_PIN3
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM1,
            .output_ch_id = 3,
            .pin = { .port = OUTS1_CH4_GPIO_Port, .pin = OUTS1_CH4_Pin },
            .tim_alternate = GPIO_AF1_TIM1,
            .tim_channel = TIM_CHANNEL_4,
            .gpio_invert = false,
        }, //ECU_OUT_PORT1_PIN4
        {
            .if_id = ECU_OUT_IF_OUTS1_SPI,
            .output_ch_id = 4,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT1_PIN5
        {
            .if_id = ECU_OUT_IF_OUTS1_SPI,
            .output_ch_id = 5,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT1_PIN6
        {
            .if_id = ECU_OUT_IF_OUTS1_SPI,
            .output_ch_id = 6,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT1_PIN7
        {
            .if_id = ECU_OUT_IF_OUTS1_SPI,
            .output_ch_id = 7,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT1_PIN8
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM2,
            .output_ch_id = 8,
            .pin = { .port = OUTS1_IN9_GPIO_Port, .pin = OUTS1_IN9_Pin },
            .tim_alternate = GPIO_AF9_TIM13,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = false,
        }, //ECU_OUT_PORT1_PIN9
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM2,
            .output_ch_id = 9,
            .pin = { .port = OUTS1_IN10_GPIO_Port, .pin = OUTS1_IN10_Pin },
            .tim_alternate = GPIO_AF9_TIM14,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = false,
        }, //ECU_OUT_PORT1_PIN10
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM2,
            .output_ch_id = 10,
            .pin = { .port = OUTS1_IN11_GPIO_Port, .pin = OUTS1_IN11_Pin },
            .tim_alternate = GPIO_AF1_TIM16,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = false,
        }, //ECU_OUT_PORT1_PIN11
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM2,
            .output_ch_id = 11,
            .pin = { .port = OUTS1_IN12_GPIO_Port, .pin = OUTS1_IN12_Pin },
            .tim_alternate = GPIO_AF1_TIM17,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = false,
        }, //ECU_OUT_PORT1_PIN12
        {
            .if_id = ECU_OUT_IF_OUTS1_SPI,
            .output_ch_id = 12,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT1_PIN13
        {
            .if_id = ECU_OUT_IF_OUTS1_SPI,
            .output_ch_id = 13,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT1_PIN14
        {
            .if_id = ECU_OUT_IF_OUTS1_SPI,
            .output_ch_id = 14,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT1_PIN15
        {
            .if_id = ECU_OUT_IF_OUTS1_SPI,
            .output_ch_id = 15,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT1_PIN16
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM1,
            .output_ch_id = 0,
            .pin = { .port = OUTS2_CH1_GPIO_Port, .pin = OUTS2_CH1_Pin },
            .tim_alternate = GPIO_AF2_TIM3,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = false,
        }, //ECU_OUT_PORT2_PIN1
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM1,
            .output_ch_id = 1,
            .pin = { .port = OUTS2_CH2_GPIO_Port, .pin = OUTS2_CH2_Pin },
            .tim_alternate = GPIO_AF2_TIM3,
            .tim_channel = TIM_CHANNEL_2,
            .gpio_invert = false,
        }, //ECU_OUT_PORT2_PIN2
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM1,
            .output_ch_id = 2,
            .pin = { .port = OUTS2_CH3_GPIO_Port, .pin = OUTS2_CH3_Pin },
            .tim_alternate = GPIO_AF2_TIM3,
            .tim_channel = TIM_CHANNEL_3,
            .gpio_invert = false,
        }, //ECU_OUT_PORT2_PIN3
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM1,
            .output_ch_id = 3,
            .pin = { .port = OUTS2_CH4_GPIO_Port, .pin = OUTS2_CH4_Pin },
            .tim_alternate = GPIO_AF2_TIM3,
            .tim_channel = TIM_CHANNEL_4,
            .gpio_invert = false,
        }, //ECU_OUT_PORT2_PIN4
        {
            .if_id = ECU_OUT_IF_OUTS2_SPI,
            .output_ch_id = 4,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT2_PIN5
        {
            .if_id = ECU_OUT_IF_OUTS2_SPI,
            .output_ch_id = 5,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT2_PIN6
        {
            .if_id = ECU_OUT_IF_OUTS2_SPI,
            .output_ch_id = 6,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT2_PIN7
        {
            .if_id = ECU_OUT_IF_OUTS2_SPI,
            .output_ch_id = 7,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT2_PIN8
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM2,
            .output_ch_id = 8,
            .pin = { .port = OUTS2_CH9_GPIO_Port, .pin = OUTS2_CH9_Pin },
            .tim_alternate = GPIO_AF2_TIM4,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = false,
        }, //ECU_OUT_PORT2_PIN9
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM2,
            .output_ch_id = 9,
            .pin = { .port = OUTS2_CH10_GPIO_Port, .pin = OUTS2_CH10_Pin },
            .tim_alternate = GPIO_AF2_TIM4,
            .tim_channel = TIM_CHANNEL_2,
            .gpio_invert = false,
        }, //ECU_OUT_PORT2_PIN10
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM2,
            .output_ch_id = 10,
            .pin = { .port = OUTS2_CH11_GPIO_Port, .pin = OUTS2_CH11_Pin },
            .tim_alternate = GPIO_AF2_TIM4,
            .tim_channel = TIM_CHANNEL_3,
            .gpio_invert = false,
        }, //ECU_OUT_PORT2_PIN11
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM2,
            .output_ch_id = 11,
            .pin = { .port = OUTS2_CH12_GPIO_Port, .pin = OUTS2_CH12_Pin },
            .tim_alternate = GPIO_AF2_TIM4,
            .tim_channel = TIM_CHANNEL_4,
            .gpio_invert = false,
        }, //ECU_OUT_PORT2_PIN12
        {
            .if_id = ECU_OUT_IF_OUTS2_SPI,
            .output_ch_id = 12,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT2_PIN13
        {
            .if_id = ECU_OUT_IF_OUTS2_SPI,
            .output_ch_id = 13,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT2_PIN14
        {
            .if_id = ECU_OUT_IF_OUTS2_SPI,
            .output_ch_id = 14,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT2_PIN15
        {
            .if_id = ECU_OUT_IF_OUTS2_SPI,
            .output_ch_id = 15,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_OUT_PORT2_PIN16
        {
            .if_id = ECU_OUT_IF_IGN,
            .output_ch_id = 0,
            .pin = { .port = IGN_CH1_GPIO_Port, .pin = IGN_CH1_Pin },
            .gpio_invert = false,
        }, //ECU_OUT_IGN1
        {
            .if_id = ECU_OUT_IF_IGN,
            .output_ch_id = 1,
            .pin = { .port = IGN_CH2_GPIO_Port, .pin = IGN_CH2_Pin },
            .gpio_invert = false,
        }, //ECU_OUT_IGN2
        {
            .if_id = ECU_OUT_IF_IGN,
            .output_ch_id = 2,
            .pin = { .port = IGN_CH3_GPIO_Port, .pin = IGN_CH3_Pin },
            .gpio_invert = false,
        }, //ECU_OUT_IGN3
        {
            .if_id = ECU_OUT_IF_IGN,
            .output_ch_id = 3,
            .pin = { .port = IGN_CH4_GPIO_Port, .pin = IGN_CH4_Pin },
            .gpio_invert = false,
        }, //ECU_OUT_IGN4
        {
            .if_id = ECU_OUT_IF_IGN,
            .output_ch_id = 4,
            .pin = { .port = IGN_CH5_GPIO_Port, .pin = IGN_CH5_Pin },
            .gpio_invert = false,
        }, //ECU_OUT_IGN5
        {
            .if_id = ECU_OUT_IF_IGN,
            .output_ch_id = 5,
            .pin = { .port = IGN_CH6_GPIO_Port, .pin = IGN_CH6_Pin },
            .gpio_invert = false,
        }, //ECU_OUT_IGN6
        {
            .if_id = ECU_OUT_IF_IGN,
            .output_ch_id = 6,
            .pin = { .port = IGN_CH7_GPIO_Port, .pin = IGN_CH7_Pin },
            .gpio_invert = false,
        }, //ECU_OUT_IGN7
        {
            .if_id = ECU_OUT_IF_IGN,
            .output_ch_id = 7,
            .pin = { .port = IGN_CH8_GPIO_Port, .pin = IGN_CH8_Pin },
            .gpio_invert = false,
        }, //ECU_OUT_IGN8
    },
    .inputs = {
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 0,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN1
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 1,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN2
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 2,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN3
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 3,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN4
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 4,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN5
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 5,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN6
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 6,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN7
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 7,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN8
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 8,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN9
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 9,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN10
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 10,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN11
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 11,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN12
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 12,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN13
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 13,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN14
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 14,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT1_PIN15
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 15,
            .pin = { .port = FLEXIO1_VRSO_GPIO_Port, .pin = FLEXIO1_VRSO_Pin },
            .tim_alternate = GPIO_AF2_TIM12,
            .htim = &htim12,
            .tim_channel_rising_falling = TIM_CHANNEL_1,
        }, //ECU_IN_PORT1_VRS
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 0,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN1
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 1,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN2
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 2,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN3
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 3,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN4
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 9,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN10
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 10,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN11
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 11,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN12
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 12,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN13
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 13,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN14
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 14,
            .pin = { .port = NULL, .pin = 0 },
        }, //ECU_IN_PORT2_PIN15
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 15,
            .pin = { .port = FLEXIO2_VRSO_GPIO_Port, .pin = FLEXIO2_VRSO_Pin },
            .tim_alternate = GPIO_AF2_TIM12,
            .htim = &htim12,
            .tim_channel_rising_falling = TIM_CHANNEL_2,
        }, //ECU_IN_PORT2_VRS
        {
            .if_id = ECU_IN_IF_ADC1,
            .input_ch_id = 0,
        }, //ECU_IN_ADC1_IN1
        {
            .if_id = ECU_IN_IF_ADC1,
            .input_ch_id = 1,
        }, //ECU_IN_ADC1_IN1
    },
};


ITCM_FUNC error_t ecu_config_gpio_output_pwm_set_value_internal(ecu_config_gpio_output_t *output, uint32_t value)
{
  error_t err = E_OK;
  TIM_HandleTypeDef *htim;
  volatile uint32_t *reg = NULL;
  ecu_config_gpio_outputs_if_t *output_if;
  float fvalue;

  do {
    output_if = &ecu_config_gpio.outputs_if[output->if_id];
    htim = output_if->htim;

    switch(output->tim_channel) {
      case TIM_CHANNEL_1:
        reg = &htim->Instance->CCR1;
        break;
      case TIM_CHANNEL_2:
        reg = &htim->Instance->CCR2;
        break;
      case TIM_CHANNEL_3:
        reg = &htim->Instance->CCR3;
        break;
      case TIM_CHANNEL_4:
        reg = &htim->Instance->CCR4;
        break;
      case TIM_CHANNEL_5:
        reg = &htim->Instance->CCR5;
        break;
      case TIM_CHANNEL_6:
        reg = &htim->Instance->CCR6;
        break;
      default:
        break;
    }

    if(reg == NULL) {
      err = E_FAULT;
      break;
    }

    fvalue = (float)value * output_if->pwm_cfg.period_multiplier;
    value = CLAMP(fvalue, 0, output_if->pwm_cfg.pwm_period);
    *reg = value;

  } while(0);

  return err;
}

static error_t ecu_config_gpio_spi_ch_set(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata)
{
  error_t err = E_NOTSUPPORT;



  return err;
}

static ITCM_FUNC error_t ecu_config_gpio_ch_set(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata)
{
  error_t err = E_OK;
  ecu_config_gpio_output_t *channel = ecu_config_gpio.outputs_if[interface_id].channels[channel_id];

  if(channel->type == ECU_GPIO_TYPE_DIRECT) {
    gpio_write(&channel->pin, value > 0 ? channel->gpio_invert : !channel->gpio_invert);
  } else if(channel->type == ECU_GPIO_TYPE_PWM) {
    err = ecu_config_gpio_output_pwm_set_value_internal(channel, value);
  }

  return err;
}

error_t ecu_config_gpio_output_init(void)
{
  error_t err = E_OK;
  bool valid = false;

  do {
    for(int i = 0; i < ITEMSOF(ecu_config_gpio.outputs_if); i++) {
      ecu_config_gpio.outputs_if[i].output_if_id = output_if_register(&ecu_config_gpio.outputs_if[i].cfg, ecu_config_gpio.outputs_if[i].usrdata);
      if(ecu_config_gpio.outputs_if[i].output_if_id < 0) {
        err = E_FAULT;
        break;
      }
    }

    if(err != E_OK) {
      break;
    }

    for(int i = 0; i < ITEMSOF(ecu_config_gpio.outputs); i++) {
      ecu_config_gpio.outputs[i].output_id = output_ch_register(ecu_config_gpio.outputs_if[ecu_config_gpio.outputs[i].if_id].output_if_id, ecu_config_gpio.outputs[i].output_ch_id, ecu_config_gpio.outputs[i].usrdata);
      if(ecu_config_gpio.outputs[i].output_id < 0) {
        err = E_FAULT;
        break;
      }

      ecu_config_gpio.outputs_if[ecu_config_gpio.outputs[i].if_id].channels[ecu_config_gpio.outputs[i].output_ch_id] = &ecu_config_gpio.outputs[i];

      valid = gpio_valid(&ecu_config_gpio.outputs[i].pin);
      if(valid == true) {
        err = output_ch_dest_gpio(ecu_config_gpio.outputs[i].output_id, &ecu_config_gpio.outputs[i].pin, ecu_config_gpio.outputs[i].gpio_invert);
        if(err != E_OK) {
          break;
        }
      }
    }

    if(err != E_OK) {
      break;
    }
  } while(0);

  return err;
}

error_t ecu_config_gpio_input_init(void)
{
  error_t err = E_OK;
  bool valid = false;

  do {
    for(int i = 0; i < ITEMSOF(ecu_config_gpio.inputs_if); i++) {
      ecu_config_gpio.inputs_if[i].input_if_id = input_if_register(&ecu_config_gpio.inputs_if[i].cfg, ecu_config_gpio.inputs_if[i].usrdata);
      if(ecu_config_gpio.inputs_if[i].input_if_id < 0) {
        err = E_FAULT;
        break;
      }
    }

    if(err != E_OK) {
      break;
    }

    for(int i = 0; i < ITEMSOF(ecu_config_gpio.inputs); i++) {
      ecu_config_gpio.inputs[i].input_id = input_ch_register(ecu_config_gpio.inputs_if[ecu_config_gpio.inputs[i].if_id].input_if_id, ecu_config_gpio.inputs[i].input_ch_id, ecu_config_gpio.inputs[i].usrdata);
      if(ecu_config_gpio.inputs[i].input_id < 0) {
        err = E_FAULT;
        break;
      }

      ecu_config_gpio.inputs_if[ecu_config_gpio.inputs[i].if_id].channels[ecu_config_gpio.inputs[i].input_ch_id] = &ecu_config_gpio.inputs[i];

      valid = gpio_valid(&ecu_config_gpio.inputs[i].pin);
      if(valid == true) {
        err = input_ch_source_gpio(ecu_config_gpio.inputs[i].input_id, &ecu_config_gpio.inputs[i].pin, ecu_config_gpio.inputs[i].gpio_invert);
        if(err != E_OK) {
          break;
        }
      }
    }

    if(err != E_OK) {
      break;
    }
  } while(0);

  return err;
}

error_t ecu_config_gpio_output_set_mode(ecu_gpio_output_pin_t pin, ecu_gpio_output_type_t type)
{
  error_t err = E_OK;
  bool valid = false;

  do {
    if(pin >= ECU_OUT_MAX || type >= ECU_GPIO_TYPE_MAX) {
      err = E_PARAM;
      break;
    }

    if(type == ECU_GPIO_TYPE_DIRECT) {
      valid = gpio_valid(&ecu_config_gpio.outputs[pin].pin);
      if(valid == false) {
        err = E_NOTSUPPORT;
        break;
      }

      gpio_configure_direct_output(&ecu_config_gpio.outputs[pin].pin, ecu_config_gpio.outputs[pin].gpio_invert);

    } else if(type == ECU_GPIO_TYPE_PWM) {
      err = ecu_config_gpio_output_has_pwm_support(pin, &valid);
      if(err != E_OK) {
        break;
      }
      if(valid == false) {
        err = E_NOTSUPPORT;
        break;
      }

      gpio_configure_pwm_output(&ecu_config_gpio.outputs[pin].pin, ecu_config_gpio.outputs[pin].tim_alternate);

      err = ecu_config_gpio_output_pwm_configure(pin, &ecu_config_gpio.outputs_if[ecu_config_gpio.outputs[pin].if_id].pwm_cfg);
      if(err != E_OK) {
        break;
      }
    }

    ecu_config_gpio.outputs[pin].type = type;

  } while(0);

  return err;
}

error_t ecu_config_gpio_output_pwm_configure(ecu_gpio_output_pin_t pin, ecu_config_gpio_output_if_pwm_cfg_t *config)
{
  HAL_StatusTypeDef status;
  error_t err = E_OK;
  bool valid = false;
  TIM_HandleTypeDef *htim;
  ecu_config_gpio_outputs_if_t *if_ctx = NULL;
  uint32_t period, freq, prescaler;
  uint32_t tim_period_max;
  uint32_t tim_prescaler_max;
  float prescaler_flt, period_flt;
  float prescaler_normalize, period_normalize;

  do {
    if(pin >= ECU_OUT_MAX || config == NULL) {
      err = E_PARAM;
      break;
    }

    err = ecu_config_gpio_output_has_pwm_support(pin, &valid);
    if(err != E_OK) {
      break;
    }
    if(valid == false) {
      err = E_NOTSUPPORT;
      break;
    }

    if_ctx = &ecu_config_gpio.outputs_if[ecu_config_gpio.outputs[pin].if_id];
    htim = if_ctx->htim;

    if(htim == NULL) {
      err = E_NOTSUPPORT;
      break;
    }

    if(&if_ctx->pwm_cfg != config) {
      memcpy(&ecu_config_gpio.outputs_if[ecu_config_gpio.outputs[pin].if_id].pwm_cfg, config, sizeof(ecu_config_gpio_output_if_pwm_cfg_t));
      config = &ecu_config_gpio.outputs_if[ecu_config_gpio.outputs[pin].if_id].pwm_cfg;
    }

    err = ecu_config_get_tim_base_frequency(htim, &freq);
    if(err != E_OK) {
      break;
    }

    tim_prescaler_max = USHRT_MAX;
    if(IS_TIM_32B_COUNTER_INSTANCE(htim->Instance)) {
      tim_period_max = UINT_MAX;
    } else {
      tim_period_max = USHRT_MAX;
    }

    prescaler_flt = (float)freq / (float)config->frequency / config->pwm_period;
    period_flt = config->pwm_period;

    prescaler_normalize = ceilf(prescaler_flt / tim_prescaler_max);
    if(prescaler_normalize != 1.0f) {
      prescaler_flt /= prescaler_normalize;
      period_flt *= prescaler_normalize;
    }

    if(period_flt > tim_period_max) {
      period_normalize = tim_period_max / period_flt;
      prescaler_flt /= period_normalize;
      period_flt *= period_normalize;

      config->period_multiplier = prescaler_normalize * period_normalize;
    } else {
      config->period_multiplier = prescaler_normalize;
    }

    if(period_flt > tim_period_max || prescaler_flt > tim_prescaler_max) {
      err = E_PARAM;
      break;
    }

    prescaler = roundf(prescaler_flt);
    period = roundf(period_flt);

    prescaler = CLAMP(prescaler, 1, tim_prescaler_max);
    period = CLAMP(period, 1, tim_period_max);

    htim->Init.Prescaler = prescaler - 1;
    htim->Init.Period = period - 1;

    status = HAL_TIM_Base_Init(htim);
    if(status != HAL_OK) {
      err = E_HAL;
      break;
    }


  } while(0);

  return err;
}

ITCM_FUNC error_t ecu_config_gpio_output_pwm_set_value(ecu_gpio_output_pin_t pin, uint32_t value)
{
  error_t err = E_OK;
  bool valid = false;
  ecu_config_gpio_output_t *output;

  do {
    err = ecu_config_gpio_output_has_pwm_support(pin, &valid);
    if(err != E_OK) {
      break;
    }

    if(valid != true) {
      err = E_NOTSUPPORT;
      break;
    }

    output = &ecu_config_gpio.outputs[pin];

    err = ecu_config_gpio_output_pwm_set_value_internal(output, value);

  } while(0);

  return err;
}

ITCM_FUNC error_t ecu_config_gpio_output_pwm_set_dutycycle(ecu_gpio_output_pin_t pin, float dutycycle)
{
  error_t err = E_OK;
  bool valid = false;
  ecu_config_gpio_output_t *output;
  ecu_config_gpio_outputs_if_t *output_if;
  float fvalue;
  uint32_t value;

  do {
    err = ecu_config_gpio_output_has_pwm_support(pin, &valid);
    if(err != E_OK) {
      break;
    }

    if(valid != true) {
      err = E_NOTSUPPORT;
      break;
    }

    output = &ecu_config_gpio.outputs[pin];
    output_if = &ecu_config_gpio.outputs_if[output->if_id];

    fvalue = dutycycle * output_if->pwm_cfg.pwm_period;
    value = CLAMP(fvalue, 0, output_if->pwm_cfg.pwm_period);

    err = ecu_config_gpio_output_pwm_set_value_internal(output, value);

  } while(0);

  return err;
}

error_t ecu_config_gpio_output_has_pwm_support(ecu_gpio_output_pin_t pin, bool *support)
{
  error_t err = E_OK;
  TIM_HandleTypeDef *htim;

  do {
    if(pin >= ECU_OUT_MAX || support == NULL) {
      err = E_PARAM;
      break;
    }

    htim = ecu_config_gpio.outputs_if[ecu_config_gpio.outputs[pin].if_id].htim;
    if(htim == NULL) {
      *support = false;
      break;
    }

    *support = true;

  } while(0);

  return err;
}

error_t ecu_config_gpio_output_get_pin(ecu_gpio_output_pin_t pin, gpio_t *gpio)
{
  error_t err = E_OK;

  do {
    if(pin >= ECU_OUT_MAX || gpio == NULL) {
      err = E_PARAM;
      break;
    }

    *gpio = ecu_config_gpio.outputs[pin].pin;

  } while(0);

  return err;
}


error_t ecu_config_gpio_input_register_callback(ecu_gpio_input_pin_t pin, ecu_config_gpio_input_cb_t callback)
{
  error_t err = E_OK;

  do {
    if(pin >= ECU_IN_MAX) {
      err = E_PARAM;
      break;
    }

    ecu_config_gpio.inputs[pin].irq_cb = callback;

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_get_pin(ecu_gpio_input_pin_t pin, gpio_t *gpio)
{
  error_t err = E_OK;

  do {
    if(pin >= ECU_IN_MAX || gpio == NULL) {
      err = E_PARAM;
      break;
    }

    *gpio = ecu_config_gpio.inputs[pin].pin;

  } while(0);

  return err;
}
