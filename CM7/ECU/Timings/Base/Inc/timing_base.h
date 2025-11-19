/*
 * timing_base.h
 *
 *  Created on: Jun 6, 2024
 *      Author: VHEMaster
 */

#ifndef TIMING_BASE_INC_TIMING_BASE_H_
#define TIMING_BASE_INC_TIMING_BASE_H_

#include "common.h"
#include "versioned_timing_base.h"
#include "time.h"

#include "config_sensors.h"
#include "config_engine.h"

typedef struct timing_base_diag_tag timing_base_diag_t;
typedef struct timing_base_data_tag timing_base_data_t;

typedef void (*timing_base_signal_update_cb_t)(void *usrdata, const timing_base_data_t *data, const timing_base_diag_t *diag);

typedef enum {
  TIMING_CRANKSHAFT_MODE_IDLE = 0,
  TIMING_CRANKSHAFT_MODE_DETECTED,
  TIMING_CRANKSHAFT_MODE_VALID,
  TIMING_CRANKSHAFT_MODE_VALID_PHASED,
  TIMING_CRANKSHAFT_MODE_MAX,
}timing_base_crankshaft_mode_t;

typedef enum {
  TIMING_RUNTIME_CYLINDER_SEQUENTIALED_NONE = 0,
  TIMING_RUNTIME_CYLINDER_SEQUENTIAL,
  TIMING_RUNTIME_CYLINDER_SEMISEQUENTIAL_DISTRIBUTOR,
  TIMING_RUNTIME_CYLINDER_SEQUENTIALED_MAX,
}timing_base_runtime_cylinder_sequentialed_type_t;

typedef struct timing_base_diag_tag {
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

}timing_base_diag_t;

typedef struct {
    ckp_data_t sensor_data;
    float pos_phased;
    timing_base_crankshaft_mode_t mode;
    bool sync_phase_at_odd_rev;

    struct {
      bool synchronized;
      bool sync_at_odd_rev;
    }roughtest;

    bool valid;
}timing_base_data_crankshaft_t;

typedef struct {
    cmp_data_t sensor_data;
    float pos_absolute;
    float pos_relative;
    bool valid;
}timing_base_data_camshaft_t;

typedef struct {
    bool synchronized;
    bool sync_at_odd_rev;
    ecu_sensor_cmp_t sync_camshaft_instance;
    timing_base_data_camshaft_t instances[ECU_SENSOR_CMP_MAX];

    bool valid;
}timing_base_data_camshafts_t;

typedef struct {
    bool position_valid;
    bool phased;
    float position_prev;
}timing_base_req_t;

typedef struct {
    timing_base_req_t timing_req;
    timing_base_data_crankshaft_t crankshaft_data;
}timing_base_cylinder_sequentialed_cylinder_ctx_t;

typedef struct {
    timing_base_cylinder_sequentialed_cylinder_ctx_t cylinders[ECU_CYLINDER_MAX];
}timing_base_cylinder_sequentialed_ctx_t;

typedef struct timing_base_data_tag {
    timing_base_data_crankshaft_t crankshaft;
    timing_base_data_camshafts_t camshafts;
    timing_base_cylinder_sequentialed_ctx_t sequentialed[TIMING_RUNTIME_CYLINDER_SEQUENTIALED_MAX];
}timing_base_data_t;

typedef struct {
    ecu_sensor_ckp_t ckp_instance;
    ecu_sensor_cmp_t cmp_instances[ECU_SENSOR_CMP_MAX];
    timing_base_signal_update_cb_t signal_update_cb;
    void *signal_update_usrdata;
    const ecu_config_engine_calibration_t *calibration_config;
}timing_base_init_ctx_t;

typedef struct {
    bool configured;

    void *ignition_ctx;
    void *injection_ctx;
    void *rough_ctx;
}timing_base_runtime_t;

typedef struct {
    timing_base_init_ctx_t init;
    timing_base_config_t config;
    bool ready;
    bool configured;

    timing_base_runtime_t runtime;

    timing_base_diag_t diag;
    timing_base_data_t data;

    uint32_t process_update_trigger_counter;

}timing_base_ctx_t;

error_t timing_base_init(timing_base_ctx_t *ctx, const timing_base_init_ctx_t *init_ctx);
error_t timing_base_configure(timing_base_ctx_t *ctx, const timing_base_config_t *config);
error_t timing_base_reset(timing_base_ctx_t *ctx);

void timing_base_ckp_signal_update(timing_base_ctx_t *ctx, const ckp_data_t *data, const ckp_diag_t *diag);
void timing_base_cmp_signal_update(timing_base_ctx_t *ctx, ecu_sensor_cmp_t cmp_instance, const cmp_data_t *data, const cmp_diag_t *diag);
void timing_base_signal_update_cb(void *usrdata, const timing_base_data_t *data, const timing_base_diag_t *diag);

error_t timing_base_get_crankshaft_data(timing_base_ctx_t *ctx, timing_base_data_crankshaft_t *data);
error_t timing_base_get_data(timing_base_ctx_t *ctx, timing_base_data_t *data);
error_t timing_base_get_data_ptr(timing_base_ctx_t *ctx, const timing_base_data_t **data);
error_t timing_base_get_diag(timing_base_ctx_t *ctx, timing_base_diag_t *diag);

#endif /* TIMING_BASE_INC_TIMING_BASE_H_ */
