/*
 * cmp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_CMP_INC_CMP_H_
#define SENSORS_CMP_INC_CMP_H_

#include "common.h"
#include "ckp.h"
#include "inputs.h"
#include "time.h"
#include "versioned_cmp.h"

typedef enum {
  CMP_INSTANCE_1 = 0,
  CMP_INSTANCE_2,
  CMP_INSTANCE_3,
  CMP_INSTANCE_4,
  CMP_INSTANCE_MAX
}cmp_instance_t;

typedef struct cmp_ctx_tag cmp_ctx_t;
typedef struct cmp_data_tag cmp_data_t;
typedef union cmp_diag_tag cmp_diag_t;

typedef error_t (*cmp_signal_ref_init_cb_t)(cmp_ctx_t *ctx, cmp_instance_t instance_index, void **usrdata);
typedef void (*cmp_signal_ref_loop_cb_t)(cmp_ctx_t *ctx, void *usrdata);
typedef void (*cmp_signal_ref_signal_cb_t)(cmp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata);
typedef void (*cmp_signal_ref_ckp_update_cb_t)(cmp_ctx_t *ctx, void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);

typedef void (*cmp_signal_update_cb_t)(void *usrdata, const cmp_data_t *data, const cmp_diag_t *diag);
typedef error_t (*cmp_ckp_update_req_cb_t)(void *usrdata, ckp_req_t *req, ckp_data_t *data);

typedef struct {
    cmp_signal_ref_init_cb_t func_init_cb;
    cmp_signal_ref_signal_cb_t func_signal_cb;
    cmp_signal_ref_loop_cb_t func_main_cb;
    cmp_signal_ref_loop_cb_t func_slow_cb;
    cmp_signal_ref_loop_cb_t func_fast_cb;
    cmp_signal_ref_ckp_update_cb_t func_ckp_update_cb;
}cmp_signal_ref_cfg_t;

typedef struct {
    const cmp_signal_ref_cfg_t *cfg;
    void *usrdata;
}cmp_signal_ref_ctx_t;

typedef union cmp_diag_tag {
    uint32_t data;
    struct {
        bool signal_sequence : 1;
        bool no_signal : 1;
        bool wrong_signal : 1;
        bool extra_signal : 1;
        bool signal_lost : 1;
        bool signal_width : 1;
        bool bad_pulse : 1;
        bool ckp_error : 1;
    }bits;
}cmp_diag_t;

typedef enum {
  CMP_DATA_NONE = 0,
  CMP_DATA_DETECTED,
  CMP_DATA_SYNCHRONIZED,
  CMP_DATA_VALID,
  CMP_DATA_MAX
}cmp_data_validity_t;

typedef struct cmp_data_tag {
    float position;
    cmp_data_validity_t validity;
    bool sync_at_odd_rev;
}cmp_data_t;

typedef struct {
    float position_prev;
}cmp_req_t;

typedef struct  {
    cmp_instance_t instance_index;
    ckp_instance_t ckp_instance_index;
    cmp_signal_update_cb_t signal_update_cb;
    void *signal_update_usrdata;
    cmp_ckp_update_req_cb_t ckp_update_req_cb;
    void *ckp_update_usrdata;
}cmp_init_ctx_t;

typedef struct cmp_ctx_tag {
    cmp_init_ctx_t init;
    cmp_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    cmp_req_t req;
    cmp_data_t data;
    cmp_diag_t diag;

    cmp_signal_ref_ctx_t signal_ref_type_ctx;

    bool started;
    time_us_t startup_time;

    time_msmnt_item_t load_signal_cb;
    time_msmnt_item_t load_update_cb;

}cmp_ctx_t;

error_t cmp_init(cmp_ctx_t *ctx, const cmp_init_ctx_t *init_ctx);
error_t cmp_configure(cmp_ctx_t *ctx, const cmp_config_t *config);
error_t cmp_reset(cmp_ctx_t *ctx);

void cmp_loop_main(cmp_ctx_t *ctx);
void cmp_loop_slow(cmp_ctx_t *ctx);
void cmp_loop_fast(cmp_ctx_t *ctx);
void cmp_ckp_signal_update(void *usrdata, const ckp_data_t *data, const ckp_diag_t *diag);

error_t cmp_get_value(cmp_ctx_t *ctx, cmp_data_t *data);
error_t cmp_get_diag(cmp_ctx_t *ctx, cmp_diag_t *diag);

#endif /* SENSORS_CMP_INC_CMP_H_ */
