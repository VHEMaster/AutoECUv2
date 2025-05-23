/*
 * l9960_internal.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_L9960_INC_L9960_INTERNAL_H_
#define DEVICES_L9960_INC_L9960_INTERNAL_H_

#include "l9960.h"

error_t l9960_serial_operation(l9960_ctx_t *ctx, l9960_request_payload_t request, l9960_response_payload_t *response);
void l9960_internal_set_enabled(l9960_ctx_t *ctx, bool enabled);

#endif /* DEVICES_L9960GP_INC_L9960_INTERNAL_H_ */
