/* CM7 owns persistent/default configuration. Runtime lives on CM4. */
#include "config_router.h"
#include "config_global.h"
#include "comm_ipc.h"
static const router_config_t ecu_comm_router_default_config[ECU_COMM_ROUTER_MAX] = {
  {
      .can = {
          .signals = {
              .upstream_list = {

              },
              .downstream_list = {
                  {
                      .enabled = false,
                      .msg_id_1_start = 0x100,
                      .msg_id_2_end = 0x1FF,
                      .can_instance = ECU_COMM_CAN_1,
                      .signals_instance = ECU_COMM_SIGNALS_1,
                  },
              },
          },
      },
      .diagnostics = {
          .can_isotp = {
              {
                  .enabled = true,
                  .can_instance = ECU_COMM_CAN_1,
                  .uds_instance = ECU_COMM_UDS_1,
                  .obd2_instance = ECU_COMM_OBD2_1,
                  .upstream_phy_msg_id = 0x7E0,
                  .upstream_func_msg_id = 0x7DF,
                  .downstream_msg_id = 0x7E8,
              }, //ECU_COMM_ISOTP_1
              {

              }, //ECU_COMM_ISOTP_2
          },
          .kwp = {
              {
                  .enabled = true,
                  .uds_instance = ECU_COMM_UDS_2,
                  .obd2_instance = ECU_COMM_OBD2_2,
              }, //ECU_COMM_KWP_1
          },
      },
  }, //ECU_COMM_ROUTER_1
};
static const bool ecu_comm_router_enabled_default[ECU_COMM_ROUTER_MAX] = {
    true,
};
_Static_assert(sizeof(router_config_t) <= IPC_RPC_BYTES, "Configuration exceeds IPC payload");
error_t ecu_comm_router_get_default_config(ecu_comm_router_t instance, router_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_ROUTER_MAX || !config) return E_PARAM;
  memcpy(config, &ecu_comm_router_default_config[instance], sizeof(*config));
  config->enabled = ecu_comm_router_enabled_default[instance];
  return E_OK;
}
error_t ecu_comm_router_configure(ecu_comm_router_t instance, const router_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_ROUTER_MAX || !config) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_ROUTER, instance, COMM_IPC_CONFIGURE), config, sizeof(*config), NULL, NULL);
}
error_t ecu_comm_router_reset(ecu_comm_router_t instance)
{
  if((unsigned)instance >= ECU_COMM_ROUTER_MAX) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_ROUTER, instance, COMM_IPC_RESET), NULL, 0, NULL, NULL);
}
