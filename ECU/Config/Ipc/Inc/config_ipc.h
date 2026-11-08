/*
 * config_ipc.h
 *
 *  Created on: 7 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef CONFIG_IPC_INC_CONFIG_IPC_H_
#define CONFIG_IPC_INC_CONFIG_IPC_H_

#include "common.h"
#include "ipc_sharedmemory.h"
#include "ipc_fifo_writer.h"
#include "ipc_fifo_reader.h"
#include "ipc_buffer_writer.h"
#include "ipc_buffer_reader.h"
#include "ipc_transfer_sender.h"
#include "ipc_transfer_receiver.h"

#include "config_ipc_types.h"

error_t ecu_ipc_init(void);

error_t ecu_ipc_get_ipc_ctx(ecu_ipc_type_t type, ecu_ipc_instance_t instance, void **ctx);

error_t ecu_ipc_get_type_max(ecu_ipc_type_t *type_max);
error_t ecu_ipc_get_instance_max(ecu_ipc_type_t type, ecu_ipc_instance_t *instance_max);

error_t ecu_ipc_get_sharedmemory_ctx(ecu_ipc_sharedmemory_t instance, ipc_sharedmemory_ctx_t **ctx);
error_t ecu_ipc_get_fifo_writer_ctx(ecu_ipc_fifo_writer_t instance, ipc_fifo_writer_ctx_t **ctx);
error_t ecu_ipc_get_fifo_reader_ctx(ecu_ipc_fifo_reader_t instance, ipc_fifo_reader_ctx_t **ctx);
error_t ecu_ipc_get_buffer_writer_ctx(ecu_ipc_buffer_writer_t instance, ipc_buffer_writer_ctx_t **ctx);
error_t ecu_ipc_get_buffer_reader_ctx(ecu_ipc_buffer_reader_t instance, ipc_buffer_reader_ctx_t **ctx);
error_t ecu_ipc_get_transfer_sender_ctx(ecu_ipc_transfer_sender_t instance, ipc_transfer_sender_ctx_t **ctx);
error_t ecu_ipc_get_transfer_receiver_ctx(ecu_ipc_transfer_receiver_t instance, ipc_transfer_receiver_ctx_t **ctx);

#endif /* CONFIG_IPC_INC_CONFIG_IPC_H_ */
