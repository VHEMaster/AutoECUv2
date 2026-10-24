/* CM4 -> CM7 services. The high-rate parameter path uses value mailboxes. */
#include "comm_ipc.h"
#include "config_common.h"
error_t comm_ipc_dispatch(uint32_t key, void *payload, uint32_t *length)
{
  if(*length != sizeof(comm_ipc_parameter_rpc_t)) { *length = 0; return E_PARAM; }
  comm_ipc_parameter_rpc_t *request = payload;
  ecu_config_parameter_id_t id = { .id = request->parameter_id };
  if(key != COMM_IPC_PARAMETER_READ && key != COMM_IPC_PARAMETER_WRITE) { *length = 0; return E_NOTSUPPORT; }
  ecu_core_runtime_value_ctx_t *parameter;
  error_t err = ecu_config_common_get_parameter_ptr_by_id(id, &parameter);
  if(err != E_OK) { *length = 0; return err; }
  uint32_t prim = EnterCritical();
  if(key == COMM_IPC_PARAMETER_WRITE) *parameter = request->value;
  else request->value = *parameter;
  ExitCritical(prim);
  if(key == COMM_IPC_PARAMETER_WRITE) *length = 0;
  return E_OK;
}
