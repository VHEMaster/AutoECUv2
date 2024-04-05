/*
 * middlelayer_gpio.h
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#ifndef MIDDLELAYER_INC_MIDDLELAYER_GPIO_H_
#define MIDDLELAYER_INC_MIDDLELAYER_GPIO_H_

void middlelayer_gpio_init(void);
void middlelayer_gpio_loop_main(void);
void middlelayer_gpio_loop_slow(void);
void middlelayer_gpio_loop_fast(void);

#endif /* MIDDLELAYER_INC_MIDDLELAYER_GPIO_H_ */
