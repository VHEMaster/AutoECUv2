/*
 * gpio_pin.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef COMMON_INC_GPIO_PIN_H_
#define COMMON_INC_GPIO_PIN_H_

#include "main.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
}gpio_pin_t;

#endif /* COMMON_INC_GPIO_PIN_H_ */
