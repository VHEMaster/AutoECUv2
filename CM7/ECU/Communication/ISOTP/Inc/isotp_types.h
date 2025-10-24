/*
 * isotp_types.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef COMMUNICATION_ISOTP_INC_ISOTP_TYPES_H_
#define COMMUNICATION_ISOTP_INC_ISOTP_TYPES_H_

#include "common.h"
#include "time.h"
#include "versioned_isotp.h"

#define ISOTP_PAYLOAD_LEN_MAX   4095u
#define ISOTP_FRAME_FIFO_LEN    16u
#define ISOTP_FRAME_LEN         8u

typedef enum {
  ISOTP_OK = 0,
  ISOTP_TIMEOUT_AS,
  ISOTP_TIMEOUT_BS,
  ISOTP_TIMEOUT_CR,
  ISOTP_TIMEOUT_AR,
  ISOTP_WRONG_SN,
  ISOTP_WRONG_DL,
  ISOTP_INVALID_FS,
  ISOTP_RX_OVERFLOW,
  ISOTP_BUFFER_OVERFLOW,
  ISOTP_UNEXPECTED_PDU,
  ISOTP_IGNORED_PDU,
  ISOTP_STMIN_VIOLATION,
  ISOTP_MAX
}isotp_error_code_t;

typedef enum {
  ISOTP_STATE_IDLE = 0,

  ISOTP_STATE_RX_SF_FF,
  ISOTP_STATE_RX_FC_OVF,
  ISOTP_STATE_RX_FC,
  ISOTP_STATE_RX_CF,
  ISOTP_STATE_RX_CPLT,

  ISOTP_STATE_TX_SF_FF,
  ISOTP_STATE_TX_WAIT_FC,
  ISOTP_STATE_TX_CF,

}isotp_state_t;

typedef struct isotp_ctx_tag isotp_ctx_t;

typedef void (*isotp_error_callback_t)(isotp_ctx_t *ctx, isotp_error_code_t code, void *userdata);

typedef struct {
    isotp_error_callback_t error_callback;
    void *callback_userdata;
}isotp_init_ctx_t;

typedef struct {
    uint8_t payload[ISOTP_FRAME_LEN];
}isotp_frame_t;

typedef struct {
    isotp_frame_t buffer[ISOTP_FRAME_FIFO_LEN];
    uint32_t read;
    uint32_t write;
}isotp_frame_fifo_t;

typedef struct {
    uint8_t payload[ISOTP_PAYLOAD_LEN_MAX];
    uint16_t length;
    bool ready;
    uint32_t error_code_counter[ISOTP_MAX];
}isotp_data_t;

typedef struct {
    uint16_t left;
    uint16_t pos;
    uint16_t sn;
    uint16_t bs;
    uint16_t bn;
    bool safc;
    time_delta_us_t separation_time;
}isotp_fc_t;

typedef struct isotp_ctx_tag {
    isotp_config_t config;
    isotp_init_ctx_t init;
    bool initialized;
    bool configured;

    isotp_frame_fifo_t frame_fifo_downstream;
    isotp_frame_fifo_t frame_fifo_upstream;

    isotp_data_t data_downstream;
    isotp_data_t data_upstream;

    isotp_fc_t flowcontrol;

    isotp_error_code_t local_error_code;
    isotp_error_code_t error_code;
    isotp_state_t state;
    time_us_t state_time;
    bool reset_trigger;

}isotp_ctx_t;

#endif /* COMMUNICATION_ISOTP_INC_ISOTP_TYPES_H_ */
