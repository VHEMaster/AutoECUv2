/*
 * flash_memory_layout.h
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_ECU_FLASH_INC_ECU_FLASH_MEMORY_LAYOUT_H_
#define CONFIG_ECU_FLASH_INC_ECU_FLASH_MEMORY_LAYOUT_H_

#include "errors.h"
#include <stdint.h>
#include "flash.h"

#define ECU_FLASH_DIE_SIZE              0x400000
#define ECU_FLASH_DIE_SECTOR_SIZE       0x1000
#define ECU_FLASH_DIE_PAGE_SIZE         0x100

#define ECU_FLASH_DIE_BLOCK_SIZE_8K     (8 * 1024)
#define ECU_FLASH_DIE_BLOCK_SIZE_32K    (32 * 1024)
#define ECU_FLASH_DIE_BLOCK_SIZE_64K    (64 * 1024)

#define ECU_FLASH_DIES_COUNT            2

#define ECU_FLASH_SIZE                  (ECU_FLASH_DIE_SIZE * ECU_FLASH_DIES_COUNT)
#define ECU_FLASH_SECTOR_SIZE           (ECU_FLASH_DIE_SECTOR_SIZE * ECU_FLASH_DIES_COUNT)
#define ECU_FLASH_PAGE_SIZE             (ECU_FLASH_DIE_PAGE_SIZE * ECU_FLASH_DIES_COUNT)

#define ECU_FLASH_BLOCKS_COUNT          72
#define ECU_FLASH_BLOCK_SIZE_16K        (ECU_FLASH_DIE_BLOCK_SIZE_8K * ECU_FLASH_DIES_COUNT)
#define ECU_FLASH_BLOCK_SIZE_64K        (ECU_FLASH_DIE_BLOCK_SIZE_32K * ECU_FLASH_DIES_COUNT)
#define ECU_FLASH_BLOCK_SIZE_128K       (ECU_FLASH_DIE_BLOCK_SIZE_64K * ECU_FLASH_DIES_COUNT)

#define ECU_FLASH_SECTION_TYPE_INDEX_TO_LAYOUT(layout, type, index) ((type) * ((layout)->section_index_max) + (index))

typedef struct {
    uint32_t sectors_count;
    uint32_t sector_size;
    uint32_t block_size;
}flash_mem_layout_block_info_t;

typedef enum {
  FLASH_LAYOUT_SECTION_USAGE_SECTOR = 0,
  FLASH_LAYOUT_SECTION_USAGE_BLOCK
}flash_mem_layout_section_usage_t;

typedef struct {
    flash_section_type_t section_type;
    flash_section_index_t section_index;
    uint16_t section_length;
    uint16_t block_index;
    uint16_t sector_index;
}flash_mem_layout_section_info_t;

typedef struct {
    uint32_t mem_size;
    uint32_t blocks;
    uint32_t sections;
    const flash_mem_layout_block_info_t *blocks_info;
    const flash_mem_layout_section_info_t *sections_info;

    uint16_t section_type_max;
    uint16_t section_index_max;
    uint32_t section_type_to_section_layout_count;
    uint16_t *section_type_to_section_layout;
}flash_mem_layout_t;

error_t flash_mem_layout_init(void);
error_t flash_mem_layout_get_section_info(const flash_mem_layout_section_info_t **section_info, uint16_t section_type, uint16_t section_index);
error_t flash_mem_layout_get(const flash_mem_layout_t **layout);

#endif /* CONFIG_ECU_FLASH_INC_ECU_FLASH_MEMORY_LAYOUT_H_ */
