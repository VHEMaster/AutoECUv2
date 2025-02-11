/*
 * config_tables.c
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_tables_t ecu_tables_config_default = {

};

error_t ecu_calibration_tables_get_default_config(ecu_calibration_instance_t instance, ecu_config_tables_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_tables_config_default, sizeof(ecu_config_tables_t));

  } while(0);

  return err;
}
