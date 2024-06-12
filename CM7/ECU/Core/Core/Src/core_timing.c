/*
 * core_timing.c
 *
 *  Created on: Jun 12, 2024
 *      Author: VHEMaster
 */

#include "core_timing.h"
#include "common.h"

ITCM_FUNC void core_timing_signal_update_cb(void *usrdata, const timing_data_t *data, const timing_diag_t *diag)
{
  ecu_core_ctx_t *ecu_core_ctx = (ecu_core_ctx_t *)usrdata;

  (void)ecu_core_ctx;
}
