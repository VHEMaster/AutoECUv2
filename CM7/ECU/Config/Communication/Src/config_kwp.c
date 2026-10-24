/* CM7 owns persistent/default configuration. Runtime lives on CM4. */
#include "config_kwp.h"
#include "config_global.h"
#include "comm_ipc.h"
static const kwp_config_t ecu_comm_kwp_default_config[ECU_COMM_KWP_MAX] = {
  {

  }, //ECU_KWP_IF_1
};
static const bool ecu_comm_kwp_enabled_default[ECU_COMM_KWP_MAX] = {
    true
};
_Static_assert(sizeof(kwp_config_t) <= IPC_RPC_BYTES, "Configuration exceeds IPC payload");
error_t ecu_comm_kwp_get_default_config(ecu_comm_kwp_t instance, kwp_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_KWP_MAX || !config) return E_PARAM;
  memcpy(config, &ecu_comm_kwp_default_config[instance], sizeof(*config));
  config->enabled = ecu_comm_kwp_enabled_default[instance];
  return E_OK;
}
error_t ecu_comm_kwp_configure(ecu_comm_kwp_t instance, const kwp_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_KWP_MAX || !config) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_KWP, instance, COMM_IPC_CONFIGURE), config, sizeof(*config), NULL, NULL);
}
error_t ecu_comm_kwp_reset(ecu_comm_kwp_t instance)
{
  if((unsigned)instance >= ECU_COMM_KWP_MAX) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_KWP, instance, COMM_IPC_RESET), NULL, 0, NULL, NULL);
}
