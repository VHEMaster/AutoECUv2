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
#include "config_sensors.h"
#include "compiler.h"
#include <stddef.h>

static const flash_mem_layout_block_info_t flash_mem_layout_blocks[ECU_FLASH_BLOCKS_COUNT] = {
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_64K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_64K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
/*
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_128K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_128K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_64K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_64K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
    {
        .sectors_count = ECU_FLASH_BLOCK_SIZE_16K / ECU_FLASH_SECTOR_SIZE,
        .block_size = ECU_FLASH_BLOCK_SIZE_16K,
    },
*/
};

static const flash_mem_layout_section_info_t flash_mem_layout_sections[] = {
    {
        .section_type = FLASH_SECTION_TYPE_ID,
        .section_index = 0,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 0,
        .sector_index = 0,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_FLEXIO,
        .section_index = ECU_DEVICE_FLEXIO_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 0,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_FLEXIO,
        .section_index = ECU_DEVICE_FLEXIO_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 1,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_MOTOR,
        .section_index = ECU_DEVICE_MOTOR_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 2,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_MOTOR,
        .section_index = ECU_DEVICE_MOTOR_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 3,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_STEPPER,
        .section_index = ECU_DEVICE_STEPPER_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 4,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_WBLS,
        .section_index = ECU_DEVICE_WBLS_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 6,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_HW_WBLS,
        .section_index = ECU_DEVICE_WBLS_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 4,
        .sector_index = 7,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_CKP,
        .section_index = ECU_SENSOR_CKP_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 0,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_CMP,
        .section_index = ECU_SENSOR_CMP_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 2,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_CMP,
        .section_index = ECU_SENSOR_CMP_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 3,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_CMP,
        .section_index = ECU_SENSOR_CMP_3,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 4,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_CMP,
        .section_index = ECU_SENSOR_CMP_4,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 5,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_ECT,
        .section_index = ECU_SENSOR_ECT_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 6,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_ECT,
        .section_index = ECU_SENSOR_ECT_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 7,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_IAT,
        .section_index = ECU_SENSOR_IAT_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 8,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_IAT,
        .section_index = ECU_SENSOR_IAT_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 9,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_MAF,
        .section_index = ECU_SENSOR_MAF_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 10,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_MAF,
        .section_index = ECU_SENSOR_MAF_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 11,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_MAP,
        .section_index = ECU_SENSOR_MAP_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 12,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_MAP,
        .section_index = ECU_SENSOR_MAP_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 13,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_MAP,
        .section_index = ECU_SENSOR_MAP_3,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 14,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_MAP,
        .section_index = ECU_SENSOR_MAP_4,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 5,
        .sector_index = 15,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_TPS,
        .section_index = ECU_SENSOR_TPS_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 6,
        .sector_index = 0,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_TPS,
        .section_index = ECU_SENSOR_TPS_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 6,
        .sector_index = 1,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_VSS,
        .section_index = ECU_SENSOR_VSS_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 6,
        .sector_index = 2,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_APS,
        .section_index = ECU_SENSOR_APS_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 6,
        .sector_index = 3,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_EGT,
        .section_index = ECU_SENSOR_EGT_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 6,
        .sector_index = 4,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_SENS_EGT,
        .section_index = ECU_SENSOR_EGT_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 6,
        .sector_index = 5,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_TIMING,
        .section_index = ECU_MODULE_TIMING_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 0,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_ETC,
        .section_index = ECU_MODULE_ETC_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 2,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_ETC,
        .section_index = ECU_MODULE_ETC_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 3,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_VVT,
        .section_index = ECU_MODULE_VVT_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 8,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_VVT,
        .section_index = ECU_MODULE_VVT_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 9,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_VVT,
        .section_index = ECU_MODULE_VVT_3,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 10,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_VVT,
        .section_index = ECU_MODULE_VVT_4,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 11,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_FUELPUMP,
        .section_index = ECU_MODULE_FUELPUMP_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 12,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_FUELPUMP,
        .section_index = ECU_MODULE_FUELPUMP_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 13,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_COOLINGFAN,
        .section_index = ECU_MODULE_COOLINGFAN_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 14,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_COOLINGFAN,
        .section_index = ECU_MODULE_COOLINGFAN_2,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 8,
        .sector_index = 15,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_MODULE_IGNITION,
        .section_index = ECU_MODULE_IGNITION_1,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 9,
        .sector_index = 0,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_CALIBRATION_IO,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 12,
        .sector_index = 0,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_CALIBRATION_CYLINDERS,
        .section_length = ECU_FLASH_SECTOR_SIZE,
        .block_index = 12,
        .sector_index = 1,
        .uses_full_block = false,
    },
    {
        .section_type = FLASH_SECTION_TYPE_RUNTIME_CORRECTIONS,
        .section_length = ECU_FLASH_BLOCK_SIZE_128K,
        .block_index = 32,
        .sector_index = 0,
        .uses_full_block = true,
    },
};

