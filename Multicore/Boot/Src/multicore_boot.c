/*
 * multicore_boot.c
 *
 *  Created on: 2 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "multicore_boot.h"

void multicore_boot_init(void)
{
#ifdef CORE_CM7
  const multicore_boot_flag_t entered = MULTICORE_BOOT_CM7_ENTERED;
#else
  const multicore_boot_flag_t entered = MULTICORE_BOOT_CM4_ENTERED;
#endif
  __HAL_RCC_HSEM_CLK_ENABLE();

  if(HAL_HSEM_IsSemTaken(entered)) {
    NVIC_SystemReset();
  }

  multicore_boot_publish(entered);
#ifdef CORE_CM4
  multicore_boot_wait(MULTICORE_BOOT_CLOCKS);
#endif
}

void multicore_boot_publish(multicore_boot_flag_t flag)
{
  __DMB();
  HAL_HSEM_FastTake(flag);
}

void multicore_boot_wait(multicore_boot_flag_t flag)
{
  while(!HAL_HSEM_IsSemTaken(flag)) {
    __NOP();
  }
  __DMB();
}

#ifdef CORE_CM7
void multicore_boot_clocks_ready(void)
{
  multicore_boot_publish(MULTICORE_BOOT_CLOCKS);
  HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);
}
#endif
