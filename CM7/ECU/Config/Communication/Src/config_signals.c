/* CM7 owns persistent/default configuration. Runtime lives on CM4. */
#include "config_signals.h"
#include "config_global.h"
#include "comm_ipc.h"
static const signals_config_t ecu_comm_signals_default_config[ECU_COMM_SIGNALS_MAX] = {
  {
      .messages_lists = {
          .upstream_list = {
              {

              },
          },
          .downstream_list = {
              {
                  .enabled = true,
                  .message_id = 0x100,
                  .message_len = 8u,
                  .upstream_timeout = 500 * TIME_US_IN_MS,
                  .downstream_periodicity = 100 * TIME_US_IN_MS,
                  .has_counter = true,
                  .has_crc = true,
                  .counter_mask = 0x0F,
                  .crc_poly = 0x1D,
                  .counter_byte_pos = 6,
                  .crc_byte_pos = 7,
              },
          },
      },
      .signals_lists = {
          .upstream_list = {
              {

              },
          },
          .downstream_list = {
              {
                  .enabled = true,
                  .message_id = 0x100,
                  .description = {
                      .byteorder = CAN_SIGNAL_BYTEORDER_INTEL,
                      .type = CAN_SIGNAL_TYPE_UFLT,
                      .bitlength = 8,
                      .startbit = 0,
                      .offset = 0,
                      .gain = 40.0f,
                  },
                  .parameter_id = { .bitfield = {
                      .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                      .type = ECU_SENSOR_TYPE_CKP, .instance = ECU_SENSOR_CKP_1,
                      .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_CKP_READ_PARAM_RPM,
                  }},
              },
              {
                  .enabled = true,
                  .message_id = 0x100,
                  .description = {
                      .byteorder = CAN_SIGNAL_BYTEORDER_INTEL,
                      .type = CAN_SIGNAL_TYPE_UFLT,
                      .bitlength = 8,
                      .startbit = 8,
                      .offset = 0,
                      .gain = 0.012f,
                  },
                  .parameter_id = { .bitfield = {
                      .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                      .type = ECU_SENSOR_TYPE_MAP, .instance = ECU_SENSOR_MAP_1,
                      .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_MAP_READ_PARAM_PRESSURE,
                  }},
              },
              {
                  .enabled = true,
                  .message_id = 0x100,
                  .description = {
                      .byteorder = CAN_SIGNAL_BYTEORDER_INTEL,
                      .type = CAN_SIGNAL_TYPE_UFLT,
                      .bitlength = 8,
                      .startbit = 16,
                      .offset = 0,
                      .gain = 0.4f,
                  },
                  .parameter_id = { .bitfield = {
                      .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                      .type = ECU_SENSOR_TYPE_TPS, .instance = ECU_SENSOR_TPS_1,
                      .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_TPS_READ_PARAM_THROTTLE_POSITION,
                  }},
              },
          },
      },
  }, //ECU_COMM_SIGNALS_1
};
static const bool ecu_comm_signals_enabled_default[ECU_COMM_SIGNALS_MAX] = {
    true,
};
_Static_assert(sizeof(signals_config_t) <= IPC_RPC_BYTES, "Configuration exceeds IPC payload");
error_t ecu_comm_signals_get_default_config(ecu_comm_signals_t instance, signals_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_SIGNALS_MAX || !config) return E_PARAM;
  memcpy(config, &ecu_comm_signals_default_config[instance], sizeof(*config));
  config->enabled = ecu_comm_signals_enabled_default[instance];
  return E_OK;
}
error_t ecu_comm_signals_configure(ecu_comm_signals_t instance, const signals_config_t *config)
{
  if((unsigned)instance >= ECU_COMM_SIGNALS_MAX || !config) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_SIGNALS, instance, COMM_IPC_CONFIGURE), config, sizeof(*config), NULL, NULL);
}
error_t ecu_comm_signals_reset(ecu_comm_signals_t instance)
{
  if((unsigned)instance >= ECU_COMM_SIGNALS_MAX) return E_PARAM;
  return ipc_rpc_call(0, COMM_IPC_KEY(ECU_COMM_TYPE_SIGNALS, instance, COMM_IPC_RESET), NULL, 0, NULL, NULL);
}
