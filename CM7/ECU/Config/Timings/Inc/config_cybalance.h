/*
 * config_cybalance.h
 *
 *  Created on: Nov 14, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_CYBALANCE_H_
#define CONFIG_INC_CONFIG_CYBALANCE_H_

#include "versioned_timings.h"

typedef enum {
  ECU_TIMING_CYBALANCE_READ_PARAM_MAX
}ecu_timing_cybalance_read_params_t;

typedef enum {
  ECU_TIMING_CYBALANCE_WRITE_PARAM_MAX
}ecu_timing_cybalance_write_params_t;

error_t ecu_timings_cybalance_init(ecu_timing_cybalance_t instance, cybalance_ctx_t *ctx);
error_t ecu_timings_cybalance_get_default_config(ecu_timing_cybalance_t instance, cybalance_config_t *config);
error_t ecu_timings_cybalance_configure(ecu_timing_cybalance_t instance, const cybalance_config_t *config);
error_t ecu_timings_cybalance_reset(ecu_timing_cybalance_t instance);

error_t ecu_timings_cybalance_get_runtime_data_ptr(ecu_timing_cybalance_t instance, cybalance_runtime_ctx_t **data);

#endif /* CONFIG_INC_CONFIG_CYBALANCE_H_ */
