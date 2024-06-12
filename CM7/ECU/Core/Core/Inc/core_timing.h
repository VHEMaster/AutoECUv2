/*
 * core_timing.h
 *
 *  Created on: Jun 12, 2024
 *      Author: VHEMaster
 */

#ifndef CORE_CORE_INC_CORE_TIMING_H_
#define CORE_CORE_INC_CORE_TIMING_H_

#include "core.h"

void core_timing_signal_update_cb(void *usrdata, const timing_data_t *data, const timing_diag_t *diag);

#endif /* CORE_CORE_INC_CORE_TIMING_H_ */
