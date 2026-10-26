/*
 * crc_fsm.h
 *
 *  Created on: May 5, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_CRC_INC_CRC_FSM_H_
#define DRIVERS_CRC_INC_CRC_FSM_H_

#include "crc.h"
#include "time.h"
#include "config_extern.h"

#define CRC_DMA_MAX_LEN       (64 * 1024)
#define CRC_DMA_TIMEOUT_US    (10 * TIME_US_IN_MS)

typedef enum {
  CRC_FSM_CONDITION = 0,
  CRC_FSM_DMA_REQUEST,
  CRC_FSM_DMA_WAIT,
  CRC_FSM_DMA_WAIT_ABORT,
  CRC_FSM_DMA_MAX
}crc_fsm_t;

typedef struct {
    CRC_HandleTypeDef *hcrc;
    MDMA_HandleTypeDef *hdma;
    bool ready;
    bool locked;
    crc_fsm_t fsm_process;

    bool cmd_ready;
    bool cmd_reset;
    error_t cmd_errcode;
    uint32_t cmd_address;
    uint32_t cmd_length;
    uint32_t cmd_len_cur;
    uint32_t cmd_len_left;
    error_t dma_errcode;
    time_us_t dma_timestamp;

}crc_ctx_t;

error_t crc_fsm(crc_ctx_t *ctx);

#endif /* DRIVERS_CRC_INC_CRC_FSM_H_ */
