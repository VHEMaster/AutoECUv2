/*
 * calcdata_proc.h
 *
 *  Created on: Jun 7, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_CALCDATA_CORE_INC_CALCDATA_PROC_H_
#define CORE_CALCDATA_CORE_INC_CALCDATA_PROC_H_

#include "calcdata.h"


error_t core_calcdata_proc_calc_output(ecu_core_ctx_t *ctx,
    ecu_config_calcdata_output_data_index_t output_data_index,
    ecu_config_calcdata_output_varianted_index_t variant,
    ecu_core_runtime_value_ctx_t *banks_output);

#endif /* CORE_CALCDATA_CORE_INC_CALCDATA_PROC_H_ */