static uint16_t flash_mem_layout_section_type_to_section_index[FLASH_SECTION_TYPE_MAX][FLASH_SECTION_INDEX_MAX] = {0};
static uint32_t flash_mem_layout_section_type_to_address[FLASH_SECTION_TYPE_MAX][FLASH_SECTION_INDEX_MAX] = {0};
static uint32_t flash_mem_layout_block_index_to_address[ECU_FLASH_BLOCKS_COUNT] = {0};

static const flash_mem_layout_t flash_mem_layout = {
    .mem_size = ECU_FLASH_SIZE * ECU_FLASH_DIES_COUNT,
    .sector_size = ECU_FLASH_SECTOR_SIZE,
    .blocks = ITEMSOF(flash_mem_layout_blocks),
    .blocks_info = flash_mem_layout_blocks,
    .blocks_addresses = flash_mem_layout_block_index_to_address,
    .sections = ITEMSOF(flash_mem_layout_sections),
    .sections_info = flash_mem_layout_sections,

    .section_type_max = FLASH_SECTION_TYPE_MAX,
    .section_index_max = FLASH_SECTION_INDEX_MAX,
    .section_type_to_section_layout_count = FLASH_SECTION_TYPE_MAX * FLASH_SECTION_INDEX_MAX,
    .section_type_to_section_layout = (uint16_t *)flash_mem_layout_section_type_to_section_index,
    .section_type_to_addresses = (uint32_t *)flash_mem_layout_section_type_to_address,
};

error_t flash_mem_layout_init(void)
{
  error_t err = E_OK;
  const flash_mem_layout_t *layout = &flash_mem_layout;
  uint16_t index = 0xFFFF;
  uint16_t block_index;
  uint32_t address;

  do {
    for(flash_section_type_t t = 0; t < layout->section_type_max; t++) {
      for(flash_section_index_t i = 0; i < layout->section_index_max; i++) {
        index = ECU_FLASH_SECTION_TYPE_INDEX_TO_LAYOUT(layout, t, i);
        layout->section_type_to_section_layout[index] = 0xFFFF;
        layout->section_type_to_addresses[index] = 0xFFFFFFFF;
      }
    }

    address = 0;
    for(uint32_t b = 0; b < layout->blocks; b++) {
      layout->blocks_addresses[b] = address;
      address += layout->blocks_info[b].block_size;
    }

    for(flash_section_type_t t = 0; t < layout->section_type_max; t++) {
      for(flash_section_index_t i = 0; i < layout->section_index_max; i++) {
        index = ECU_FLASH_SECTION_TYPE_INDEX_TO_LAYOUT(layout, t, i);
        for(int s = 0; s < layout->sections; s++) {
          block_index = layout->sections_info[s].block_index;
          if(layout->sections_info[s].section_type == t &&
              layout->sections_info[s].section_index == i) {
            if(layout->section_type_to_section_layout[index] == 0xFFFF &&
                layout->section_type_to_addresses[index] == 0xFFFFFFFF &&
                layout->sections_info[s].sector_index < layout->blocks_info[block_index].sectors_count &&
                layout->sections_info[s].block_index < layout->blocks &&
                (layout->sections_info[s].uses_full_block == true || (layout->sector_size * layout->sections_info[s].sector_index + layout->sections_info[s].section_length <= layout->blocks_info[block_index].block_size)) &&
                (layout->sections_info[s].uses_full_block == false || (layout->sections_info[s].sector_index == 0 && layout->sections_info[s].section_length <= layout->blocks_info[block_index].block_size))) {
              address = layout->blocks_addresses[block_index];
              address += layout->sector_size * layout->sections_info[s].sector_index;
              layout->section_type_to_addresses[index] = address;
              layout->section_type_to_section_layout[index] = s;
            } else {
              err = E_INVALACT;
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

error_t flash_mem_layout_get_section_address(uint32_t *address, uint16_t section_type, uint16_t section_index)
{
  error_t err = E_OK;
  uint16_t index;
  uint32_t addr;
  const flash_mem_layout_t *layout = &flash_mem_layout;

  do {
    BREAK_IF_ACTION(address == NULL || section_type >= FLASH_SECTION_TYPE_MAX || section_index >= FLASH_SECTION_INDEX_MAX, err = E_PARAM);

    index = ECU_FLASH_SECTION_TYPE_INDEX_TO_LAYOUT(layout, section_type, section_index);
    addr = layout->section_type_to_addresses[index];
    BREAK_IF_ACTION(addr == 0xFFFFFFF, err = E_NOTSUPPORT);

    *address = addr;

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
