/*
 * config_cylinders.c
 *
 *  Created on: Jun 9, 2024
 *      Author: VHEMaster
 */

#include "common.h"
#include "config_calibration.h"
#include "versioned_calibration.h"

static const ecu_config_cylinders_t ecu_calibration_cylinders_config_default = {
    .cylinders_count = 4,
    .cylinders = {
        {
            .bank = ECU_BANK_1,
            .pos_offset = 360.0f,
            .cy_opposed = ECU_CYLINDER_4,
        }, //CYLINDERS_CY_1
        {
            .bank = ECU_BANK_2,
            .pos_offset = 180.0f,
            .cy_opposed = ECU_CYLINDER_3,
        }, //CYLINDERS_CY_2
        {
            .bank = ECU_BANK_2,
            .pos_offset = 540.0f,
            .cy_opposed = ECU_CYLINDER_2,
        }, //CYLINDERS_CY_3
        {
            .bank = ECU_BANK_1,
            .pos_offset = 0.0f,
            .cy_opposed = ECU_CYLINDER_1,
        }, //CYLINDERS_CY_4
    },
};

error_t ecu_calibration_cylinders_get_default_config(ecu_calibration_instance_t instance, ecu_config_cylinders_t *config)
{
  error_t err = E_OK;

  do {
    (void)instance;

    memcpy(config, &ecu_calibration_cylinders_config_default, sizeof(ecu_config_cylinders_t));

  } while(0);

  return err;
}
