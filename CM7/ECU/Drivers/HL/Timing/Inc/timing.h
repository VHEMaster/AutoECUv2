/*
 * timing.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_HL_TIMING_INC_TIMING_H_
#define DRIVERS_HL_TIMING_INC_TIMING_H_

#include "versioned_timing.h"
#include "errors.h"
#include "bool.h"
#include "time.h"
#include <stdint.h>

#include "config_sensors.h"

typedef union timing_diag_tag timing_diag_t;
typedef struct timing_data_tag timing_data_t;

typedef void (*timing_signal_update_cb_t)(void *usrdata, const timing_data_t *data, const timing_diag_t *diag);

typedef enum {
  TIMING_CRANKSHAFT_MODE_IDLE = 0,
  TIMING_CRANKSHAFT_MODE_DETECTED,
  TIMING_CRANKSHAFT_MODE_VALID,
  TIMING_CRANKSHAFT_MODE_VALID_PHASED,
  TIMING_CRANKSHAFT_MODE_MAX,
}timing_crankshaft_mode_t;

typedef union timing_diag_tag {
    uint32_t data;
    struct {

    }bits;
}timing_diag_t;

typedef struct {
    ckp_data_t sensor_data;
    float pos_phased;
    timing_crankshaft_mode_t mode;
    bool sensor_has_failure;
}timing_data_crankshaft_t;

typedef struct {
    cmp_data_t sensor_data;
    float pos_absolute;
    float pos_relative;
    bool valid;
    bool sensor_has_failure;
}timing_data_camshaft_t;

typedef struct {
    bool synchronized;
    bool sync_at_odd_rev;
    ecu_sensor_cmp_t sync_camshaft_instance;
    timing_data_camshaft_t instances[ECU_SENSOR_CMP_MAX];
}timing_data_camshafts_t;

typedef struct timing_data_tag {
    timing_data_crankshaft_t crankshaft;
    timing_data_camshafts_t camshafts;
}timing_data_t;

typedef struct {
    ecu_sensor_ckp_t ckp_instances[ECU_SENSOR_CKP_MAX];
    ecu_sensor_cmp_t cmp_instances[ECU_SENSOR_CMP_MAX];
    timing_signal_update_cb_t signal_update_cb;
    void *signal_update_usrdata;
}timing_init_ctx_t;

typedef struct {
    timing_init_ctx_t init;
    timing_config_t config;
    bool ready;
    bool configured;

    timing_diag_t diag;
    timing_data_t data;

}timing_ctx_t;

error_t timing_init(timing_ctx_t *ctx, const timing_init_ctx_t *init_ctx);
error_t timing_configure(timing_ctx_t *ctx, const timing_config_t *config);
error_t timing_reset(timing_ctx_t *ctx);

void timing_ckp_signal_update(timing_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag);
void timing_cmp_signal_update(timing_ctx_t *ctx, ecu_sensor_cmp_t cmp_instance, const cmp_data_t *data, const cmp_diag_t *diag);

error_t timing_get_data(timing_ctx_t *ctx, timing_data_t *data);
error_t timing_get_diag(timing_ctx_t *ctx, timing_diag_t *diag);

void timing_loop_main(timing_ctx_t *ctx);
void timing_loop_slow(timing_ctx_t *ctx);
void timing_loop_fast(timing_ctx_t *ctx);


#endif /* DRIVERS_HL_TIMING_INC_TIMING_H_ */
