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
#include "bool.h"

typedef enum {
  FLASH_CMD_NONE = 0,
  FLASH_CMD_READ,
  FLASH_CMD_WRITE,
  FLASH_CMD_MAX
}flash_cmd_type_t;

typedef enum {
  FLASH_FSM_CONDITION = 0,
  FLASH_FSM_SECTION_INFO,
  FLASH_FSM_SECTION_ADDR,
  FLASH_FSM_READ_HEADER,
  FLASH_FSM_READ_PAYLOAD,
  FLASH_FSM_READ_CHECKSUM_LOCK,
  FLASH_FSM_READ_CHECKSUM_HEADER,
  FLASH_FSM_READ_CHECKSUM_PAYLOAD,
  FLASH_FSM_READ_CHECKSUM_UNLOCK,
  FLASH_FSM_ERASE,
  FLASH_FSM_WRITE_HEADER,
  FLASH_FSM_WRITE_PAYLOAD,
  FLASH_FSM_MAX,
}flash_fsm_process_t;

typedef enum {
  FLASH_ERASE_TYPE_NONE = 0,
  FLASH_ERASE_TYPE_SECTOR,
  FLASH_ERASE_TYPE_BLOCK
}flash_erase_type_t;

typedef struct {
    qspi_ctx_t *qspi_ctx;
    bool ready;
    bool locked;

    flash_cmd_type_t cmd_request;
    error_t cmd_errcode;

    uint16_t cmd_section_type;
    uint16_t cmd_section_index;
    const void *cmd_payload_tx;
    void *cmd_payload_rx;
    uint32_t cmd_address;
    uint32_t cmd_length;
    flash_erase_type_t cmd_erase_type;
    flash_fsm_process_t fsm_process;
    flash_section_header_t section_header;
    crc_checksum_t checksum;

    uint32_t cmd_xaddress;
    uint8_t cmd_dupl_index;

}flash_runtime_ctx_t;

error_t flash_fsm(flash_runtime_ctx_t *ctx);

#endif /* CONFIG_FLASH_INC_FLASH_FSM_H_ */
