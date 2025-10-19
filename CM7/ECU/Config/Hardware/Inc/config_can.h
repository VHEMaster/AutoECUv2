/*
 * config_can.h
 *
 *  Created on: Oct 19, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_INC_CONFIG_CAN_H_
#define CONFIG_INC_CONFIG_CAN_H_

#include "can.h"

typedef enum {
  ECU_CAN_IF_MAIN = 0,
  ECU_CAN_IF_MAX
}ecu_can_if_enum_t;

error_t ecu_config_can_init(void);
error_t ecu_config_can_register_callbacks(can_cfg_t *can_cfg, ecu_can_if_enum_t interface);
error_t ecu_config_can_get_if_cfg(can_cfg_t *can_cfg, ecu_can_if_enum_t interface);

#endif /* CONFIG_INC_CONFIG_CAN_H_ */
