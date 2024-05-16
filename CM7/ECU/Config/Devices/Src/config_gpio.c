/*
 * config_gpio.c
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include <limits.h>
#include <math.h>
#include "config_devices.h"
#include "config_gpio.h"
#include "config_extern.h"
#include "config_rcc.h"
#include "compiler.h"
#include "gpio.h"

typedef struct {
    uint16_t exti_pin;
    ecu_gpio_exti_cb_t func;
    void *usrdata;
}ecu_gpio_exti_ctx_t;

typedef struct {
    ecu_gpio_output_pin_t output_pin;
    gpio_t pin;
    ecu_gpio_output_if_t if_id;
    output_ch_id_t output_ch_id;
    output_id_t output_id;
    uint32_t tim_alternate;
    uint32_t tim_channel;
    bool gpio_invert;
    ecu_gpio_output_type_t type;
    void *usrdata;
    bool locked;
}ecu_gpio_output_t;

typedef struct {
    ecu_gpio_input_pin_t input_pin;
    gpio_t pin;
    ecu_gpio_input_if_t if_id;
    input_ch_id_t input_ch_id;
    input_id_t input_id;
    TIM_HandleTypeDef *htim;
    uint32_t tim_alternate;
    uint32_t tim_active_channel_rising_falling;
    uint32_t tim_active_channel_rising;
    uint32_t tim_active_channel_falling;
    bool tim_channel_rising_falling_direct;
    bool tim_channel_rising_direct;
    bool tim_channel_falling_direct;
    uint32_t tim_ic_filter;
    ecu_gpio_input_capture_edge_t current_capture_edge;
    ecu_gpio_input_cb_t irq_cb;
    ecu_gpio_input_type_t supported_modes;
    ecu_gpio_input_type_t current_mode;
    bool exti_support;
    bool gpio_invert;
    void *usrdata;
    bool locked;
}ecu_gpio_input_t;

typedef struct {
    input_if_cfg_t cfg;
    input_if_id_t input_if_id;
    ecu_gpio_input_t *channels[ECU_IN_MAX];
    void *usrdata;
}ecu_gpio_inputs_if_t;

typedef struct {
    output_if_id_t output_if_id;
    output_if_cfg_t cfg;
    ecu_gpio_output_if_pwm_cfg_t pwm_cfg;
    TIM_HandleTypeDef *htim;
    ecu_gpio_output_t *channels[ECU_OUT_MAX];
    void *usrdata;
}ecu_gpio_outputs_if_t;

typedef struct {
    ecu_gpio_outputs_if_t outputs_if[ECU_OUT_IF_MAX];
    ecu_gpio_inputs_if_t inputs_if[ECU_IN_IF_MAX];
    ecu_gpio_output_t outputs[ECU_OUT_MAX];
    ecu_gpio_input_t inputs[ECU_IN_MAX];
    ecu_gpio_exti_ctx_t exti[ECU_EXTI_MAX];
}ecu_config_gpio_t;

static error_t ecu_config_gpio_ch_set(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata);
static error_t ecu_config_gpio_spi_ch_set(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata);
static error_t ecu_config_gpio_flexio_ch_get(input_if_id_t interface_id, input_ch_id_t channel_id, input_value_t *value, void *usrdata);

static ecu_config_gpio_t ecu_gpio_setup = {
    .outputs_if = {
        {
            .htim = &htim1,
            .cfg = { .ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS1_PWM1
        {
            .cfg = { .ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS1_DIRECT1
        {
            .cfg = { .ch_set = ecu_config_gpio_spi_ch_set, },
            .usrdata = NULL,
        }, //ECU_OUT_IF_OUTS1_SPI
        {
            .htim = &htim3,
            .cfg = { .ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS2_PWM1
        {
            .htim = &htim4,
            .cfg = { .ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_OUTS2_PWM2
        {
            .cfg = { .ch_set = ecu_config_gpio_spi_ch_set, },
            .usrdata = NULL,
        }, //ECU_OUT_IF_OUTS2_SPI
        {
            .cfg = { .ch_set = ecu_config_gpio_ch_set, },
        }, //ECU_OUT_IF_IGN
    },
    .inputs_if = {
        {
            .cfg = { .ch_get = ecu_config_gpio_flexio_ch_get, },
            .usrdata = NULL,
        }, //ECU_IN_IF_FLEXIO1
        {
            .cfg = { .ch_get = ecu_config_gpio_flexio_ch_get, },
            .usrdata = NULL,
        }, //ECU_IN_IF_FLEXIO2
    },
    .outputs = {
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM1,
            .output_ch_id = 0,
            .pin = { .port = OUTS1_CH1_GPIO_Port, .pin = OUTS1_CH1_Pin },
            .tim_alternate = GPIO_AF1_TIM1,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = true,
        }, //ECU_OUT_PORT1_PIN1
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM1,
            .output_ch_id = 1,
            .pin = { .port = OUTS1_CH2_GPIO_Port, .pin = OUTS1_CH2_Pin },
            .tim_alternate = GPIO_AF1_TIM1,
            .tim_channel = TIM_CHANNEL_2,
            .gpio_invert = true,
        }, //ECU_OUT_PORT1_PIN2
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM1,
            .output_ch_id = 2,
            .pin = { .port = OUTS1_CH3_GPIO_Port, .pin = OUTS1_CH3_Pin },
            .tim_alternate = GPIO_AF1_TIM1,
            .tim_channel = TIM_CHANNEL_3,
            .gpio_invert = true,
        }, //ECU_OUT_PORT1_PIN3
        {
            .if_id = ECU_OUT_IF_OUTS1_PWM1,
            .output_ch_id = 3,
            .pin = { .port = OUTS1_CH4_GPIO_Port, .pin = OUTS1_CH4_Pin },
            .tim_alternate = GPIO_AF1_TIM1,
            .tim_channel = TIM_CHANNEL_4,
            .gpio_invert = true,
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
            .if_id = ECU_OUT_IF_OUTS1_DIRECT1,
            .output_ch_id = 8,
            .pin = { .port = OUTS1_IN9_GPIO_Port, .pin = OUTS1_IN9_Pin },
            .gpio_invert = true,
        }, //ECU_OUT_PORT1_PIN9
        {
            .if_id = ECU_OUT_IF_OUTS1_DIRECT1,
            .output_ch_id = 9,
            .pin = { .port = OUTS1_IN10_GPIO_Port, .pin = OUTS1_IN10_Pin },
            .gpio_invert = true,
        }, //ECU_OUT_PORT1_PIN10
        {
            .if_id = ECU_OUT_IF_OUTS1_DIRECT1,
            .output_ch_id = 10,
            .pin = { .port = OUTS1_IN11_GPIO_Port, .pin = OUTS1_IN11_Pin },
            .gpio_invert = true,
        }, //ECU_OUT_PORT1_PIN11
        {
            .if_id = ECU_OUT_IF_OUTS1_DIRECT1,
            .output_ch_id = 11,
            .pin = { .port = OUTS1_IN12_GPIO_Port, .pin = OUTS1_IN12_Pin },
            .gpio_invert = true,
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
            .if_id = ECU_OUT_IF_OUTS2_PWM2,
            .output_ch_id = 0,
            .pin = { .port = OUTS2_CH1_GPIO_Port, .pin = OUTS2_CH1_Pin },
            .tim_alternate = GPIO_AF2_TIM3,
            .tim_channel = TIM_CHANNEL_1,
            .gpio_invert = true,
        }, //ECU_OUT_PORT2_PIN1
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM1,
            .output_ch_id = 1,
            .pin = { .port = OUTS2_CH2_GPIO_Port, .pin = OUTS2_CH2_Pin },
            .tim_alternate = GPIO_AF2_TIM3,
            .tim_channel = TIM_CHANNEL_2,
            .gpio_invert = true,
        }, //ECU_OUT_PORT2_PIN2
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM1,
            .output_ch_id = 2,
            .pin = { .port = OUTS2_CH3_GPIO_Port, .pin = OUTS2_CH3_Pin },
            .tim_alternate = GPIO_AF2_TIM3,
            .tim_channel = TIM_CHANNEL_3,
            .gpio_invert = true,
        }, //ECU_OUT_PORT2_PIN3
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM1,
            .output_ch_id = 3,
            .pin = { .port = OUTS2_CH4_GPIO_Port, .pin = OUTS2_CH4_Pin },
            .tim_alternate = GPIO_AF2_TIM3,
            .tim_channel = TIM_CHANNEL_4,
            .gpio_invert = true,
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
            .gpio_invert = true,
        }, //ECU_OUT_PORT2_PIN9
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM2,
            .output_ch_id = 9,
            .pin = { .port = OUTS2_CH10_GPIO_Port, .pin = OUTS2_CH10_Pin },
            .tim_alternate = GPIO_AF2_TIM4,
            .tim_channel = TIM_CHANNEL_2,
            .gpio_invert = true,
        }, //ECU_OUT_PORT2_PIN10
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM2,
            .output_ch_id = 10,
            .pin = { .port = OUTS2_CH11_GPIO_Port, .pin = OUTS2_CH11_Pin },
            .tim_alternate = GPIO_AF2_TIM4,
            .tim_channel = TIM_CHANNEL_3,
            .gpio_invert = true,
        }, //ECU_OUT_PORT2_PIN11
        {
            .if_id = ECU_OUT_IF_OUTS2_PWM2,
            .output_ch_id = 11,
            .pin = { .port = OUTS2_CH12_GPIO_Port, .pin = OUTS2_CH12_Pin },
            .tim_alternate = GPIO_AF2_TIM4,
            .tim_channel = TIM_CHANNEL_4,
            .gpio_invert = true,
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
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN1
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 1,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN2
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 2,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN3
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 3,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN4
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 4,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN5
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 5,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN6
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 6,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN7
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 7,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN8
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 8,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN9
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 9,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN10
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 10,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN11
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 11,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN12
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 12,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN13
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 13,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN14
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 14,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT1_PIN15
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 12,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG,
        }, //ECU_IN_PORT1_V12V
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 13,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG,
        }, //ECU_IN_PORT1_VREF
        {
            .if_id = ECU_IN_IF_FLEXIO1,
            .input_ch_id = 14,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG,
        }, //ECU_IN_PORT1_VIGN
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 0,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN1
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 1,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN2
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 2,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN3
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 3,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN4
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 4,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN5
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 5,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN6
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 6,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN7
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 7,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN8
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 8,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN9
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 9,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,

        }, //ECU_IN_PORT2_PIN10
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 10,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN11
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 11,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG | ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN12
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 12,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN13
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 13,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN14
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 14,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_PIN15
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 12,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG,
        }, //ECU_IN_PORT2_V12V
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 13,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG,
        }, //ECU_IN_PORT2_VREF
        {
            .if_id = ECU_IN_IF_FLEXIO2,
            .input_ch_id = 14,
            .pin = { .port = NULL, .pin = 0 },
            .supported_modes = ECU_GPIO_INPUT_TYPE_ANALOG,
        }, //ECU_IN_PORT2_VIGN
        {
            .if_id = ECU_IN_IF_TIM,
            .input_ch_id = 0,
            .pin = { .port = FLEXIO1_VRSO_GPIO_Port, .pin = FLEXIO1_VRSO_Pin },
            .tim_alternate = GPIO_AF2_TIM12,
            .htim = &htim12,
            .tim_active_channel_rising_falling = HAL_TIM_ACTIVE_CHANNEL_1,
            .tim_channel_rising_falling_direct = true,
            .tim_ic_filter = 15,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL | ECU_GPIO_INPUT_TYPE_CAPTURE,
        }, //ECU_IN_PORT1_VRS
        {
            .if_id = ECU_IN_IF_TIM,
            .input_ch_id = 1,
            .pin = { .port = FLEXIO2_VRSO_GPIO_Port, .pin = FLEXIO2_VRSO_Pin },
            .tim_alternate = GPIO_AF2_TIM12,
            .htim = &htim12,
            .tim_active_channel_rising_falling = HAL_TIM_ACTIVE_CHANNEL_2,
            .tim_channel_rising_falling_direct = true,
            .tim_ic_filter = 15,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL | ECU_GPIO_INPUT_TYPE_CAPTURE,
        }, //ECU_IN_PORT2_VRS
        {
            .if_id = ECU_IN_IF_TIM,
            .input_ch_id = 2,
            .pin = { .port = FLEXIO1_SENT1_GPIO_Port, .pin = FLEXIO1_SENT1_Pin },
            .tim_alternate = GPIO_AF1_TIM2,
            .htim = &htim2,
            .tim_active_channel_rising = HAL_TIM_ACTIVE_CHANNEL_1,
            .tim_channel_rising_direct = true,
            .tim_active_channel_falling = HAL_TIM_ACTIVE_CHANNEL_2,
            .tim_channel_falling_direct = false,
            .tim_ic_filter = 15,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL | ECU_GPIO_INPUT_TYPE_CAPTURE,
        }, //ECU_IN_PORT1_SENT1
        {
            .if_id = ECU_IN_IF_TIM,
            .input_ch_id = 3,
            .pin = { .port = FLEXIO1_SENT2_GPIO_Port, .pin = FLEXIO1_SENT2_Pin },
            .tim_alternate = GPIO_AF2_TIM5,
            .htim = &htim5,
            .tim_active_channel_rising = HAL_TIM_ACTIVE_CHANNEL_1,
            .tim_channel_rising_direct = false,
            .tim_active_channel_falling = HAL_TIM_ACTIVE_CHANNEL_2,
            .tim_channel_falling_direct = true,
            .tim_ic_filter = 15,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL | ECU_GPIO_INPUT_TYPE_CAPTURE,
        }, //ECU_IN_PORT1_SENT2
        {
            .if_id = ECU_IN_IF_TIM,
            .input_ch_id = 4,
            .pin = { .port = FLEXIO1_SENT3_GPIO_Port, .pin = FLEXIO1_SENT3_Pin },
            .tim_alternate = GPIO_AF1_TIM2,
            .htim = &htim2,
            .tim_active_channel_rising = HAL_TIM_ACTIVE_CHANNEL_3,
            .tim_channel_rising_direct = true,
            .tim_active_channel_falling = HAL_TIM_ACTIVE_CHANNEL_4,
            .tim_channel_falling_direct = false,
            .tim_ic_filter = 15,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL | ECU_GPIO_INPUT_TYPE_CAPTURE,
        }, //ECU_IN_PORT1_SENT3
        {
            .if_id = ECU_IN_IF_TIM,
            .input_ch_id = 5,
            .pin = { .port = FLEXIO1_SENT4_GPIO_Port, .pin = FLEXIO1_SENT4_Pin },
            .tim_alternate = GPIO_AF2_TIM5,
            .htim = &htim5,
            .tim_active_channel_rising = HAL_TIM_ACTIVE_CHANNEL_3,
            .tim_channel_rising_direct = false,
            .tim_active_channel_falling = HAL_TIM_ACTIVE_CHANNEL_4,
            .tim_channel_falling_direct = true,
            .tim_ic_filter = 15,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL | ECU_GPIO_INPUT_TYPE_CAPTURE,
        }, //ECU_IN_PORT1_SENT4
        {
            .if_id = ECU_IN_IF_EXTI,
            .input_ch_id = 0,
            .pin = { .port = FLEXIO2_SENT1_GPIO_Port, .pin = FLEXIO2_SENT1_Pin },
            .exti_support = true,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_SENT1
        {
            .if_id = ECU_IN_IF_EXTI,
            .input_ch_id = 1,
            .pin = { .port = FLEXIO2_SENT2_GPIO_Port, .pin = FLEXIO2_SENT2_Pin },
            .exti_support = true,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_SENT2
        {
            .if_id = ECU_IN_IF_EXTI,
            .input_ch_id = 2,
            .pin = { .port = FLEXIO2_SENT3_GPIO_Port, .pin = FLEXIO2_SENT3_Pin },
            .exti_support = true,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_SENT3
        {
            .if_id = ECU_IN_IF_EXTI,
            .input_ch_id = 3,
            .pin = { .port = FLEXIO2_SENT4_GPIO_Port, .pin = FLEXIO2_SENT4_Pin },
            .exti_support = true,
            .current_capture_edge = ECU_IN_CAPTURE_EDGE_BOTH,
            .supported_modes = ECU_GPIO_INPUT_TYPE_DIGITAL,
        }, //ECU_IN_PORT2_SENT4
    },
};


ITCM_FUNC error_t ecu_config_gpio_output_pwm_set_value_internal(ecu_gpio_output_t *output, uint32_t value)
{
  error_t err = E_OK;
  TIM_HandleTypeDef *htim;
  volatile uint32_t *reg = NULL;
  ecu_gpio_outputs_if_t *output_if;
  float fvalue;

  do {
    output_if = &ecu_gpio_setup.outputs_if[output->if_id];
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
  error_t err = E_OK;
  ecu_gpio_output_t *channel = NULL;
  ecu_gpio_outputs_if_t *interface = NULL;
  tle6240_ctx_t *ctx = NULL;

  do {
    BREAK_IF_ACTION(interface_id >= ITEMSOF(ecu_gpio_setup.outputs_if), err = E_PARAM);
    BREAK_IF_ACTION(channel_id >= ITEMSOF(ecu_gpio_setup.outputs_if[interface_id].channels), err = E_PARAM);

    interface = &ecu_gpio_setup.outputs_if[interface_id];
    channel = ecu_gpio_setup.outputs_if[interface_id].channels[channel_id];
    if(interface->usrdata == NULL) {
      switch(channel->if_id) {
        case ECU_OUT_IF_OUTS1_SPI:
          err = ecu_devices_get_output_ctx(ECU_DEVICE_OUTPUT_1, &ctx);
          break;
        case ECU_OUT_IF_OUTS2_SPI:
          err = ecu_devices_get_output_ctx(ECU_DEVICE_OUTPUT_2, &ctx);
          break;
        default:
          err = E_NOTSUPPORT;
          break;
      }
      BREAK_IF(err != E_OK);
      interface->usrdata = ctx;
    }

    ctx = (tle6240_ctx_t *)interface->usrdata;
    BREAK_IF_ACTION(ctx == NULL, err = E_FAULT);

    err = tle6240_ch_write(interface->usrdata, channel->output_ch_id, value > 0 ? true : false);

  } while(0);

  return err;
}

static error_t ecu_config_gpio_flexio_ch_get(input_if_id_t interface_id, input_ch_id_t channel_id, input_value_t *value, void *usrdata)
{
  error_t err = E_OK;
  ecu_gpio_input_t *channel = NULL;
  l9966_ctx_t *ctx = NULL;
  float l9966_output_value = 0;
  l9966_ctrl_dig_inputs_t l9966_digital_value = 0;

  do {
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);
    BREAK_IF_ACTION(interface_id >= ITEMSOF(ecu_gpio_setup.inputs_if), err = E_PARAM);
    BREAK_IF_ACTION(channel_id >= ITEMSOF(ecu_gpio_setup.inputs_if[interface_id].channels), err = E_PARAM);

    channel = ecu_gpio_setup.inputs_if[interface_id].channels[channel_id];
    switch(channel->if_id) {
      case ECU_IN_IF_FLEXIO1:
        err = ecu_devices_get_flexio_ctx(ECU_DEVICE_FLEXIO_1, &ctx);
        break;
      case ECU_IN_IF_FLEXIO2:
        err = ecu_devices_get_flexio_ctx(ECU_DEVICE_FLEXIO_2, &ctx);
        break;
      default:
        err = E_NOTSUPPORT;
        break;
    }

    BREAK_IF(err != E_OK);
    BREAK_IF_ACTION(ctx == NULL, err = E_FAULT);

    switch(ecu_gpio_setup.inputs_if[interface_id].channels[channel_id]->current_mode) {
      case ECU_GPIO_INPUT_TYPE_ANALOG:
        err = l9966_get_sqncr_output(ctx, channel->input_ch_id, &l9966_output_value);
        *value = roundf(l9966_output_value * INPUTS_ANALOG_MULTIPLIER);
        break;
      case ECU_GPIO_INPUT_TYPE_DIGITAL:
        err = l9966_get_inputs(ctx, &l9966_digital_value);
        *value = (l9966_digital_value >> channel->input_ch_id) & 1;
        break;
      default:
        err = E_NOTSUPPORT;
        break;
    }

  } while(0);

  return err;
}

static ITCM_FUNC error_t ecu_config_gpio_ch_set(output_if_id_t interface_id, output_ch_id_t channel_id, output_value_t value, void *usrdata)
{
  error_t err = E_OK;
  ecu_gpio_output_t *channel = ecu_gpio_setup.outputs_if[interface_id].channels[channel_id];

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
    for(int i = 0; i < ITEMSOF(ecu_gpio_setup.outputs_if); i++) {
      ecu_gpio_setup.outputs_if[i].output_if_id = output_if_register(&ecu_gpio_setup.outputs_if[i].cfg, ecu_gpio_setup.outputs_if[i].usrdata);
      if(ecu_gpio_setup.outputs_if[i].output_if_id < 0) {
        err = E_FAULT;
        break;
      }
    }

    if(err != E_OK) {
      break;
    }

    for(int i = 0; i < ITEMSOF(ecu_gpio_setup.outputs); i++) {
      ecu_gpio_setup.outputs[i].locked = true;
      do {
        ecu_gpio_setup.outputs[i].output_pin = i;
        ecu_gpio_setup.outputs[i].output_id = output_ch_register(ecu_gpio_setup.outputs_if[ecu_gpio_setup.outputs[i].if_id].output_if_id, ecu_gpio_setup.outputs[i].output_ch_id, ecu_gpio_setup.outputs[i].usrdata);
        if(ecu_gpio_setup.outputs[i].output_id < 0) {
          err = E_FAULT;
          break;
        }

        ecu_gpio_setup.outputs_if[ecu_gpio_setup.outputs[i].if_id].channels[ecu_gpio_setup.outputs[i].output_ch_id] = &ecu_gpio_setup.outputs[i];


        valid = gpio_valid(&ecu_gpio_setup.outputs[i].pin);
        if(valid == true) {
          err = ecu_config_gpio_output_set_mode(i, ECU_GPIO_TYPE_DIRECT);
          if(err != E_OK) {
            break;
          }
        }
      } while(0);
      ecu_gpio_setup.outputs[i].locked = false;
      BREAK_IF(err != E_OK);
    }

    if(err != E_OK) {
      break;
    }
  } while(0);

  return err;
}

static void ecu_config_gpio_input_capture_cb(TIM_HandleTypeDef *htim)
{
  ecu_gpio_input_t *channel;
  ecu_gpio_input_level_t level = ECU_IN_LEVEL_UNDEFINED;
  uint32_t active_channel, usrdata_index;

  do {
    BREAK_IF(htim == NULL);

    active_channel = htim->Channel;
    usrdata_index = POSITION_VAL(active_channel);
    BREAK_IF(usrdata_index >= ITEMSOF(htim->usrdata));

    channel = htim->usrdata[usrdata_index];
    BREAK_IF(channel == NULL);
    BREAK_IF(channel->htim != htim);
    BREAK_IF(channel->irq_cb == NULL);

    if(channel->tim_active_channel_rising_falling == active_channel) {
      if(channel->current_capture_edge == ECU_IN_CAPTURE_EDGE_RISING) {
        level = ECU_IN_LEVEL_HIGH;
      } else if(channel->current_capture_edge == ECU_IN_CAPTURE_EDGE_FALLING) {
        level = ECU_IN_LEVEL_LOW;
      } else {
        level = ECU_IN_LEVEL_UNDEFINED;
      }
    } else if(channel->tim_active_channel_rising == active_channel) {
      level = ECU_IN_LEVEL_HIGH;
    } else if(channel->tim_active_channel_falling == active_channel) {
      level = ECU_IN_LEVEL_LOW;
    }
    channel->irq_cb(channel->input_pin, level, channel->usrdata);

  } while(0);
}

static void ecu_config_gpio_input_exti_cb(void *usrdata)
{
  ecu_gpio_input_t *input = (ecu_gpio_input_t *)usrdata;
  ecu_gpio_input_level_t level = ECU_IN_LEVEL_UNDEFINED;

  if(input != NULL) {
    if(input->irq_cb != NULL) {
      if(input->current_capture_edge == ECU_IN_CAPTURE_EDGE_RISING) {
        level = ECU_IN_LEVEL_HIGH;
      } else if(input->current_capture_edge == ECU_IN_CAPTURE_EDGE_FALLING) {
        level = ECU_IN_LEVEL_LOW;
      } else {
        level = ECU_IN_LEVEL_UNDEFINED;
      }
      input->irq_cb(input->input_pin, level, input->usrdata);
    }
  }
}

static error_t ecu_gpio_input_tim_ac_to_ch(uint32_t active_channel, uint32_t *tim_channel)
{
  error_t err = E_OK;

  switch(active_channel) {
    case HAL_TIM_ACTIVE_CHANNEL_1:
      *tim_channel = TIM_CHANNEL_1;
      break;
    case HAL_TIM_ACTIVE_CHANNEL_2:
      *tim_channel = TIM_CHANNEL_2;
      break;
    case HAL_TIM_ACTIVE_CHANNEL_3:
      *tim_channel = TIM_CHANNEL_3;
      break;
    case HAL_TIM_ACTIVE_CHANNEL_4:
      *tim_channel = TIM_CHANNEL_4;
      break;
    case HAL_TIM_ACTIVE_CHANNEL_5:
      *tim_channel = TIM_CHANNEL_5;
      break;
    case HAL_TIM_ACTIVE_CHANNEL_6:
      *tim_channel = TIM_CHANNEL_6;
      break;
    default:
      err = E_PARAM;
      break;
  }

  return err;
}

error_t ecu_config_gpio_input_init(void)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  bool valid = false;
  uint32_t tim_channel;

  do {
    for(int i = 0; i < ITEMSOF(ecu_gpio_setup.inputs_if); i++) {
      ecu_gpio_setup.inputs_if[i].input_if_id = input_if_register(&ecu_gpio_setup.inputs_if[i].cfg, ecu_gpio_setup.inputs_if[i].usrdata);
      if(ecu_gpio_setup.inputs_if[i].input_if_id < 0) {
        err = E_FAULT;
        break;
      }
    }

    BREAK_IF(err != E_OK);

    for(int i = 0; i < ITEMSOF(ecu_gpio_setup.inputs); i++) {
      ecu_gpio_setup.inputs[i].locked = true;
      do {
        ecu_gpio_setup.inputs[i].input_pin = i;
        ecu_gpio_setup.inputs[i].input_id = input_ch_register(ecu_gpio_setup.inputs_if[ecu_gpio_setup.inputs[i].if_id].input_if_id, ecu_gpio_setup.inputs[i].input_ch_id, ecu_gpio_setup.inputs[i].usrdata);
        if(ecu_gpio_setup.inputs[i].input_id < 0) {
          err = E_FAULT;
          break;
        }

        ecu_gpio_setup.inputs_if[ecu_gpio_setup.inputs[i].if_id].channels[ecu_gpio_setup.inputs[i].input_ch_id] = &ecu_gpio_setup.inputs[i];

        valid = gpio_valid(&ecu_gpio_setup.inputs[i].pin);
        if(valid == true) {
          err = ecu_config_gpio_input_has_mode_support(i, ECU_GPIO_INPUT_TYPE_CAPTURE, &valid);
          BREAK_IF(err != E_OK);

          if(valid == true) {
            BREAK_IF_ACTION(ecu_gpio_setup.inputs[i].htim == NULL, err = E_NOTSUPPORT);

            err = ecu_config_gpio_input_set_mode(i, ECU_GPIO_INPUT_TYPE_CAPTURE);
            BREAK_IF(err != E_OK);

            err = input_ch_source_gpio(ecu_gpio_setup.inputs[i].input_id, &ecu_gpio_setup.inputs[i].pin, ecu_gpio_setup.inputs[i].gpio_invert);
            BREAK_IF(err != E_OK);

            status = HAL_TIM_RegisterCallback(ecu_gpio_setup.inputs[i].htim, HAL_TIM_IC_CAPTURE_CB_ID, ecu_config_gpio_input_capture_cb);
            BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

            if(ecu_gpio_setup.inputs[i].tim_active_channel_falling != HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
              ecu_gpio_setup.inputs[i].htim->usrdata[POSITION_VAL(ecu_gpio_setup.inputs[i].tim_active_channel_falling)] = &ecu_gpio_setup.inputs[i];

              err = ecu_gpio_input_tim_ac_to_ch(ecu_gpio_setup.inputs[i].tim_active_channel_falling, &tim_channel);
              BREAK_IF(err != E_OK);

              err = ecu_config_gpio_input_set_capture_edge(i, ecu_gpio_setup.inputs[i].current_capture_edge);
              BREAK_IF(err != E_OK);

              status = HAL_TIM_IC_Start_IT(ecu_gpio_setup.inputs[i].htim, tim_channel);
              BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
            }
            if(ecu_gpio_setup.inputs[i].tim_active_channel_rising != HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
              ecu_gpio_setup.inputs[i].htim->usrdata[POSITION_VAL(ecu_gpio_setup.inputs[i].tim_active_channel_rising)] = &ecu_gpio_setup.inputs[i];

              err = ecu_gpio_input_tim_ac_to_ch(ecu_gpio_setup.inputs[i].tim_active_channel_rising, &tim_channel);
              BREAK_IF(err != E_OK);

              err = ecu_config_gpio_input_set_capture_edge(i, ecu_gpio_setup.inputs[i].current_capture_edge);
              BREAK_IF(err != E_OK);

              status = HAL_TIM_IC_Start_IT(ecu_gpio_setup.inputs[i].htim, tim_channel);
              BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
            }
            if(ecu_gpio_setup.inputs[i].tim_active_channel_rising_falling != HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
              ecu_gpio_setup.inputs[i].htim->usrdata[POSITION_VAL(ecu_gpio_setup.inputs[i].tim_active_channel_rising_falling)] = &ecu_gpio_setup.inputs[i];

              err = ecu_gpio_input_tim_ac_to_ch(ecu_gpio_setup.inputs[i].tim_active_channel_rising_falling, &tim_channel);
              BREAK_IF(err != E_OK);

              err = ecu_config_gpio_input_set_capture_edge(i, ecu_gpio_setup.inputs[i].current_capture_edge);
              BREAK_IF(err != E_OK);

              status = HAL_TIM_IC_Start_IT(ecu_gpio_setup.inputs[i].htim, tim_channel);
              BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);
            }

          } else {
            err = ecu_config_gpio_input_has_mode_support(i, ECU_GPIO_INPUT_TYPE_DIGITAL, &valid);
            BREAK_IF(err != E_OK);

            BREAK_IF_ACTION(valid != true, err = E_NOTSUPPORT);

            err = ecu_config_gpio_input_set_mode(i, ECU_GPIO_INPUT_TYPE_DIGITAL);
            BREAK_IF(err != E_OK);

            err = input_ch_source_gpio(ecu_gpio_setup.inputs[i].input_id, &ecu_gpio_setup.inputs[i].pin, ecu_gpio_setup.inputs[i].gpio_invert);
            BREAK_IF(err != E_OK);

            if(ecu_gpio_setup.inputs[i].exti_support == true) {
              err = ecu_config_gpio_input_set_capture_edge(i, ecu_gpio_setup.inputs[i].current_capture_edge);
              BREAK_IF(err != E_OK);

              ecu_config_gpio_exti_register(ecu_gpio_setup.inputs[i].pin.pin, ecu_config_gpio_input_exti_cb, &ecu_gpio_setup.inputs[i]);
            }
          }

        } else if(ecu_gpio_setup.inputs_if[ecu_gpio_setup.inputs[i].if_id].cfg.ch_get != NULL) {

          err = ecu_config_gpio_input_has_mode_support(i, ECU_GPIO_INPUT_TYPE_DIGITAL, &valid);
          BREAK_IF(err != E_OK);

          if(valid == true) {
            err = ecu_config_gpio_input_set_mode(i, ECU_GPIO_INPUT_TYPE_DIGITAL);
            BREAK_IF(err != E_OK);
          } else {
            err = ecu_config_gpio_input_has_mode_support(i, ECU_GPIO_INPUT_TYPE_ANALOG, &valid);
            BREAK_IF(err != E_OK);

            BREAK_IF_ACTION(valid != true, err = E_NOTSUPPORT);

            err = ecu_config_gpio_input_set_mode(i, ECU_GPIO_INPUT_TYPE_ANALOG);
            BREAK_IF(err != E_OK);
          }

          err = input_ch_source_func(ecu_gpio_setup.inputs[i].input_id, ecu_gpio_setup.inputs_if[ecu_gpio_setup.inputs[i].if_id].cfg.ch_get);
          BREAK_IF(err != E_OK);
        }
      } while(0);

      ecu_gpio_setup.inputs[i].locked = false;
      BREAK_IF(err != E_OK);
    }

    BREAK_IF(err != E_OK);
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
      valid = gpio_valid(&ecu_gpio_setup.outputs[pin].pin);
      if(valid == false) {
        err = E_NOTSUPPORT;
        break;
      }

      gpio_configure_direct_output(&ecu_gpio_setup.outputs[pin].pin, ecu_gpio_setup.outputs[pin].gpio_invert);

      err = output_ch_dest_gpio(ecu_gpio_setup.outputs[pin].output_id, &ecu_gpio_setup.outputs[pin].pin, ecu_gpio_setup.outputs[pin].gpio_invert);
      if(err != E_OK) {
        break;
      }

    } else if(type == ECU_GPIO_TYPE_PWM) {
      err = ecu_config_gpio_output_has_pwm_support(pin, &valid);
      if(err != E_OK) {
        break;
      }
      if(valid == false) {
        err = E_NOTSUPPORT;
        break;
      }

      gpio_configure_pwm_output(&ecu_gpio_setup.outputs[pin].pin, ecu_gpio_setup.outputs[pin].tim_alternate);

      err = ecu_config_gpio_output_pwm_configure(pin, &ecu_gpio_setup.outputs_if[ecu_gpio_setup.outputs[pin].if_id].pwm_cfg);
      if(err != E_OK) {
        break;
      }

      err = output_ch_dest_func(ecu_gpio_setup.outputs[pin].output_id, ecu_gpio_setup.outputs_if[ecu_gpio_setup.outputs[pin].if_id].cfg.ch_set);
      if(err != E_OK) {
        break;
      }

    }

    ecu_gpio_setup.outputs[pin].type = type;

  } while(0);

  return err;
}

error_t ecu_config_gpio_output_pwm_configure(ecu_gpio_output_pin_t pin, ecu_gpio_output_if_pwm_cfg_t *config)
{
  HAL_StatusTypeDef status;
  error_t err = E_OK;
  bool valid = false;
  TIM_HandleTypeDef *htim;
  ecu_gpio_outputs_if_t *if_ctx = NULL;
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

    if_ctx = &ecu_gpio_setup.outputs_if[ecu_gpio_setup.outputs[pin].if_id];
    htim = if_ctx->htim;

    if(htim == NULL) {
      err = E_NOTSUPPORT;
      break;
    }

    if(&if_ctx->pwm_cfg != config) {
      memcpy(&ecu_gpio_setup.outputs_if[ecu_gpio_setup.outputs[pin].if_id].pwm_cfg, config, sizeof(ecu_gpio_output_if_pwm_cfg_t));
      config = &ecu_gpio_setup.outputs_if[ecu_gpio_setup.outputs[pin].if_id].pwm_cfg;
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

    prescaler = CLAMP(prescaler, 1, tim_prescaler_max) - 1;
    period = CLAMP(period, 1, tim_period_max) - 1;

    if(htim->Init.Prescaler != prescaler || htim->Init.Period != period) {
      htim->Init.Prescaler = prescaler;
      htim->Init.Period = period;
      status = HAL_TIM_Base_Init(htim);
      if(status != HAL_OK) {
        err = E_HAL;
        break;
      }
    }

    status = HAL_TIM_PWM_Start(htim, ecu_gpio_setup.outputs[pin].tim_channel);
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
  ecu_gpio_output_t *output;

  do {
    err = ecu_config_gpio_output_has_pwm_support(pin, &valid);
    if(err != E_OK) {
      break;
    }

    if(valid != true) {
      err = E_NOTSUPPORT;
      break;
    }

    output = &ecu_gpio_setup.outputs[pin];

    err = ecu_config_gpio_output_pwm_set_value_internal(output, value);

  } while(0);

  return err;
}

ITCM_FUNC error_t ecu_config_gpio_output_pwm_set_dutycycle(ecu_gpio_output_pin_t pin, float dutycycle)
{
  error_t err = E_OK;
  bool valid = false;
  ecu_gpio_output_t *output;
  ecu_gpio_outputs_if_t *output_if;
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

    output = &ecu_gpio_setup.outputs[pin];
    output_if = &ecu_gpio_setup.outputs_if[output->if_id];

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

    htim = ecu_gpio_setup.outputs_if[ecu_gpio_setup.outputs[pin].if_id].htim;
    if(htim == NULL) {
      *support = false;
      break;
    }

    *support = true;

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_set_mode(ecu_gpio_input_pin_t pin, ecu_gpio_input_type_t mode)
{
  error_t err = E_OK;
  bool valid = false;

  do {
    err = ecu_config_gpio_input_has_mode_support(pin, mode, &valid);
    if(err != E_OK) {
      break;
    }

    if(valid != true) {
      err = E_NOTSUPPORT;
      break;
    }

    err = E_PARAM;

    for(uint8_t i = 1; i > 0 && i <= mode; i <<= 1) {
      if(i == mode) {
        ecu_gpio_setup.inputs[pin].current_mode = mode;
        err = E_OK;
        break;
      }
    }

  } while(0);

  return err;
}

static error_t ecu_config_gpio_input_set_capture_edge_channel(ecu_gpio_input_t *input, uint32_t tim_channel, uint32_t tim_edge, bool direct)
{
  error_t err = E_OK;
  HAL_StatusTypeDef status;
  TIM_IC_InitTypeDef config_ic = {0};

  do {
    config_ic.ICPolarity = tim_edge;
    config_ic.ICSelection = direct ? TIM_ICSELECTION_DIRECTTI : TIM_ICSELECTION_INDIRECTTI;
    config_ic.ICPrescaler = TIM_ICPSC_DIV1;
    config_ic.ICFilter = input->tim_ic_filter;

    status = HAL_TIM_IC_ConfigChannel(input->htim, &config_ic, tim_channel);
    BREAK_IF_ACTION(status != HAL_OK, err = E_HAL);

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_set_capture_edge(ecu_gpio_input_pin_t pin, ecu_gpio_input_capture_edge_t capture_edge)
{
  error_t err = E_OK;
  ecu_gpio_input_t *input;
  GPIO_InitTypeDef gpio_init = {0};
  uint32_t tim_channel;

  do {
    input = &ecu_gpio_setup.inputs[pin];

    if(input->current_mode == ECU_GPIO_INPUT_TYPE_CAPTURE && input->htim != NULL) {

      if(capture_edge == ECU_IN_CAPTURE_EDGE_BOTH) {
        if(input->tim_active_channel_rising_falling != HAL_TIM_ACTIVE_CHANNEL_CLEARED &&
            (input->tim_active_channel_rising != HAL_TIM_ACTIVE_CHANNEL_CLEARED ||
                input->tim_active_channel_falling != HAL_TIM_ACTIVE_CHANNEL_CLEARED)) {
          err = E_INVALACT;
          break;
        }

        if(input->tim_active_channel_rising_falling != HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
          err = ecu_gpio_input_tim_ac_to_ch(input->tim_active_channel_rising_falling, &tim_channel);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_set_capture_edge_channel(input, tim_channel, TIM_INPUTCHANNELPOLARITY_BOTHEDGE, input->tim_channel_rising_falling_direct);
          BREAK_IF(err != E_OK);

        } else if(input->tim_active_channel_rising != HAL_TIM_ACTIVE_CHANNEL_CLEARED && input->tim_active_channel_falling != HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
          err = ecu_gpio_input_tim_ac_to_ch(input->tim_active_channel_rising, &tim_channel);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_set_capture_edge_channel(input, tim_channel, TIM_INPUTCHANNELPOLARITY_RISING, input->tim_channel_rising_direct);
          BREAK_IF(err != E_OK);

          err = ecu_gpio_input_tim_ac_to_ch(input->tim_active_channel_falling, &tim_channel);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_set_capture_edge_channel(input, tim_channel, TIM_INPUTCHANNELPOLARITY_FALLING, input->tim_channel_falling_direct);
          BREAK_IF(err != E_OK);

        } else {
          err = E_NOTSUPPORT;
          break;
        }
      } else {
        if(input->tim_active_channel_rising != HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
          err = ecu_gpio_input_tim_ac_to_ch(input->tim_active_channel_rising, &tim_channel);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_set_capture_edge_channel(input, tim_channel, TIM_INPUTCHANNELPOLARITY_RISING, input->tim_channel_rising_direct);
          BREAK_IF(err != E_OK);
        }
        if(input->tim_active_channel_falling != HAL_TIM_ACTIVE_CHANNEL_CLEARED) {
          err = ecu_gpio_input_tim_ac_to_ch(input->tim_active_channel_falling, &tim_channel);
          BREAK_IF(err != E_OK);
          err = ecu_config_gpio_input_set_capture_edge_channel(input, tim_channel, TIM_INPUTCHANNELPOLARITY_FALLING, input->tim_channel_falling_direct);
          BREAK_IF(err != E_OK);
        }
      }

      gpio_init.Pin = input->pin.pin;
      gpio_init.Mode = GPIO_MODE_AF_PP;
      gpio_init.Alternate = input->tim_alternate;
      gpio_init.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(input->pin.port, &gpio_init);

    } else if(input->current_mode == ECU_GPIO_INPUT_TYPE_DIGITAL && input->exti_support == true) {
      switch(capture_edge) {
        case ECU_IN_CAPTURE_EDGE_BOTH:
          gpio_init.Mode = GPIO_MODE_IT_RISING_FALLING;
          break;
        case ECU_IN_CAPTURE_EDGE_RISING:
          gpio_init.Mode = GPIO_MODE_IT_RISING;
          break;
        case ECU_IN_CAPTURE_EDGE_FALLING:
          gpio_init.Mode = GPIO_MODE_IT_FALLING;
          break;
        default:
          err = E_PARAM;
          break;
      }
      BREAK_IF(err != E_OK);

      gpio_init.Pin = input->pin.pin;
      gpio_init.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(input->pin.port, &gpio_init);
    } else {
      err = E_NOTSUPPORT;
    }

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_has_mode_support(ecu_gpio_input_pin_t pin, ecu_gpio_input_type_t mode, bool *support)
{
  error_t err = E_OK;

  do {
    if(pin >= ECU_IN_MAX || support == NULL || mode >= ECU_GPIO_INPUT_TYPE_MAX || mode == 0) {
      err = E_PARAM;
      break;
    }

    if((ecu_gpio_setup.inputs[pin].supported_modes & mode) == mode) {
      *support = true;
    } else {
      *support = false;
    }

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

    *gpio = ecu_gpio_setup.outputs[pin].pin;

  } while(0);

  return err;
}

error_t ecu_config_gpio_output_get_id(ecu_gpio_output_pin_t pin, output_id_t *id)
{
  error_t err = E_OK;

  do {
    if(pin >= ECU_OUT_MAX || id == NULL) {
      err = E_PARAM;
      break;
    }

    *id = ecu_gpio_setup.outputs[pin].output_id;

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_get_id(ecu_gpio_input_pin_t pin, input_id_t *id)
{
  error_t err = E_OK;

  do {
    if(pin >= ECU_IN_MAX || id == NULL) {
      err = E_PARAM;
      break;
    }

    *id = ecu_gpio_setup.inputs[pin].input_id;

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_set_usrdata(ecu_gpio_input_pin_t pin, void *usrdata)
{
  error_t err = E_OK;

  do {
    if(pin >= ECU_IN_MAX) {
      err = E_PARAM;
      break;
    }

    ecu_gpio_setup.inputs[pin].usrdata = usrdata;

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_register_callback(ecu_gpio_input_pin_t pin, ecu_gpio_input_cb_t callback)
{
  error_t err = E_OK;

  do {
    if(pin >= ECU_IN_MAX) {
      err = E_PARAM;
      break;
    }

    ecu_gpio_setup.inputs[pin].irq_cb = callback;

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

    *gpio = ecu_gpio_setup.inputs[pin].pin;

  } while(0);

  return err;
}

error_t ecu_config_gpio_exti_init(void)
{
  error_t err = E_OK;

  do {
    memset(ecu_gpio_setup.exti, 0, sizeof(ecu_gpio_setup.exti));

  } while(0);

  return err;
}

void ecu_config_gpio_exti_call(uint16_t exti_pin)
{
  uint8_t bit = POSITION_VAL(exti_pin);

  if(bit < ECU_EXTI_MAX) {
    if(ecu_gpio_setup.exti[bit].func) {
      ecu_gpio_setup.exti[bit].func(ecu_gpio_setup.exti[bit].usrdata);
    }
  }
}

error_t ecu_config_gpio_exti_register(uint16_t exti_pin, ecu_gpio_exti_cb_t func, void *usrdata)
{
  error_t err = E_OVERFLOW;

  uint8_t bit = POSITION_VAL(exti_pin);

  if(bit < ECU_EXTI_MAX) {
    if(ecu_gpio_setup.exti[bit].func == NULL) {
      ecu_gpio_setup.exti[bit].exti_pin = exti_pin;
      ecu_gpio_setup.exti[bit].usrdata = usrdata;
      ecu_gpio_setup.exti[bit].func = func;
      err = E_OK;
    }
  }

  return err;
}

error_t ecu_config_gpio_output_lock(ecu_gpio_output_pin_t pin)
{
  error_t err = E_OK;
  uint32_t prim;
  ecu_gpio_output_t *pin_ctx;

  do {
    BREAK_IF_ACTION(pin >= ECU_OUT_MAX, err = E_PARAM);

    pin_ctx = &ecu_gpio_setup.outputs[pin];

    prim = EnterCritical();
    if(pin_ctx->locked == true) {
      err = E_BUSY;
    } else {
      pin_ctx->locked = true;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t ecu_config_gpio_output_unlock(ecu_gpio_output_pin_t pin)
{
  error_t err = E_OK;
  uint32_t prim;
  ecu_gpio_output_t *pin_ctx;

  do {
    BREAK_IF_ACTION(pin >= ECU_OUT_MAX, err = E_PARAM);

    pin_ctx = &ecu_gpio_setup.outputs[pin];

    prim = EnterCritical();
    if(pin_ctx->locked == false) {
      err = E_INVALACT;
    } else {
      pin_ctx->locked = false;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_lock(ecu_gpio_input_pin_t pin)
{
  error_t err = E_OK;
  uint32_t prim;
  ecu_gpio_input_t *pin_ctx;

  do {
    BREAK_IF_ACTION(pin >= ECU_IN_MAX, err = E_PARAM);

    pin_ctx = &ecu_gpio_setup.inputs[pin];

    prim = EnterCritical();
    if(pin_ctx->locked == true) {
      err = E_BUSY;
    } else {
      pin_ctx->locked = true;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}

error_t ecu_config_gpio_input_unlock(ecu_gpio_input_pin_t pin)
{
  error_t err = E_OK;
  uint32_t prim;
  ecu_gpio_input_t *pin_ctx;

  do {
    BREAK_IF_ACTION(pin >= ECU_IN_MAX, err = E_PARAM);

    pin_ctx = &ecu_gpio_setup.inputs[pin];

    prim = EnterCritical();
    if(pin_ctx->locked == false) {
      err = E_INVALACT;
    } else {
      pin_ctx->locked = false;
      err = E_OK;
    }
    ExitCritical(prim);

  } while(0);

  return err;
}
