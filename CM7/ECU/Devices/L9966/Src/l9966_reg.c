/*
 * l9966_reg.c
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#include "l9966_reg.h"
#include "compiler.h"

error_t l9966_reg_init(l9966_reg_cfg_map_t *register_map)
{
  error_t err = E_OK;
  uint8_t map_index = 0;

  register_map->raw[map_index].addr = L9966_REG_DWT_VOLT_SRC_LSF_CTRL;
  register_map->raw[map_index].value = &register_map->data.dwt_volt_src_lsf_ctrl.data;
  map_index++;

  register_map->raw[map_index].addr = L9966_REG_SWITCH_ROUTE;
  register_map->raw[map_index].value = &register_map->data.switch_route.data;
  map_index++;

  register_map->raw[map_index].addr = L9966_REG_ACTIVE_DISCHARGE_LSF_CTRL;
  register_map->raw[map_index].value = &register_map->data.active_discharge_lsf_ctrl.data;
  map_index++;

  for(int i = 0; i < ITEMSOF(register_map->data.curr_src_ctrl); i++) {
    register_map->raw[map_index].addr = L9966_REG_CURR_SRC_CTRL_x + i;
    register_map->raw[map_index].value = &register_map->data.curr_src_ctrl[i].data;
    map_index++;
  }

  for(int i = 0; i < ITEMSOF(register_map->data.gtm_to_sent_route); i++) {
    register_map->raw[map_index].addr = L9966_REG_GTM_TO_SENT_ROUTE_x + i;
    register_map->raw[map_index].value = &register_map->data.gtm_to_sent_route[i].data;
    map_index++;
  }

  register_map->raw[map_index].addr = L9966_REG_WAK_MSK;
  register_map->raw[map_index].value = &register_map->data.wak_msk.data;
  map_index++;

  register_map->raw[map_index].addr = L9966_REG_SLEEP_CONFIG;
  register_map->raw[map_index].value = &register_map->data.sleep_config.data;
  map_index++;

  register_map->raw[map_index].addr = L9966_REG_VRS;
  register_map->raw[map_index].value = &register_map->data.reg_vrs.data;
  map_index++;

  register_map->raw[map_index].addr = L9966_REG_SQNCR_INT_MSK_FLG;
  register_map->raw[map_index].value = &register_map->data.sqncr_int_msk_flg.data;
  map_index++;

  register_map->raw[map_index].addr = L9966_REG_ADC_TIMING;
  register_map->raw[map_index].value = &register_map->data.adc_timing.data;
  map_index++;

  register_map->raw[map_index].addr = L9966_REG_SQNCR_CTRL;
  register_map->raw[map_index].value = &register_map->data.sqncr_ctrl.data;
  map_index++;

  for(int i = 0; i < ITEMSOF(register_map->data.sqncr_cmd); i++) {
    register_map->raw[map_index].addr = L9966_REG_SQNCR_CMD_x + i;
    register_map->raw[map_index].value = &register_map->data.sqncr_cmd[i].data;
    map_index++;
  }

  register_map->raw[map_index].addr = L9966_REG_GEN_STATUS;
  register_map->raw[map_index].value = &register_map->data.status.data;
  map_index++;

  register_map->raw_count = map_index;

  return err;
}
