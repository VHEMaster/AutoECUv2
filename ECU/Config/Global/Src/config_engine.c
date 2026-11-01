/*
 * config_engine.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_engine.h"

ALIGNED_CACHE BUFFER_DMA ecu_config_global_engine_t ecu_config_global_engine = {0};
ALIGNED_CACHE BUFFER_DMA ecu_config_global_engine_double_t ecu_config_global_engine_double = {0};

error_t ecu_config_global_get_global_engine_config(const ecu_config_global_engine_t **config)
{
  error_t err = E_OK;
  ecu_config_global_engine_t *global_config = &ecu_config_global_engine;

  do {
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    *config = global_config;

  } while(0);

  return err;
}

error_t ecu_config_global_get_engine_calibration_config(const ecu_config_engine_calibration_t **config)
{
  error_t err = E_OK;
  ecu_config_global_engine_t *global_config = &ecu_config_global_engine;

  do {
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    *config = &global_config->calibration;

  } while(0);

  return err;
}

error_t ecu_config_global_get_engine_runtime_config(ecu_config_engine_runtime_t **config)
{
  error_t err = E_OK;
  ecu_config_global_engine_t *global_config = &ecu_config_global_engine;

  do {
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);

    *config = &global_config->runtime;

  } while(0);

  return err;
}
