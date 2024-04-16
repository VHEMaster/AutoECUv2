/*
 * tle6240_internal.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_TLE6240GP_INC_TLE6240_INTERNAL_H_
#define DEVICES_TLE6240GP_INC_TLE6240_INTERNAL_H_

#include "tle6240.h"

error_t tle6240_serial_operation(tle6240_ctx_t *ctx, uint8_t ctrl, uint8_t data, uint16_t *diag);

#endif /* DEVICES_TLE6240GP_INC_TLE6240_INTERNAL_H_ */
