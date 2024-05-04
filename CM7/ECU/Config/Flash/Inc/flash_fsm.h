/*
 * flash_fsm.h
 *
 *  Created on: May 4, 2024
 *      Author: VHEMaster
 */

#ifndef CONFIG_FLASH_INC_FLASH_FSM_H_
#define CONFIG_FLASH_INC_FLASH_FSM_H_

#include "flash.h"
#include "config_devices.h"
#include "bool.h"

typedef enum {
  FLASH_CMD_NONE = 0,
  FLASH_CMD_READ,
  FLASH_CMD_WRITE
}flash_cmd_type_t;

typedef struct {
    qspi_ctx_t *qspi_ctx;
    bool ready;
    bool locked;

    flash_cmd_type_t cmd_request;
    error_t cmd_errcode;

    uint32_t cmd_address;
    const void *cmd_payload_tx;
    void *cmd_payload_rx;
    uint32_t cmd_length;
    flash_erase_type_t cmd_erase_type;

}flash_runtime_ctx_t;

error_t flash_fsm(flash_runtime_ctx_t *ctx);

#endif /* CONFIG_FLASH_INC_FLASH_FSM_H_ */
