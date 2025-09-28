/*
 * config_powermoding.c
 *
 *  Created on: Aug 18, 2025
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_powermoding_t ecu_calibration_powermoding_config_default = {
    .standby_state_phy_accept_time = 200 * TIME_US_IN_MS,
    .sleep_state_phy_accept_time = 200 * TIME_US_IN_MS,
};

error_t ecu_calibration_powermoding_get_default_config(ecu_calibration_instance_t instance, ecu_config_powermoding_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_calibration_powermoding_config_default, sizeof(ecu_config_powermoding_t));

  } while(0);

  return err;
}
