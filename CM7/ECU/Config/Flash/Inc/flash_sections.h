/*
 * flash_sections.h
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_FLASH_INC_FLASH_SECTIONS_H_
#define CONFIG_FLASH_INC_FLASH_SECTIONS_H_

#include "errors.h"
#include "compiler.h"
#include <stdint.h>

typedef struct {
    uint16_t type;
    uint16_t version;
    uint16_t index;
    uint16_t length;
    uint32_t crc;
    uint8_t reserved[20];
}flash_section_header_t ALIGNED(32);

typedef struct {
    uint32_t address;
    uint16_t type;
    uint16_t index;
    uint16_t length;
}flash_section_info_t;

error_t flash_section_lock();
error_t flash_section_unlock();
error_t flash_section_find(flash_section_info_t *info);
error_t flash_section_allocate(flash_section_info_t *info);

#endif /* CONFIG_FLASH_INC_FLASH_SECTIONS_H_ */
