/*
 * flash_fsm.h
 *
 *  Created on: May 4, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_FLASH_INC_FLASH_FSM_H_
#define CONFIG_FLASH_INC_FLASH_FSM_H_

#include "flash.h"
#include "crc.h"
#include "config_devices.h"
#include "flash_memory_layout.h"
#include "bool.h"

typedef enum {
  FLASH_CMD_NONE = 0,
  FLASH_CMD_READ,
  FLASH_CMD_WRITE,
  FLASH_CMD_MAX
}flash_cmd_type_t;

typedef enum {
  FLASH_FSM_CONDITION = 0,
  FLASH_FSM_FLASH_LOCK,
  FLASH_FSM_SECTION_INFO,
  FLASH_FSM_SECTION_ADDR,

  FLASH_FSM_READ_HEADER,
  FLASH_FSM_READ_HEADER_SYNC,
  FLASH_FSM_READ_PAYLOAD,
  FLASH_FSM_READ_PAYLOAD_SYNC,
  FLASH_FSM_READ_CHECKSUM_LOCK,
  FLASH_FSM_READ_CHECKSUM_HEADER,
  FLASH_FSM_READ_CHECKSUM_PAYLOAD,
  FLASH_FSM_READ_CHECKSUM_UNLOCK,
  FLASH_FSM_READ_VERIFY,

  FLASH_FSM_WRITE_CHECKSUM_LOCK,
  FLASH_FSM_WRITE_CHECKSUM_HEADER,
  FLASH_FSM_WRITE_CHECKSUM_PAYLOAD,
  FLASH_FSM_WRITE_CHECKSUM_UNLOCK,
  FLASH_FSM_WRITE_ERASE,
  FLASH_FSM_WRITE_ERASE_SYNC,
  FLASH_FSM_WRITE_HEADER,
  FLASH_FSM_WRITE_HEADER_SYNC,
  FLASH_FSM_WRITE_PAYLOAD,
  FLASH_FSM_WRITE_PAYLOAD_SYNC,
  FLASH_FSM_WRITE_MEMMAP_ENABLE,
  FLASH_FSM_WRITE_MEMMAP_CRC_LOCK,
  FLASH_FSM_WRITE_MEMMAP_CRC_HEADER,
  FLASH_FSM_WRITE_MEMMAP_CRC_PAYLOAD,
  FLASH_FSM_WRITE_MEMMAP_CRC_UNLOCK,
  FLASH_FSM_WRITE_MEMMAP_DISABLE,
  FLASH_FSM_WRITE_CHECKSUM_VERIFY,
  FLASH_FSM_WRITE_ADDR,

  FLASH_FSM_FLASH_UNLOCK,

  FLASH_FSM_MAX,
}flash_fsm_process_t;

typedef enum {
  FLASH_ERASE_TYPE_NONE = 0,
  FLASH_ERASE_TYPE_SECTOR,
  FLASH_ERASE_TYPE_BLOCK
}flash_erase_type_t;

typedef struct {
  flash_section_header_t section_header ALIGNED_CACHE;
}flash_runtime_dma_ctx_t;

typedef struct {
    qspi_ctx_t *qspi_ctx;
    bool ready;
    bool locked;

    flash_cmd_type_t cmd_request;
    error_t cmd_errcode;
    error_t cmd_errcode_internal;

    const flash_mem_layout_section_info_t *cmd_section;
    uint16_t cmd_section_type;
    uint16_t cmd_section_index;
    flash_payload_version_t cmd_payload_version;
    flash_payload_version_t *cmd_payload_version_p;
    const void *cmd_payload_tx_temp;
    const void *cmd_payload_tx;
    void *cmd_payload_rx;
    uint32_t cmd_address_base;
    uint32_t cmd_address;
    uint32_t cmd_length;
    uint32_t cmd_cur_len;
    uint32_t cmd_left_len;
    flash_erase_type_t cmd_erase_type;
    flash_fsm_process_t fsm_process;
    crc_checksum_t checksum;

    uint32_t cmd_xaddress;
    uint8_t cmd_dupl_index;

    flash_runtime_dma_ctx_t *dma_ctx;

}flash_runtime_ctx_t;

error_t flash_fsm(flash_runtime_ctx_t *ctx);

#endif /* CONFIG_FLASH_INC_FLASH_FSM_H_ */
