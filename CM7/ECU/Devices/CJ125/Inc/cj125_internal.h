/*
 * cj125_internal.h
 *
 *  Created on: Apr 19, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_CJ125_INC_CJ125_INTERNAL_H_
#define DEVICES_CJ125_INC_CJ125_INTERNAL_H_

#include "cj125.h"

error_t cj125_serial_operation(cj125_ctx_t *ctx, cj125_payload_t request, cj125_payload_t *response);

#endif /* DEVICES_CJ125GP_INC_CJ125_INTERNAL_H_ */
