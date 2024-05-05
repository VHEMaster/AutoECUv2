/*
 * flash.h
 *
 *  Created on: May 4, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_FLASH_INC_FLASH_H_
#define CONFIG_FLASH_INC_FLASH_H_

#include "errors.h"
#include "compiler.h"
#include <stdint.h>

#define ECU_FLASH_SECTION_HEADER_LENGTH     (sizeof(flash_section_header_t))

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

typedef struct {
    uint16_t section_type;
    uint16_t section_index;
    uint16_t version;
    uint16_t pages;
    uint16_t reserved[11];
    uint16_t crc;
}flash_section_header_t ALIGNED(32);

error_t flash_init(void);
void flash_loop_fast(void);

error_t flash_lock(void);
error_t flash_unlock(void);

error_t flash_section_read(uint16_t section_type, uint16_t section_index, void *payload, uint32_t length);
error_t flash_section_write(uint16_t section_type, uint16_t section_index, uint32_t address, const void *payload, uint32_t length);

#endif /* CONFIG_FLASH_INC_FLASH_H_ */
