/*
 * config_timing_base.h

 *  Created on: Nov 13, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_TIMING_BASE_H_
#define CONFIG_INC_CONFIG_TIMING_BASE_H_

#include "config_timings.h"
#include "timing_base.h"

#include "versioned_ignition.h"
#include "versioned_injection.h"

#define ECU_TIMINGS_BASE_CALLBACKS_MAX   16

typedef enum {
  ECU_TIMING_BASE_READ_PARAM_MODE = 0,
  ECU_TIMING_BASE_READ_PARAM_RPM,
  ECU_TIMING_BASE_READ_PARAM_PERIOD,
  ECU_TIMING_BASE_READ_PARAM_REVS_COUNT,
  ECU_TIMING_BASE_READ_PARAM_MAX
}ecu_timing_base_read_params_t;

typedef enum {
  ECU_TIMING_BASE_WRITE_PARAM_MAX
}ecu_timing_base_write_params_t;

error_t ecu_timings_base_init(ecu_timing_base_t instance, timing_base_ctx_t *ctx);
error_t ecu_timings_base_get_default_config(ecu_timing_base_t instance, timing_base_config_t *config);
error_t ecu_timings_base_configure(ecu_timing_base_t instance, const timing_base_config_t *config);
error_t ecu_timings_base_reset(ecu_timing_base_t instance);

error_t ecu_timings_base_register_cb(ecu_timing_base_t instance, timing_base_signal_update_cb_t callback, void *usrdata);

error_t ecu_timings_base_get_crankshaft_data(ecu_timing_base_t instance, timing_base_data_crankshaft_t *data);
error_t ecu_timings_base_get_data(ecu_timing_base_t instance, timing_base_data_t *data);
error_t ecu_timings_base_get_data_ptr(ecu_timing_base_t instance, const timing_base_data_t **data);
error_t ecu_timings_base_get_diag(ecu_timing_base_t instance, timing_base_diag_t *diag);

#endif /* CONFIG_INC_CONFIG_TIMING_BASE_H_ */
