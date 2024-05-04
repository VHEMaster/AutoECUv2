/*
 * flash_memory_layout.c
 *
 *  Created on: May 3, 2024
 *      Author: VHEMaster
 */

#include "flash.h"
#include "flash_memory_layout.h"
#include "config_global.h"
#include "config_devices.h"
#include "compiler.h"
#include <stddef.h>

static const flash_mem_layout_block_info_t flash_mem_layout_blocks[ECU_FLASH_BLOCKS_COUNT] = {

    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_64K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_64K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_64K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_64K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .sector_size = ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
};

static const flash_mem_layout_section_info_t flash_mem_layout_sections[] = {
    {
        .section_type = FLASH_SECTION_TYPE_ID,
        .section_index = 0,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 0,
        .sector_index = 0,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_FLEXIO,
        .section_index = ECU_DEVICE_FLEXIO_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 0,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_FLEXIO,
        .section_index = ECU_DEVICE_FLEXIO_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 1,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_MOTOR,
        .section_index = ECU_DEVICE_MOTOR_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 2,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_MOTOR,
        .section_index = ECU_DEVICE_MOTOR_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 3,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_STEPPER,
        .section_index = ECU_DEVICE_STEPPER_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 4,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_WBLS,
        .section_index = ECU_DEVICE_WBLS_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 6,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_WBLS,
        .section_index = ECU_DEVICE_WBLS_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 7,
    },
};

static uint16_t flash_mem_layout_section_type_to_section_index[FLASH_SECTION_TYPE_MAX][FLASH_SECTION_INDEX_MAX] = {0};

static const flash_mem_layout_t flash_mem_layout = {
    .mem_size = ECU_FLASH_SIZE * ECU_FLASH_DIES_COUNT,
    .blocks = ITEMSOF(flash_mem_layout_blocks),
    .blocks_info = flash_mem_layout_blocks,
    .sections = ITEMSOF(flash_mem_layout_sections),
    .sections_info = flash_mem_layout_sections,

    .section_type_max = FLASH_SECTION_TYPE_MAX,
    .section_index_max = FLASH_SECTION_INDEX_MAX,
    .section_type_to_section_layout_count = ITEMSOF(flash_mem_layout_section_type_to_section_index),
    .section_type_to_section_layout = (uint16_t *)flash_mem_layout_section_type_to_section_index
};

error_t flash_mem_layout_init(void)
{
  error_t err = E_OK;
  const flash_mem_layout_t *layout = &flash_mem_layout;
  uint16_t index = 0xFFFF;

  do {
    for(flash_section_type_t t = 0; t < layout->section_type_max; t++) {
      for(flash_section_index_t i = 0; i < layout->section_index_max; i++) {
        index = ECU_FLASH_SECTION_TYPE_INDEX_TO_LAYOUT(layout, t, i);
        layout->section_type_to_section_layout[index] = 0xFFFF;
      }
    }

    for(flash_section_type_t t = 0; t < layout->section_type_max; t++) {
      for(flash_section_index_t i = 0; i < layout->section_index_max; i++) {
        index = ECU_FLASH_SECTION_TYPE_INDEX_TO_LAYOUT(layout, t, i);
        for(int s = 0; s < layout->sections; s++) {
          if(layout->sections_info[s].section_type == t &&
              layout->sections_info[s].section_index == i) {
            if(layout->section_type_to_section_layout[index] == 0xFFFF) {
              layout->section_type_to_section_layout[index] = s;
            } else {
              err = E_NOTSUPPORT;
              break;
            }
          }
        }
      }
    }
  } while(0);

  return err;
}

error_t flash_mem_layout_get_section_info(const flash_mem_layout_section_info_t **section_info, uint16_t section_type, uint16_t section_index)
{
  error_t err = E_OK;
  uint16_t index, section;
  const flash_mem_layout_t *layout = &flash_mem_layout;

  do {
    BREAK_IF_ACTION(section_info == NULL || section_type >= FLASH_SECTION_TYPE_MAX || section_index >= FLASH_SECTION_INDEX_MAX, err = E_PARAM);

    index = ECU_FLASH_SECTION_TYPE_INDEX_TO_LAYOUT(layout, section_type, section_index);
    section = layout->section_type_to_section_layout[index];
    BREAK_IF_ACTION(section == 0xFFFF, err = E_NOTSUPPORT);

    *section_info = &layout->sections_info[section];

  } while(0);

  return err;
}

error_t flash_mem_layout_get(const flash_mem_layout_t **layout)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(layout == NULL, err = E_PARAM);

    *layout = &flash_mem_layout;

  } while(0);

  return err;
}
