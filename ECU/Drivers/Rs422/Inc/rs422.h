/*
 * rs422.h
 *
 *  Created on: 16 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef DRIVERS_RS422_INC_RS422_H_
#define DRIVERS_RS422_INC_RS422_H_

#include "common.h"
#include "rs422_types.h"
#include "versioned_rs422.h"

typedef struct rs422_ctx_tag rs422_ctx_t;
typedef void (*rs422_callback_func_t)(rs422_ctx_t *ctx, void *usrdata);

typedef struct {
    UART_HandleTypeDef *handle;
    DMA_HandleTypeDef *hdma;
    uint32_t dma_copy_threshold;
    uint32_t dma_tx_threshold;

    void *buffer_rx;
    uint32_t buffer_rx_size;

    void *buffer_tx;
    uint32_t buffer_tx_size;

    pUART_CallbackTypeDef tx_cplt_cb;
    pUART_CallbackTypeDef rx_halfcplt_cb;
    pUART_CallbackTypeDef rx_cplt_cb;
    pUART_CallbackTypeDef error_cb;

    rs422_callback_func_t callback_tx;
    rs422_callback_func_t callback_err;
    void *callback_usrdata;
}rs422_init_ctx_t;

typedef struct rs422_ctx_tag {
    rs422_config_t config;
    rs422_init_ctx_t init;
    bool tx_process_lock;

    bool initialized;
    bool configured;

    uint32_t rx_read_pointer;
    uint32_t rx_write_pointer;
    uint32_t rx_half_size;

    uint32_t tx_read_pointer;
    uint32_t tx_write_pointer;
    uint32_t tx_dma_length;
    bool tx_busy;

    error_t tx_errcode;

    uint32_t tx_errors;
    uint32_t rx_errors;

}rs422_ctx_t;

error_t rs422_init(rs422_ctx_t *ctx, const rs422_init_ctx_t *init_ctx);
error_t rs422_configure(rs422_ctx_t *ctx, const rs422_config_t *config);
error_t rs422_reset(rs422_ctx_t *ctx);

void rs422_loop_main(rs422_ctx_t *ctx);
void rs422_loop_slow(rs422_ctx_t *ctx);
void rs422_loop_comm(rs422_ctx_t *ctx);

void rs422_rx_cplt_irq(rs422_ctx_t *ctx);
void rs422_rx_halfcplt_irq(rs422_ctx_t *ctx);
void rs422_tx_cplt_irq(rs422_ctx_t *ctx);
void rs422_error_irq(rs422_ctx_t *ctx);

error_t rs422_tx(rs422_ctx_t *ctx, const void *buffer, uint32_t length);
error_t rs422_rx(rs422_ctx_t *ctx, void *buffer, uint32_t *p_length);
error_t rs422_rx_nocopy(rs422_ctx_t *ctx, const void **p_buffer, uint32_t *p_length);
error_t rs422_rx_release(rs422_ctx_t *ctx, uint32_t length);

#endif /* DRIVERS_RS422_INC_RS422_H_ */
