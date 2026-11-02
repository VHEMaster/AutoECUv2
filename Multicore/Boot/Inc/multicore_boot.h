/*
 * multicore_boot.h
 *
 *  Created on: 2 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef BOOT_INC_MULTICORE_BOOT_H_
#define BOOT_INC_MULTICORE_BOOT_H_

#include "stm32h7xx_hal.h"

typedef enum {
  MULTICORE_BOOT_CM7_ENTERED = 0,
  MULTICORE_BOOT_CM4_ENTERED,
  MULTICORE_BOOT_CLOCKS,
  MULTICORE_BOOT_CM7_INIT,
  MULTICORE_BOOT_CM4_STARTED
} multicore_boot_flag_t;

void multicore_boot_init(void);
void multicore_boot_publish(multicore_boot_flag_t flag);
void multicore_boot_wait(multicore_boot_flag_t flag);
#ifdef CORE_CM7
void multicore_boot_clocks_ready(void);
#endif

#endif /* BOOT_INC_MULTICORE_BOOT_H_ */
