/*
 * compiler.c
 *
 *  Created on: May 22, 2024
 *      Author: VHEMaster
 */

#include "compiler.h"

INLINE uint32_t EnterCritical(void)
{
  uint32_t prim = __get_PRIMASK();

  __disable_irq();

  return prim;
}

INLINE void ExitCritical(uint32_t prim)
{
  if(!prim) {
    __enable_irq();
  }
}
