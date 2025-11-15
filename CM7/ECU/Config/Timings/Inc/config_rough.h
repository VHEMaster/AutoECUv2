/*
 * config_rough.h
 *
 *  Created on: Nov 14, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_ROUGH_H_
#define CONFIG_INC_CONFIG_ROUGH_H_

#include "versioned_timings.h"

typedef enum {
  ECU_TIMING_ROUGH_READ_PARAM_MAX
}ecu_timing_rough_read_params_t;

typedef enum {
  ECU_TIMING_ROUGH_WRITE_PARAM_MAX
}ecu_timing_rough_write_params_t;

error_t ecu_timings_rough_init(ecu_timing_rough_t instance, rough_ctx_t *ctx);
error_t ecu_timings_rough_get_default_config(ecu_timing_rough_t instance, rough_config_t *config);
error_t ecu_timings_rough_configure(ecu_timing_rough_t instance, const rough_config_t *config);
error_t ecu_timings_rough_reset(ecu_timing_rough_t instance);

error_t ecu_timings_rough_get_runtime_data_ptr(ecu_timing_rough_t instance, rough_runtime_ctx_t **data);

#endif /* CONFIG_INC_CONFIG_ROUGH_H_ */
