/* CM7 owns persistent/default configuration. Runtime lives on CM4. */
#include "config_isotp.h"
#include "config_global.h"
#include "comm_ipc.h"
static const isotp_config_t ecu_comm_isotp_config_default = {
    .timeout = 1000 * TIME_US_IN_MS,
    .upstream_block_size = 20,
    .upstream_min_separation_time = 0,
    .upstream_separation_time_gap = 200,
    .downstream_pass_early_ovf = false,
};
static const bool ecu_comm_isotp_enabled_default[ECU_COMM_ISOTP_MAX] = {
    true,
    true
};
_Static_assert(sizeof(isotp_config_t) <= IPC_RPC_BYTES, "Configuration exceeds IPC payload");
error_t ecu_comm_isotp_get_default_config(ecu_comm_isotp_t instance, isotp_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_ISOTP_MAX || !config) return E_PARAM;
  memcpy(config, &ecu_comm_isotp_config_default, sizeof(*config));
  config->enabled = ecu_comm_isotp_enabled_default[instance];
  return E_OK;
}
error_t ecu_comm_isotp_configure(ecu_comm_isotp_t instance, const isotp_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_ISOTP_MAX || !config) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_ISOTP, instance, COMM_IPC_CONFIGURE), config, sizeof(*config), NULL, NULL);
}
error_t ecu_comm_isotp_reset(ecu_comm_isotp_t instance)
{
  if((unsigned)instance >= ECU_COMM_ISOTP_MAX) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_ISOTP, instance, COMM_IPC_RESET), NULL, 0, NULL, NULL);
}
