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

typedef void (*ecu_gpio_exti_cb_t)(void *usrdata);

#define ECU_EXTI_MAX    (16)

typedef enum {
  ECU_GPIO_TYPE_DIRECT = 0,
  ECU_GPIO_TYPE_PWM,
  ECU_GPIO_TYPE_MAX,
}ecu_gpio_output_type_t;

typedef enum {
  ECU_GPIO_INPUT_TYPE_ANALOG = 1,
  ECU_GPIO_INPUT_TYPE_DIGITAL = 2,
  ECU_GPIO_INPUT_TYPE_CAPTURE = 4,
  ECU_GPIO_INPUT_TYPE_MAX = 8,
}ecu_gpio_input_type_t;

typedef enum {
  ECU_OUT_IF_OUTS1_PWM1 = 0,
  ECU_OUT_IF_OUTS1_DIRECT1,
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
  ECU_IN_IF_TIM,
  ECU_IN_IF_EXTI,
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
  ECU_IN_PORT1_V12V,
  ECU_IN_PORT1_VREF,
  ECU_IN_PORT1_VIGN,
  ECU_IN_PORT2_PIN1,
  ECU_IN_PORT2_PIN2,
  ECU_IN_PORT2_PIN3,
  ECU_IN_PORT2_PIN4,
  ECU_IN_PORT2_PIN5,
  ECU_IN_PORT2_PIN6,
  ECU_IN_PORT2_PIN7,
  ECU_IN_PORT2_PIN8,
  ECU_IN_PORT2_PIN9,
  ECU_IN_PORT2_PIN10,
  ECU_IN_PORT2_PIN11,
  ECU_IN_PORT2_PIN12,
  ECU_IN_PORT2_PIN13,
  ECU_IN_PORT2_PIN14,
  ECU_IN_PORT2_PIN15,
  ECU_IN_PORT2_V12V,
  ECU_IN_PORT2_V5V,
  ECU_IN_PORT2_VIGN,
  ECU_IN_PORT1_VRS,
  ECU_IN_PORT2_VRS,
  ECU_IN_PORT1_SENT1,
  ECU_IN_PORT1_SENT2,
  ECU_IN_PORT1_SENT3,
  ECU_IN_PORT1_SENT4,
  ECU_IN_PORT2_SENT1,
  ECU_IN_PORT2_SENT2,
  ECU_IN_PORT2_SENT3,
  ECU_IN_PORT2_SENT4,
  ECU_IN_MAX,
}ecu_gpio_input_pin_t;

typedef enum {
  ECU_IN_LEVEL_LOW = 0,
  ECU_IN_LEVEL_HIGH,
  ECU_IN_LEVEL_UNDEFINED,
}ecu_gpio_input_level_t;

typedef enum {
  ECU_IN_CAPTURE_EDGE_RISING = 0,
  ECU_IN_CAPTURE_EDGE_FALLING,
  ECU_IN_CAPTURE_EDGE_BOTH,
  ECU_IN_CAPTURE_EDGE_MAX
}ecu_gpio_input_capture_edge_t;

typedef struct {
    float period_multiplier;
    uint32_t pwm_period;
    uint32_t frequency;
}ecu_gpio_output_if_pwm_cfg_t;

typedef void (*ecu_gpio_input_cb_t)(ecu_gpio_input_pin_t pin, ecu_gpio_input_level_t level);

error_t ecu_config_gpio_output_init(void);
error_t ecu_config_gpio_output_set_mode(ecu_gpio_output_pin_t pin, ecu_gpio_output_type_t type);
error_t ecu_config_gpio_output_pwm_configure(ecu_gpio_output_pin_t pin, ecu_gpio_output_if_pwm_cfg_t *config);
error_t ecu_config_gpio_output_pwm_set_value(ecu_gpio_output_pin_t pin, uint32_t value);
error_t ecu_config_gpio_output_pwm_set_dutycycle(ecu_gpio_output_pin_t pin, float dutycycle);
error_t ecu_config_gpio_output_has_pwm_support(ecu_gpio_output_pin_t pin, bool *support);
error_t ecu_config_gpio_output_get_pin(ecu_gpio_output_pin_t pin, gpio_t *gpio);
error_t ecu_config_gpio_output_get_id(ecu_gpio_output_pin_t pin, output_id_t *id);
error_t ecu_config_gpio_output_lock(ecu_gpio_output_pin_t pin);
error_t ecu_config_gpio_output_unlock(ecu_gpio_output_pin_t pin);

error_t ecu_config_gpio_input_init(void);
error_t ecu_config_gpio_input_register_callback(ecu_gpio_input_pin_t pin, ecu_gpio_input_cb_t callback);
error_t ecu_config_gpio_input_get_pin(ecu_gpio_input_pin_t pin, gpio_t *gpio);
error_t ecu_config_gpio_input_set_capture_edge(ecu_gpio_input_pin_t pin, ecu_gpio_input_capture_edge_t capture_edge);
error_t ecu_config_gpio_input_set_mode(ecu_gpio_input_pin_t pin, ecu_gpio_input_type_t mode);
error_t ecu_config_gpio_input_has_mode_support(ecu_gpio_input_pin_t pin, ecu_gpio_input_type_t mode, bool *support);
error_t ecu_config_gpio_input_get_id(ecu_gpio_input_pin_t pin, input_id_t *id);
error_t ecu_config_gpio_input_lock(ecu_gpio_input_pin_t pin);
error_t ecu_config_gpio_input_unlock(ecu_gpio_input_pin_t pin);

error_t ecu_config_gpio_exti_init(void);
error_t ecu_config_gpio_exti_register(uint16_t exti_pin, ecu_gpio_exti_cb_t func, void *usrdata);
void ecu_config_gpio_exti_call(uint16_t exti_pin);

#endif /* CONFIG_INC_CONFIG_GPIO_H_ */
