/*
 * obd2.h
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#ifndef COMMUNICATION_OBD2_INC_OBD2_H_
#define COMMUNICATION_OBD2_INC_OBD2_H_

#include "obd2_types.h"
#include "versioned_obd2.h"

typedef struct obd2_ctx_tag obd2_ctx_t;

typedef void (*obd2_error_callback_t)(obd2_ctx_t *ctx, obd2_error_code_t code, void *userdata);

typedef struct {
    obd2_error_callback_t error_callback;
    void *callback_userdata;
}obd2_init_ctx_t;

typedef struct obd2_ctx_tag {
    obd2_config_t config;
    obd2_init_ctx_t init;
    bool initialized;
    bool configured;

    const obd2_mode1_setup_t *mode1_setup;
    obd2_mode1_data_t mode1_data[OBD2_PID_01_MAX];
    obd2_mode9_data_t mode9_data[OBD2_PID_09_MAX];

    obd2_error_code_t error_code;
    bool reset_trigger;

    uint8_t upstream_data[OBD2_DATA_LENGTH_MAX];
    uint16_t upstream_data_len;
    bool upstream_available;

    uint8_t downstream_data[OBD2_DATA_LENGTH_MAX];
    uint16_t downstream_data_len;
    bool downstream_available;

    time_us_t downstream_time;

}obd2_ctx_t;

error_t obd2_init(obd2_ctx_t *ctx, const obd2_init_ctx_t *init);
error_t obd2_configure(obd2_ctx_t *ctx, const obd2_config_t *config);
void obd2_loop(obd2_ctx_t *ctx);

error_t obd2_reset(obd2_ctx_t *ctx);

error_t obd2_message_write_upstream(obd2_ctx_t *ctx, const uint8_t *payload, uint16_t length);
error_t obd2_message_read_downstream(obd2_ctx_t *ctx, uint8_t *payload, uint16_t *length);

#endif /* COMMUNICATION_OBD2_INC_OBD2_H_ */
