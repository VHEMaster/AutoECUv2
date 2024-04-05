/*
 * config_gpio.h
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_GPIO_H_
#define CONFIG_INC_CONFIG_GPIO_H_

#include "gpio.h"
#include "outputs.h"
#include "inputs.h"

typedef enum {
  ECU_GPIO_TYPE_DIRECT = 0,
  ECU_GPIO_TYPE_PWM,
  ECU_GPIO_TYPE_MAX,
}ecu_gpio_output_type_t;

typedef enum {
  ECU_OUT_IF_OUTS1_PWM1 = 0,
  ECU_OUT_IF_OUTS1_PWM2,
  ECU_OUT_IF_OUTS1_PWM3,
  ECU_OUT_IF_OUTS1_PWM4,
  ECU_OUT_IF_OUTS1_PWM5,
  ECU_OUT_IF_OUTS1_SPI,
  ECU_OUT_IF_OUTS2_PWM1,
  ECU_OUT_IF_OUTS2_PWM2,
  ECU_OUT_IF_OUTS2_SPI,
  ECU_OUT_IF_IGN,
  ECU_OUT_IF_MAX
}ecu_gpio_output_if_t;

typedef enum {
  ECU_IN_IF_FLEXIO1 = 0,
  ECU_IN_IF_FLEXIO2,
  ECU_IN_IF_ADC1,
  ECU_IN_IF_MAX
}ecu_gpio_input_if_t;

typedef enum {
  ECU_OUT_PORT1_PIN1 = 0,
  ECU_OUT_PORT1_PIN2,
  ECU_OUT_PORT1_PIN3,
  ECU_OUT_PORT1_PIN4,
  ECU_OUT_PORT1_PIN5,
  ECU_OUT_PORT1_PIN6,
  ECU_OUT_PORT1_PIN7,
  ECU_OUT_PORT1_PIN8,
  ECU_OUT_PORT1_PIN9,
  ECU_OUT_PORT1_PIN10,
  ECU_OUT_PORT1_PIN11,
  ECU_OUT_PORT1_PIN12,
  ECU_OUT_PORT1_PIN13,
  ECU_OUT_PORT1_PIN14,
  ECU_OUT_PORT1_PIN15,
  ECU_OUT_PORT1_PIN16,
  ECU_OUT_PORT2_PIN1,
  ECU_OUT_PORT2_PIN2,
  ECU_OUT_PORT2_PIN3,
  ECU_OUT_PORT2_PIN4,
  ECU_OUT_PORT2_PIN5,
  ECU_OUT_PORT2_PIN6,
  ECU_OUT_PORT2_PIN7,
  ECU_OUT_PORT2_PIN8,
  ECU_OUT_PORT2_PIN9,
  ECU_OUT_PORT2_PIN10,
  ECU_OUT_PORT2_PIN11,
  ECU_OUT_PORT2_PIN12,
  ECU_OUT_PORT2_PIN13,
  ECU_OUT_PORT2_PIN14,
  ECU_OUT_PORT2_PIN15,
  ECU_OUT_PORT2_PIN16,
  ECU_OUT_IGN1,
  ECU_OUT_IGN2,
  ECU_OUT_IGN3,
  ECU_OUT_IGN4,
  ECU_OUT_IGN5,
  ECU_OUT_IGN6,
  ECU_OUT_IGN7,
  ECU_OUT_IGN8,
  ECU_OUT_MAX,
}ecu_gpio_output_pin_t;

typedef enum {
  ECU_IN_PORT1_PIN1 = 0,
  ECU_IN_PORT1_PIN2,
  ECU_IN_PORT1_PIN3,
  ECU_IN_PORT1_PIN4,
  ECU_IN_PORT1_PIN5,
  ECU_IN_PORT1_PIN6,
  ECU_IN_PORT1_PIN7,
  ECU_IN_PORT1_PIN8,
  ECU_IN_PORT1_PIN9,
  ECU_IN_PORT1_PIN10,
  ECU_IN_PORT1_PIN11,
  ECU_IN_PORT1_PIN12,
  ECU_IN_PORT1_PIN13,
  ECU_IN_PORT1_PIN14,
  ECU_IN_PORT1_PIN15,
  ECU_IN_PORT1_VRS,
  ECU_IN_PORT2_PIN1,
  ECU_IN_PORT2_PIN2,
  ECU_IN_PORT2_PIN3,
  ECU_IN_PORT2_PIN4,
  ECU_IN_PORT2_PIN10,
  ECU_IN_PORT2_PIN11,
  ECU_IN_PORT2_PIN12,
  ECU_IN_PORT2_PIN13,
  ECU_IN_PORT2_PIN14,
  ECU_IN_PORT2_PIN15,
  ECU_IN_PORT2_VRS,
  ECU_IN_ADC1_IN1,
  ECU_IN_ADC1_IN2,
  ECU_IN_MAX,
}ecu_gpio_input_pin_t;

typedef struct {
    float period_multiplier;
    uint32_t pwm_period;
    uint32_t frequency;
}ecu_config_gpio_output_if_pwm_cfg_t;

typedef void (*ecu_config_gpio_input_cb_t)(ecu_gpio_input_pin_t pin);

error_t ecu_config_gpio_output_init(void);
error_t ecu_config_gpio_output_set_mode(ecu_gpio_output_pin_t pin, ecu_gpio_output_type_t type);
error_t ecu_config_gpio_output_pwm_configure(ecu_gpio_output_pin_t pin, ecu_config_gpio_output_if_pwm_cfg_t *config);
error_t ecu_config_gpio_output_pwm_set_value(ecu_gpio_output_pin_t pin, uint32_t value);
error_t ecu_config_gpio_output_pwm_set_dutycycle(ecu_gpio_output_pin_t pin, float dutycycle);
error_t ecu_config_gpio_output_has_pwm_support(ecu_gpio_output_pin_t pin, bool *support);
error_t ecu_config_gpio_output_get_pin(ecu_gpio_output_pin_t pin, gpio_t *gpio);
error_t ecu_config_gpio_output_get_id(ecu_gpio_output_pin_t pin, output_id_t *id);

error_t ecu_config_gpio_input_init(void);
error_t ecu_config_gpio_input_register_callback(ecu_gpio_input_pin_t pin, ecu_config_gpio_input_cb_t callback);
error_t ecu_config_gpio_input_get_pin(ecu_gpio_input_pin_t pin, gpio_t *gpio);

#endif /* CONFIG_INC_CONFIG_GPIO_H_ */
