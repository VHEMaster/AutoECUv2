/*
 * ckp.h
 *
 *  Created on: May 14, 2024
 *      Author: VHEMaster
 */

#ifndef SENSORS_CKP_INC_CKP_H_
#define SENSORS_CKP_INC_CKP_H_

#include "inputs.h"
#include "errors.h"
#include "time.h"
#include "versioned_ckp.h"
#include <stdint.h>

typedef enum {
  CKP_INSTANCE_1 = 0,
  CKP_INSTANCE_MAX
}ckp_instance_t;

typedef struct ckp_ctx_tag ckp_ctx_t;
typedef struct ckp_data_tag ckp_data_t;
typedef union ckp_diag_tag ckp_diag_t;

typedef error_t (*ckp_signal_ref_init_cb_t)(ckp_ctx_t *ctx, ckp_instance_t instance_index, void **usrdata);
typedef void (*ckp_signal_ref_loop_cb_t)(ckp_ctx_t *ctx, void *usrdata);
typedef void (*ckp_signal_ref_signal_cb_t)(ckp_ctx_t *ctx, ecu_gpio_input_level_t level, void *usrdata);

typedef void (*ckp_signal_update_cb_t)(void *usrdata, ckp_data_t *data, ckp_diag_t *diag);

typedef struct {
    ckp_signal_ref_init_cb_t func_init_cb;
    ckp_signal_ref_signal_cb_t func_signal_cb;
    ckp_signal_ref_loop_cb_t func_main_cb;
    ckp_signal_ref_loop_cb_t func_slow_cb;
    ckp_signal_ref_loop_cb_t func_fast_cb;
}ckp_signal_ref_cfg_t;

typedef struct {
    const ckp_signal_ref_cfg_t *cfg;
    void *usrdata;
}ckp_signal_ref_ctx_t;

typedef union ckp_diag_tag {
    uint32_t data;
    struct {

    }bits;
}ckp_diag_t;

typedef struct ckp_data_tag {

}ckp_data_t;

typedef struct {
    ckp_instance_t instance_index;
    ckp_signal_update_cb_t signal_update_cb;
}ckp_init_ctx_t;

typedef struct ckp_ctx_tag {
    ckp_init_ctx_t init;
    ckp_config_t config;
    bool ready;
    bool configured;
    bool pin_locked;

    input_id_t input_id;

    ckp_data_t data;
    ckp_diag_t diag;

    ckp_signal_ref_ctx_t signal_ref_type_ctx;

    bool started;
    time_us_t startup_time;

}ckp_ctx_t;

error_t ckp_init(ckp_ctx_t *ctx, const ckp_init_ctx_t *init_ctx);
error_t ckp_configure(ckp_ctx_t *ctx, const ckp_config_t *config);
error_t ckp_reset(ckp_ctx_t *ctx);

void ckp_loop_main(ckp_ctx_t *ctx);
void ckp_loop_slow(ckp_ctx_t *ctx);
void ckp_loop_fast(ckp_ctx_t *ctx);

error_t ckp_get_value(ckp_ctx_t *ctx, ckp_data_t *data);
error_t ckp_get_diag(ckp_ctx_t *ctx, ckp_diag_t *diag);

#endif /* SENSORS_CKP_INC_CKP_H_ */
