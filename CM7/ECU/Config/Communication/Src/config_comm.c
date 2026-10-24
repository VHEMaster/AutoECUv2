/* Runtime registry and all communication loops are owned by CM4. */
#include "comm_ipc.h"
#include "time.h"
void ecu_comm_loop_main(void)
{
  ipc_rpc_service(0u, comm_ipc_dispatch);
  comm_ipc_parameters_poll();
}
void ecu_comm_loop_slow(void) { }
void ecu_comm_loop_comm(void) { }
error_t ecu_comm_init(void) { return E_OK; }
