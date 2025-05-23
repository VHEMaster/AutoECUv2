/*
 * qspi.h
 *
 *  Created on: Apr 25, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_FLASH_QSPI_H
#define DRIVERS_FLASH_QSPI_H

#include "common.h"
#include "time.h"

#define QSPI_STATUS_POLL_INTERVAL     (80 * 5)
#define QSPI_QUAD_INIT_DELAY_US       (50)
#define QSPI_CMD_TIMEOUT_US           (100)
#define QSPI_PAYLOAD_TIMEOUT_US       (200)
#define QSPI_ABORT_TIMEOUT_US         (1 * TIME_US_IN_MS)

#define QSPI_BPR_TIMEOUT_US           (1 * TIME_US_IN_MS)
#define QSPI_BPR_SIZE                 (18)

typedef enum {
  QSPI_STATUS_REG_BUSY = 1,
  QSPI_STATUS_REG_WEL = 2,
  QSPI_STATUS_REG_WSE = 4,
  QSPI_STATUS_REG_WSP = 8,
  QSPI_STATUS_REG_WPLD = 16,
  QSPI_STATUS_REG_SEC = 32,
}qspi_status_reg_t;

typedef struct {
    uint8_t mfg_id;
    uint8_t device_type;
    uint8_t device_id;
}qspi_jedec_t;

typedef struct {
    uint8_t mfg_id[2];
    uint8_t device_type[2];
    uint8_t device_id[2];
}qspi_jedec_quad_t;

typedef struct {
    uint8_t bytes[QSPI_BPR_SIZE];
}qspi_bpr_t;

typedef struct {
    qspi_jedec_quad_t jedec_quad ALIGNED_CACHE;
    uint8_t payload_bpr[QSPI_BPR_SIZE * 2] ALIGNED_CACHE;
    uint8_t payload_dummy[32] ALIGNED_CACHE;
}qspi_dma_ctx_t;

typedef struct {
    QSPI_HandleTypeDef *hqspi;
    qspi_dma_ctx_t *dma_ctx;
    uint32_t memory_base_address;

    uint32_t flash_size;
    uint32_t flash_dies_count;
    uint32_t erase_sector_size;
    uint32_t erase_sectors_number;
    uint32_t prog_page_size;
    uint32_t prog_pages_number;
    uint32_t otp_size;
    uint32_t prot_size;

    time_delta_us_t delay_program_time;
    time_delta_us_t delay_sector_erase;
    time_delta_us_t delay_chip_erase;

    time_delta_us_t timeout_program_time;
    time_delta_us_t timeout_sector_erase;
    time_delta_us_t timeout_chip_erase;

    qspi_jedec_t expected_jedec;
    qspi_bpr_t bpr;

    QSPI_CommandTypeDef cmd_rsten;
    QSPI_CommandTypeDef cmd_rst;
    QSPI_CommandTypeDef cmd_eqio;
    QSPI_CommandTypeDef cmd_rstqios;
    QSPI_CommandTypeDef cmd_rstqioq;
    QSPI_CommandTypeDef cmd_rdsr;
    QSPI_CommandTypeDef cmd_wrsr;
    QSPI_CommandTypeDef cmd_rdcr;
    QSPI_CommandTypeDef cmd_hsread;

    QSPI_CommandTypeDef cmd_jid;
    QSPI_CommandTypeDef cmd_qjid;

    QSPI_CommandTypeDef cmd_wren;
    QSPI_CommandTypeDef cmd_wrdi;
    QSPI_CommandTypeDef cmd_se;
    QSPI_CommandTypeDef cmd_be;
    QSPI_CommandTypeDef cmd_ce;
    QSPI_CommandTypeDef cmd_prog;
    QSPI_CommandTypeDef cmd_wrsu;
    QSPI_CommandTypeDef cmd_wrre;

    QSPI_CommandTypeDef cmd_rbpr;
    QSPI_CommandTypeDef cmd_wbpr;
    QSPI_CommandTypeDef cmd_lbpr;

    QSPI_CommandTypeDef cmd_rsid;
    QSPI_CommandTypeDef cmd_psid;
    QSPI_CommandTypeDef cmd_lsid;

    QSPI_MemoryMappedTypeDef mem_map;
}qspi_init_t;

typedef enum {
  QSPI_FSM_INIT_CONDITION = 0,
  QSPI_FSM_INIT_QUAD_RST1,
  QSPI_FSM_INIT_QUAD_RST2,
  QSPI_FSM_INIT_RESET_EN,
  QSPI_FSM_INIT_RESET,
  QSPI_FSM_INIT_JEDEC,
  QSPI_FSM_INIT_QUAD,
  QSPI_FSM_INIT_QUAD_WAIT,
  QSPI_FSM_INIT_QJEDEC,
  QSPI_FSM_INIT_CFG,
  QSPI_FSM_INIT_BPR,
  QSPI_FSM_INIT_MAX
}qspi_fsm_init_t;

typedef enum {
  QSPI_FSM_PROCESS_CONDITION = 0,
  QSPI_FSM_PROCESS_IO,

  QSPI_FSM_MEMORYMAPPING_HAL,
  QSPI_FSM_MEMORYMAPPING_WAIT,

  QSPI_FSM_PROCESS_MAX
}qspi_fsm_process_t;

typedef enum {
  QSPI_FSM_IO_INITIAL = 0,
  QSPI_FSM_IO_WREN_REQ,
  QSPI_FSM_IO_WREN_WAIT,
  QSPI_FSM_IO_CMD_REQ,
  QSPI_FSM_IO_CMD_WAIT,
  QSPI_FSM_IO_PAYLOAD_REQ,
  QSPI_FSM_IO_PAYLOAD_WAIT,
  QSPI_FSM_IO_STATUS_DELAY,
  QSPI_FSM_IO_STATUS_REQ,
  QSPI_FSM_IO_STATUS_WAIT,
  QSPI_FSM_IO_ABORT_REQ,
  QSPI_FSM_IO_ABORT_WAIT,
  QSPI_FSM_IO_MAX
}qspi_fsm_io_t;

typedef struct {
    qspi_init_t init;
    bool ready;
    bool initialized;
    bool locked;
    bool init_request;
    error_t init_errcode;

    bool memory_mapping;
    bool memory_mapping_accept;

    QSPI_AutoPollingTypeDef cmd_poll;
    QSPI_CommandTypeDef *cmd_ptr;
    const void *cmd_payload_tx;
    void *cmd_payload_rx;
    bool cmd_ready;
    bool cmd_wren_needed;
    bool cmd_status_poll_needed;
    time_delta_us_t cmd_poll_delay;
    time_delta_us_t cmd_poll_timeout;
    error_t cmd_async_errcode;
    error_t cmd_errcode;
    time_us_t cmd_timestamp;
    bool cmd_uses_dma;

    qspi_fsm_init_t fsm_init;
    qspi_fsm_process_t fsm_process;
    qspi_fsm_io_t fsm_io;

    qspi_jedec_t jedec;
    bool jedec_ready;

    qspi_bpr_t bpr;
}qspi_ctx_t;


error_t qspi_init(qspi_ctx_t *ctx, const qspi_init_t *init_ctx);
error_t qspi_reset(qspi_ctx_t *ctx);

void qspi_loop_main(qspi_ctx_t *ctx);
void qspi_loop_slow(qspi_ctx_t *ctx);
void qspi_loop_fast(qspi_ctx_t *ctx);

void qspi_cplt_cb(qspi_ctx_t *ctx);
void qspi_err_cb(qspi_ctx_t *ctx);
void qspi_timeout_cb(qspi_ctx_t *ctx);

error_t qspi_lock(qspi_ctx_t *ctx);
error_t qspi_unlock(qspi_ctx_t *ctx);

error_t qspi_fast_read(qspi_ctx_t *ctx, uint32_t address, void *payload, uint32_t length);
error_t qspi_page_write(qspi_ctx_t *ctx, uint32_t address, const void *payload, uint32_t length);
error_t qspi_sector_erase(qspi_ctx_t *ctx, uint32_t address);
error_t qspi_block_erase(qspi_ctx_t *ctx, uint32_t address);
error_t qspi_chip_erase(qspi_ctx_t *ctx);


error_t qspi_otp_read(qspi_ctx_t *ctx, uint32_t address, void *payload, uint32_t length);
error_t qspi_otp_write(qspi_ctx_t *ctx, uint32_t address, const void *payload, uint32_t length);
error_t qspi_otp_lock(qspi_ctx_t *ctx);

error_t qspi_write_bpr(qspi_ctx_t *ctx, const qspi_bpr_t *bpr);

error_t qspi_sync(qspi_ctx_t *ctx);

error_t qspi_get_jedec(qspi_ctx_t *ctx, qspi_jedec_t *jedec);

error_t qspi_memory_mapping_set(qspi_ctx_t *ctx, bool enabled);

#endif /* DRIVERS_FLASH_QSPI_H */
