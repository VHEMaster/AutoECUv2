/*
 * config_knockwindow.h
 *
 *  Created on: 5 окт. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_TIMINGS_INC_CONFIG_KNOCKWINDOW_H_
#define CONFIG_TIMINGS_INC_CONFIG_KNOCKWINDOW_H_

#include "versioned_timings.h"

typedef enum {
  ECU_TIMING_KNOCKWINDOW_READ_PARAM_MAX
}ecu_timing_knockwindow_read_params_t;

typedef enum {
  ECU_TIMING_KNOCKWINDOW_WRITE_PARAM_ALLOWED = 0,
  ECU_TIMING_KNOCKWINDOW_WRITE_PARAM_KNOCK_WINDOW_START,
  ECU_TIMING_KNOCKWINDOW_WRITE_PARAM_KNOCK_WINDOW_END,
  ECU_TIMING_KNOCKWINDOW_WRITE_PARAM_MAX
}ecu_timing_knockwindow_write_params_t;

error_t ecu_timings_knockwindow_init(ecu_timing_knockwindow_t instance, knockwindow_ctx_t *ctx);
error_t ecu_timings_knockwindow_get_default_config(ecu_timing_knockwindow_t instance, knockwindow_config_t *config);
error_t ecu_timings_knockwindow_configure(ecu_timing_knockwindow_t instance, const knockwindow_config_t *config);
error_t ecu_timings_knockwindow_reset(ecu_timing_knockwindow_t instance);

error_t ecu_timings_knockwindow_get_runtime_data_ptr(ecu_timing_knockwindow_t instance, knockwindow_runtime_ctx_t **data);



#endif /* CONFIG_TIMINGS_INC_CONFIG_KNOCKWINDOW_H_ */
