/*
 * can.h
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_CAN_INC_CAN_H_
#define COMMUNICATION_CAN_INC_CAN_H_

#include "common.h"
#include "can_types.h"
#include "gpio.h"
#include "versioned_can.h"

#define CAN_RX_CB_MAX     (16)
#define CAN_ERR_CB_MAX    (8)
#define CAN_RX_FIFO_SIZE  (64)

#if defined(HAL_FDCAN_MODULE_ENABLED)
#define CAN_HANDLE_TYPE FDCAN_HandleTypeDef
#define CAN_FILTER_TYPE FDCAN_FilterTypeDef
#elif defined(HAL_CAN_MODULE_ENABLED)
#define CAN_HANDLE_TYPE CAN_HandleTypeDef
#define CAN_FILTER_TYPE CAN_FilterTypeDef
#else
#error "No CAN module enabled"
#endif

typedef struct can_ctx_tag can_ctx_t;
typedef void (*can_rx_callback_func_t)(can_ctx_t *ctx, const can_message_t *message, void *usrdata);
typedef void (*can_err_callback_func_t)(can_ctx_t *ctx, void *usrdata);

typedef struct {
    CAN_HANDLE_TYPE *handle;
    gpio_t lbk_pin;

#if (USE_HAL_FDCAN_REGISTER_CALLBACKS == 1UL)
  pFDCAN_RxFifo0CallbackTypeDef rx_fifo0_cb;
  pFDCAN_RxFifo1CallbackTypeDef rx_fifo1_cb;
  pFDCAN_CallbackTypeDef rx_buffer_cb;
  pFDCAN_CallbackTypeDef error_cb;
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
}can_init_ctx_t;

typedef struct {
    bool enabled;
    uint32_t msg_id;
    can_rx_callback_func_t func;
    void *usrdata;
}can_rx_cb_t;

typedef struct {
    bool enabled;
    can_err_callback_func_t func;
    void *usrdata;
}can_err_cb_t;

typedef struct {
    can_message_t buffer[CAN_RX_FIFO_SIZE];
    uint32_t read;
    uint32_t write;
}can_rxfifo_t;

typedef struct can_ctx_tag {
    can_config_t config;
    can_init_ctx_t init;
    bool initialized;
    bool configured;

    can_rx_cb_t rx_callbacks[CAN_RX_CB_MAX];
    can_err_cb_t err_callbacks[CAN_ERR_CB_MAX];
    FDCAN_RxHeaderTypeDef rxheader;
    FDCAN_TxHeaderTypeDef txheader;
    can_message_t rxmessage;

    CAN_FILTER_TYPE filter;

    can_rxfifo_t rxfifo;
}can_ctx_t;

error_t can_init(can_ctx_t *ctx, const can_init_ctx_t *init_ctx);
error_t can_configure(can_ctx_t *ctx, const can_config_t *config);
error_t can_reset(can_ctx_t *ctx);

void can_loop_main(can_ctx_t *ctx);
void can_loop_slow(can_ctx_t *ctx);
void can_loop_comm(can_ctx_t *ctx);

void can_rx_fifo0_irq(can_ctx_t *ctx, uint32_t fifo_its);
void can_rx_fifo1_irq(can_ctx_t *ctx, uint32_t fifo_its);
void can_rx_buffer_irq(can_ctx_t *ctx);
void can_error_irq(can_ctx_t *ctx);

error_t can_tx(can_ctx_t *ctx, const can_message_t *message);
error_t can_rx(can_ctx_t *ctx, can_message_t *message);

error_t can_register_rx_callback(can_ctx_t *ctx, uint32_t msg_id, can_rx_callback_func_t func, void *usrdata);
error_t can_register_err_callback(can_ctx_t *ctx, can_err_callback_func_t func, void *usrdata);

#endif /* COMMUNICATION_CAN_INC_CAN_H_ */
