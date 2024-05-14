/*
 * middlelayer_sensors.c
 *
 *  Created on: Apr 4, 2024
 *      Author: VHEMaster
 */

#include "middlelayer_sensors.h"
#include "config_sensors.h"

#include "config_ckp.h"
#include "config_cmp.h"
#include "config_ect.h"
#include "config_egt.h"
#include "config_iat.h"
#include "config_maf.h"
#include "config_map.h"
#include "config_tps.h"
#include "config_aps.h"

#include "compiler.h"

void middlelayer_sensors_loop_main(void)
{
  ecu_sensors_loop_main();
}

void middlelayer_sensors_loop_slow(void)
{
  ecu_sensors_loop_slow();
}

ITCM_FUNC void middlelayer_sensors_loop_fast(void)
{
  ecu_sensors_loop_fast();
}

void middlelayer_sensors_init(void)
{
  error_t err = E_OK;
  void *sensor_ctx;

  //TODO: Create DTC of sensors init
  (void)err;

  do {
    err = ecu_sensors_init();
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_CKP_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_CKP, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_ckp_init(i, (ckp_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_CKP, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_CMP_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_CMP, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_cmp_init(i, (cmp_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_CMP, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_ECT_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_ECT, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_ect_init(i, (ect_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_ECT, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_EGT_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_EGT, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_egt_init(i, (egt_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_EGT, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_IAT_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_IAT, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_iat_init(i, (iat_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_IAT, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_MAF_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_MAF, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_maf_init(i, (maf_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_MAF, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_MAP_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_MAP, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_map_init(i, (map_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_MAP, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_TPS_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_TPS, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_tps_init(i, (tps_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_TPS, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

    for(int i = 0; i < ECU_SENSOR_APS_MAX; i++) {
      err = ecu_sensors_get_sensor_ctx(ECU_SENSOR_TYPE_APS, i, &sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_aps_init(i, (aps_ctx_t *)sensor_ctx);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));

      err = ecu_sensors_set_sensor_initialized(ECU_SENSOR_TYPE_APS, i, true);
      BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));
    }
    BREAK_IF_ACTION(err != E_OK, BREAKPOINT(0));


  } while(0);
}
