/*
 * timing.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef MODULES_TIMING_INC_TIMING_H_
#define MODULES_TIMING_INC_TIMING_H_

#include "common.h"
#include "versioned_timing.h"
#include "time.h"

#include "config_sensors.h"

typedef struct timing_diag_tag timing_diag_t;
typedef struct timing_data_tag timing_data_t;

typedef void (*timing_signal_update_cb_t)(void *usrdata, const timing_data_t *data, const timing_diag_t *diag);

typedef enum {
  TIMING_CRANKSHAFT_MODE_IDLE = 0,
  TIMING_CRANKSHAFT_MODE_DETECTED,
  TIMING_CRANKSHAFT_MODE_VALID,
  TIMING_CRANKSHAFT_MODE_VALID_PHASED,
  TIMING_CRANKSHAFT_MODE_MAX,
}timing_crankshaft_mode_t;

typedef struct timing_diag_tag {
    union {
        uint32_t data;
        struct {
            bool ckp_failure : 1;
            bool pos_calc_nan : 1;
            bool roughtest_to_camshaft_missmatch : 1;
        }bits;
    }crankshaft;

    union {
        uint32_t data;
        struct {
            bool cmp_failure : 1;
            bool sync_lost : 1;
            bool sync_bad : 1;
            bool signal_lost : 1;
            bool pos_too_early : 1;
            bool pos_too_late : 1;
        }bits;
    }camshafts[ECU_SENSOR_CMP_MAX];

}timing_diag_t;

typedef struct {
    ckp_data_t sensor_data;
    float pos_phased;
    timing_crankshaft_mode_t mode;
    bool sync_phase_at_odd_rev;

    struct {
      bool synchronized;
      bool sync_at_odd_rev;
    }roughtest;

    bool valid;
}timing_data_crankshaft_t;

typedef struct {
    cmp_data_t sensor_data;
    float pos_absolute;
    float pos_relative;
    bool valid;
}timing_data_camshaft_t;

typedef struct {
    bool synchronized;
    bool sync_at_odd_rev;
    ecu_sensor_cmp_t sync_camshaft_instance;
    timing_data_camshaft_t instances[ECU_SENSOR_CMP_MAX];

    bool valid;
}timing_data_camshafts_t;

typedef struct timing_data_tag {
    timing_data_crankshaft_t crankshaft;
    timing_data_camshafts_t camshafts;
}timing_data_t;

typedef struct {
    bool position_valid;
    bool phased;
    float position_prev;
}timing_req_t;

typedef struct {
    ecu_sensor_ckp_t ckp_instance;
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

error_t timing_calculate_current_position(timing_ctx_t *ctx, float offset, bool phased, timing_req_t *req_ctx, timing_data_crankshaft_t *data);
error_t timing_calculate_offset_position(timing_ctx_t *ctx, float offset, bool phased, timing_req_t *req_ctx, timing_data_crankshaft_t *data);

void timing_position_clamp(float input, bool phased, float *output);

error_t timing_get_data(timing_ctx_t *ctx, timing_data_t *data);
error_t timing_get_diag(timing_ctx_t *ctx, timing_diag_t *diag);

void timing_loop_main(timing_ctx_t *ctx);
void timing_loop_slow(timing_ctx_t *ctx);
void timing_loop_fast(timing_ctx_t *ctx);


#endif /* MODULES_TIMING_INC_TIMING_H_ */
