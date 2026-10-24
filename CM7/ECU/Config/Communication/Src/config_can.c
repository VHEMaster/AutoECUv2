/* CM7 owns persistent/default configuration. Runtime lives on CM4. */
#include "config_can.h"
#include "config_global.h"
#include "comm_ipc.h"
static const can_config_t ecu_comm_can_default_config[ECU_COMM_CAN_MAX] = {
  {
    .baudrate = CAN_BAUDRATE_500KBPS,
    .global_filter = {
        .reject_remote_std = true,
        .reject_remote_ext = true,
        .reject_non_matching_std = true,
        .reject_non_matching_ext = true,
        .non_matching_std_fifo = CAN_FIFO_0,
        .non_matching_ext_fifo = CAN_FIFO_0,
    },
    .filter_config = {
      {
        .id_type = FDCAN_STANDARD_ID,
        .filter_type = FDCAN_FILTER_DUAL,
        .filter_config = FDCAN_FILTER_TO_RXFIFO0,
        .id1_filter = 0x7DF,
        .id2_mask = 0x7E0,
        .rx_buffer_index = 0,
      },
    },
  }, //ECU_CAN_IF_1
};
static const bool ecu_comm_can_enabled_default[ECU_COMM_CAN_MAX] = {
    true,
};
_Static_assert(sizeof(can_config_t) <= IPC_RPC_BYTES, "Configuration exceeds IPC payload");
error_t ecu_comm_can_get_default_config(ecu_comm_can_t instance, can_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_CAN_MAX || !config) return E_PARAM;
  memcpy(config, &ecu_comm_can_default_config[instance], sizeof(*config));
  config->enabled = ecu_comm_can_enabled_default[instance];
  return E_OK;
}
error_t ecu_comm_can_configure(ecu_comm_can_t instance, const can_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_CAN_MAX || !config) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_CAN, instance, COMM_IPC_CONFIGURE), config, sizeof(*config), NULL, NULL);
}
error_t ecu_comm_can_reset(ecu_comm_can_t instance)
{
  if((unsigned)instance >= ECU_COMM_CAN_MAX) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_CAN, instance, COMM_IPC_RESET), NULL, 0, NULL, NULL);
}
