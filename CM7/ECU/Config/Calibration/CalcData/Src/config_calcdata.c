/*
 * config_calcdata.c
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

#include "config_calcdata_relation_sources.h"
#include "config_calcdata_output_data.h"
#include "config_calcdata_setup.h"

error_t ecu_calibration_calcdata_get_default_config(ecu_calibration_instance_t instance, ecu_config_calcdata_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    ecu_calibration_calcdata_relation_sources_get_default_config(instance, &config->relation_sources);
    ecu_calibration_calcdata_output_data_get_default_config(instance, &config->output_data);
    ecu_calibration_calcdata_setup_get_default_config(instance, &config->setup);

  } while(0);

  return err;
}
