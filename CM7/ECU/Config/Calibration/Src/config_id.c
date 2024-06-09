/*
 * config_id.c
 *
 *  Created on: May 9, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_data_identification_t ecu_calibration_id_config_default = {

};

error_t ecu_calibration_id_get_default_config(ecu_calibration_instance_t instance, ecu_config_data_identification_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_calibration_id_config_default, sizeof(ecu_config_data_identification_t));

  } while(0);

  return err;
}
