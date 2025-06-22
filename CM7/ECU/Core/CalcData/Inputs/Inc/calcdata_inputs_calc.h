/*
 * calcdata_inputs_calc.h
 *
 *  Created on: Jun 6, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CALCDATA_INPUTS_INC_CALCDATA_INPUTS_CALC_H_
#define CORE_CALCDATA_INPUTS_INC_CALCDATA_INPUTS_CALC_H_

#include "calcdata_inputs.h"

void calcdata_inputs_calc_iat_manifold(ecu_core_ctx_t *ctx);
void calcdata_inputs_calc_runup_flag(ecu_core_ctx_t *ctx);
void calcdata_inputs_calc_cycle_charge(ecu_core_ctx_t *ctx);
void calcdata_inputs_calc_mass_air_flow(ecu_core_ctx_t *ctx);

#endif /* CORE_CALCDATA_INPUTS_INC_CALCDATA_INPUTS_CALC_H_ */
