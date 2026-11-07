/*
 * config_ipc_fifo_writer.h
 *
 *  Created on: 8 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_FIFO_WRITER_H_
#define CONFIG_IPC_INC_CONFIG_IPC_FIFO_WRITER_H_

#include "config_ipc.h"
#include "ipc_fifo_writer.h"

error_t ecu_ipc_fifo_writer_init(ecu_ipc_fifo_writer_t instance, ipc_fifo_writer_ctx_t *ctx);

#endif /* CONFIG_IPC_INC_CONFIG_IPC_FIFO_WRITER_H_ */
