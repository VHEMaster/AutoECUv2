/*
 * config_ipc_buffer_reader.h
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_BUFFER_READER_H_
#define CONFIG_IPC_INC_CONFIG_IPC_BUFFER_READER_H_

#include "config_ipc.h"
#include "ipc_buffer_reader.h"

error_t ecu_ipc_buffer_reader_init(ecu_ipc_buffer_reader_t instance, ipc_buffer_reader_ctx_t *ctx);

#endif /* CONFIG_IPC_INC_CONFIG_IPC_BUFFER_READER_H_ */
