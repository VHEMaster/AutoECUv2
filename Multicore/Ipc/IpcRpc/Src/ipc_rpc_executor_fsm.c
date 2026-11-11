/*
 * ipc_rpc_executor_fsm.c
 *
 *  Created on: 11 нояб. 2026 г.
 *      Author: VHEMaster
 */

#include "ipc_rpc_executor_fsm.h"
#include "ipc_rpc_executor.h"

error_t ipc_rpc_executor_fsm(ipc_rpc_executor_ctx_t *executor, ipc_rpc_executor_fsm_ctx_t *fsm,
    ipc_rpc_executor_fsm_start_cb_t start, ipc_rpc_executor_fsm_unpack_cb_t unpack,
    ipc_rpc_executor_fsm_execute_cb_t execute, ipc_rpc_executor_fsm_finish_cb_t finish,
    void *usrdata)
{

}
