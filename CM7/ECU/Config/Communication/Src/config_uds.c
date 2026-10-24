/* CM7 owns persistent/default configuration. Runtime lives on CM4. */
#include "config_uds.h"
#include "config_global.h"
#include "comm_ipc.h"
static const uds_config_t ecu_comm_uds_config_default = {

};
static const bool ecu_comm_uds_enabled_default[ECU_COMM_UDS_MAX] = {
    true,
    true,
    true
};
_Static_assert(sizeof(uds_config_t) <= IPC_RPC_BYTES, "Configuration exceeds IPC payload");
error_t ecu_comm_uds_get_default_config(ecu_comm_uds_t instance, uds_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_UDS_MAX || !config) return E_PARAM;
  memcpy(config, &ecu_comm_uds_config_default, sizeof(*config));
  config->enabled = ecu_comm_uds_enabled_default[instance];
  return E_OK;
}
error_t ecu_comm_uds_configure(ecu_comm_uds_t instance, const uds_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_UDS_MAX || !config) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_UDS, instance, COMM_IPC_CONFIGURE), config, sizeof(*config), NULL, NULL);
}
error_t ecu_comm_uds_reset(ecu_comm_uds_t instance)
{
  if((unsigned)instance >= ECU_COMM_UDS_MAX) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_UDS, instance, COMM_IPC_RESET), NULL, 0, NULL, NULL);
}
