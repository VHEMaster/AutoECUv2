/*
 * flash.h
 *
 *  Created on: May 4, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_FLASH_INC_FLASH_H_
#define CONFIG_FLASH_INC_FLASH_H_

#include "common.h"

#define ECU_FLASH_SECTION_HEADER_LENGTH     (sizeof(flash_section_header_t))

typedef uint16_t flash_payload_version_t;

typedef enum {
  FLASH_SECTION_TYPE_EMPTY = 0,

  FLASH_SECTION_TYPE_ID,
  FLASH_SECTION_TYPE_HW_FLEXIO,
  FLASH_SECTION_TYPE_HW_MOTOR,
  FLASH_SECTION_TYPE_HW_STEPPER,
  FLASH_SECTION_TYPE_HW_WBLS,

  FLASH_SECTION_TYPE_SENS_CKP,
  FLASH_SECTION_TYPE_SENS_CMP,
  FLASH_SECTION_TYPE_SENS_ECT,
  FLASH_SECTION_TYPE_SENS_EGT,
  FLASH_SECTION_TYPE_SENS_IAT,
  FLASH_SECTION_TYPE_SENS_MAF,
  FLASH_SECTION_TYPE_SENS_MAP,
  FLASH_SECTION_TYPE_SENS_TPS,
  FLASH_SECTION_TYPE_SENS_VSS,
  FLASH_SECTION_TYPE_SENS_APS,

  FLASH_SECTION_TYPE_MODULE_TIMING,
  FLASH_SECTION_TYPE_MODULE_ETC,

  FLASH_SECTION_TYPE_CORE_COMPONENT_CYLINDERS,

  FLASH_SECTION_TYPE_CALIBRATION_CYLINDERS,
  FLASH_SECTION_TYPE_RUNTIME_CORRECTIONS,

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
    uint16_t payload_version;
    uint16_t pages;
    uint16_t reserved[11];
    uint16_t crc;
}flash_section_header_t ALIGNED_CACHE;

error_t flash_init(void);
void flash_loop_fast(void);

error_t flash_lock(void);
error_t flash_unlock(void);

error_t flash_section_read(uint16_t section_type, uint16_t section_index, flash_payload_version_t *payload_version, void *payload, uint32_t length);
error_t flash_section_write(uint16_t section_type, uint16_t section_index, flash_payload_version_t payload_version, const void *payload, uint32_t length);
error_t flash_mem_base_address(uint32_t *base_address);
error_t flash_mem_section_address(uint32_t *section_address, uint16_t section_type, uint16_t section_index);
error_t flash_mem_header_len(uint32_t *header_len);

#endif /* CONFIG_FLASH_INC_FLASH_H_ */
