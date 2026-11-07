/*
 * config_ipc_buffer_writer.h
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_BUFFER_WRITER_H_
#define CONFIG_IPC_INC_CONFIG_IPC_BUFFER_WRITER_H_

#include "config_ipc.h"
#include "ipc_buffer_writer.h"

error_t ecu_ipc_buffer_writer_init(ecu_ipc_buffer_writer_t instance, ipc_buffer_writer_ctx_t *ctx);

#endif /* CONFIG_IPC_INC_CONFIG_IPC_BUFFER_WRITER_H_ */
