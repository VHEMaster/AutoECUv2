/*
 * core_comm.h
 *
 *  Created on: Oct 23, 2025
 *      Author: VHEMaster
 */

#ifndef CORE_COMMUNICATION_INC_CORE_COMM_H_
#define CORE_COMMUNICATION_INC_CORE_COMM_H_

#include "core.h"
#include "core_comm_types.h"

error_t core_comm_init(ecu_core_ctx_t *ctx);
void core_comm_loop_comm(ecu_core_ctx_t *ctx);
void core_comm_loop_slow(ecu_core_ctx_t *ctx);
void core_comm_loop_main(ecu_core_ctx_t *ctx);

#endif /* CORE_COMMUNICATION_INC_CORE_COMM_H_ */
