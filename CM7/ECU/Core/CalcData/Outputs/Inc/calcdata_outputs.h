/*
 * calcdata_outputs.h
 *
 *  Created on: Jun 8, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CALCDATA_INC_CORE_CALCDATA_OUTPUTS_H_
#define CORE_CALCDATA_INC_CORE_CALCDATA_OUTPUTS_H_

#include "calcdata.h"

typedef enum {
  CORE_CALCDATA_OUTPUTS_STAGE_1 = 0,
  CORE_CALCDATA_OUTPUTS_STAGE_2,
  CORE_CALCDATA_OUTPUTS_STAGE_3,
  CORE_CALCDATA_OUTPUTS_STAGE_MAX,
}ecu_core_calcdata_outputs_stage_t;

void core_calcdata_outputs_process(ecu_core_ctx_t *ctx, ecu_core_calcdata_outputs_stage_t stage);

#endif /* CORE_CALCDATA_INC_CORE_CALCDATA_OUTPUTS_H_ */
