/*
 * qspi.h
 *
 *  Created on: Apr 25, 2024
 *      Author: VHEMaster
 */

#ifndef FLASH_QSPI_H
#define FLASH_QSPI_H

#include "main.h"
#include "time.h"
#include "bool.h"
#include "errors.h"

#define QSPI_STATUS_POLL_INTERVAL     (96 * 50)
#define QSPI_CMD_TIMEOUT_US           (200)

typedef enum {
  QSPI_STATUS_REG_BUSY1 = 1,
  QSPI_STATUS_REG_WEL = 2,
  QSPI_STATUS_REG_WSE = 4,
  QSPI_STATUS_REG_WSP = 8,
  QSPI_STATUS_REG_WPLD = 16,
  QSPI_STATUS_REG_SEC = 32,
  QSPI_STATUS_REG_BUSY2 = 128,
}qspi_status_reg_t;

typedef struct {
    uint8_t mfg_id;
    uint8_t device_type;
    uint8_t device_id;
}qspi_jedec_t;

typedef struct {
    QSPI_HandleTypeDef *hqspi;
    uint32_t memory_base_address;

    void *tx_payload;
    void *rx_payload;

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

    QSPI_CommandTypeDef cmd_rsten;
    QSPI_CommandTypeDef cmd_rst;
    QSPI_CommandTypeDef cmd_eqio;
    QSPI_CommandTypeDef cmd_rstqio;
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
}qspi_init_t;

typedef struct {
    qspi_init_t init;
    bool ready;
    bool initialized;
    bool qspi_busy;
    bool locked;
    error_t init_errcode;

    QSPI_AutoPollingTypeDef cmd_poll;
    QSPI_CommandTypeDef *cmd_ptr;
    const void *cmd_payload_tx;
    void *cmd_payload_rx;
    bool cmd_ready;
    bool cmd_wren_needed;
    bool cmd_status_poll_needed;
    time_delta_us_t cmd_poll_timeout;
    error_t cmd_async_errcode;
    error_t cmd_errcode;

    qspi_status_reg_t status;
    qspi_jedec_t jedec;
    bool jedec_ready;
}qspi_ctx_t;


error_t qspi_init(qspi_ctx_t *ctx, const qspi_init_t *init_ctx);

void qspi_loop_main(qspi_ctx_t *ctx);
void qspi_loop_slow(qspi_ctx_t *ctx);
void qspi_loop_fast(qspi_ctx_t *ctx);

void qspi_cplt_cb(qspi_ctx_t *ctx);
void qspi_err_cb(qspi_ctx_t *ctx);

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

error_t qspi_prot_read(qspi_ctx_t *ctx, void *payload, uint32_t length);
error_t qspi_prot_write(qspi_ctx_t *ctx, const void *payload, uint32_t length);
error_t qspi_prot_lock(qspi_ctx_t *ctx);

error_t qspi_sync(qspi_ctx_t *ctx);

error_t qspi_get_jedec(qspi_ctx_t *ctx, qspi_jedec_t *jedec);

#endif /* FLASH_QSPI_H */
