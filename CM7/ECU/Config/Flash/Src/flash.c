/*
 * flash.c
 *
 *  Created on: May 4, 2024
 *      Author: VHEMaster
 */

#include "flash.h"
#include "flash_memory_layout.h"
#include "compiler.h"

error_t flash_init(void)
{
  error_t err = E_OK;

  do {
    err = flash_mem_layout_init();
    BREAK_IF(err != E_OK);
  } while(0);

  return err;
}
