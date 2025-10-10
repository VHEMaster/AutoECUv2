/*
 * iso-tp.h
 *
 *  Created on: Oct 7, 2025
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_ISOTP_INC_ISOTP_H_
#define COMMUNICATION_ISOTP_INC_ISOTP_H_

#include "common.h"
#include "time.h"

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
  ISOTP_STMIN_VIOLATION,
  ISOTP_MAX
}isotp_error_code_t;

typedef enum {
  ISOTP_STATE_IDLE = 0,

  ISOTP_STATE_RX_SF_FF,
  ISOTP_STATE_RX_FC,
  ISOTP_STATE_RX_CF,
  ISOTP_STATE_RX_CPLT,

  ISOTP_STATE_TX_SF_FF,
  ISOTP_STATE_TX_WAIT_FC,
  ISOTP_STATE_TX_CF,

}isotp_state_t;

typedef struct isotp_ctx_tag isotp_ctx_t;

typedef struct {
    time_us_t timeout;
    uint16_t upstream_block_size;
    time_delta_us_t upstream_min_separation_time;
    time_delta_us_t upstream_separation_time_gap;
}isotp_config_t;

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

    isotp_frame_fifo_t frame_fifo_downstream;
    isotp_frame_fifo_t frame_fifo_upstream;

    isotp_data_t data_downstream;
    isotp_data_t data_upstream;

    isotp_fc_t flowcontrol;

    isotp_error_code_t local_error_code;
    isotp_error_code_t error_code;
    isotp_state_t state;
    time_us_t state_time;


}isotp_ctx_t;

error_t isotp_init(isotp_ctx_t *ctx, const isotp_config_t *config);
error_t isotp_loop(isotp_ctx_t *ctx);

error_t isotp_frame_write_upstream(isotp_ctx_t *ctx, const isotp_frame_t *frame);
error_t isotp_frame_read_downstream(isotp_ctx_t *ctx, isotp_frame_t *frame);

error_t isotp_data_write_downstream(isotp_ctx_t *ctx, const uint8_t *payload, uint16_t length);
error_t isotp_data_read_upstream(isotp_ctx_t *ctx, uint8_t *payload, uint16_t *length);

error_t isotp_data_get_error(isotp_ctx_t *ctx, isotp_error_code_t *code);

error_t isotp_reset(isotp_ctx_t *ctx);

#endif /* COMMUNICATION_ISOTP_INC_ISOTP_H_ */
