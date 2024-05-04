/*
 * flash.h
 *
 *  Created on: May 4, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_FLASH_INC_FLASH_H_
#define CONFIG_FLASH_INC_FLASH_H_

#include "errors.h"
#include <stdint.h>

typedef enum {
  FLASH_SECTION_TYPE_EMPTY = 0,

  FLASH_SECTION_TYPE_ID,
  FLASH_SECTION_TYPE_HW_FLEXIO,
  FLASH_SECTION_TYPE_HW_MOTOR,
  FLASH_SECTION_TYPE_HW_STEPPER,
  FLASH_SECTION_TYPE_HW_WBLS,

  FLASH_SECTION_TYPE_MAX,
  FLASH_SECTION_TYPE_INVALID = 0xFFFF
}flash_section_type_t;

typedef enum {
  FLASH_SECTION_INDEX_0 = 0,
  FLASH_SECTION_INDEX_1,
  FLASH_SECTION_INDEX_2,
  FLASH_SECTION_INDEX_3,
  FLASH_SECTION_INDEX_MAX,
  FLASH_SECTION_INDEX_INVALID = 0xFFFF
}flash_section_index_t;

typedef enum {
  FLASH_ERASE_TYPE_NONE = 0,
  FLASH_ERASE_TYPE_SECTOR,
  FLASH_ERASE_TYPE_BLOCK
}flash_erase_type_t;

error_t flash_init(void);
void flash_loop_fast(void);

error_t flash_io_lock(void);
error_t flash_io_unlock(void);

error_t flash_io_read(uint32_t address, void *payload, uint32_t length);
error_t flash_io_write(flash_erase_type_t erase_type, uint32_t address, const void *payload, uint32_t length);


#endif /* CONFIG_FLASH_INC_FLASH_H_ */
