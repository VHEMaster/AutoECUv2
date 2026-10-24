#include "comm_ipc.h"
#include "config_can.h"
#include "config_kwp.h"
#include "config_isotp.h"
#include "config_uds.h"
#include "config_obd2.h"
#include "config_router.h"
#include "config_signals.h"

error_t comm_ipc_dispatch(uint32_t key, void *payload, uint32_t *length)
{
  uint32_t type = (key & 255u) - 1u;
  uint32_t instance = (key >> 8) & 255u;
  uint32_t method = key >> 16;
  error_t err = E_PARAM;
  /* These routines previously ran above FDCAN's IRQ priority on CM7.
   * Keep callback tables and protocol contexts coherent during foreground
   * reconfiguration; SysTick priority 0 remains available to HAL timeouts. */
  uint32_t saved_basepri = __get_BASEPRI();
  __set_BASEPRI_MAX(8u << (8u - __NVIC_PRIO_BITS));
  __DSB();
  __ISB();
  switch(type) {
    case ECU_COMM_TYPE_CAN:
      if(instance >= ECU_COMM_CAN_MAX) break;
      if(method == COMM_IPC_CONFIGURE && *length == sizeof(can_config_t))
        err = ecu_comm_can_configure(instance, (const can_config_t *)payload);
      else if(method == COMM_IPC_RESET && *length == 0)
        err = ecu_comm_can_reset(instance);
      break;
    case ECU_COMM_TYPE_KWP:
      if(instance >= ECU_COMM_KWP_MAX) break;
      if(method == COMM_IPC_CONFIGURE && *length == sizeof(kwp_config_t))
        err = ecu_comm_kwp_configure(instance, (const kwp_config_t *)payload);
      else if(method == COMM_IPC_RESET && *length == 0)
        err = ecu_comm_kwp_reset(instance);
      break;
    case ECU_COMM_TYPE_ISOTP:
      if(instance >= ECU_COMM_ISOTP_MAX) break;
      if(method == COMM_IPC_CONFIGURE && *length == sizeof(isotp_config_t))
        err = ecu_comm_isotp_configure(instance, (const isotp_config_t *)payload);
      else if(method == COMM_IPC_RESET && *length == 0)
        err = ecu_comm_isotp_reset(instance);
      break;
    case ECU_COMM_TYPE_UDS:
      if(instance >= ECU_COMM_UDS_MAX) break;
      if(method == COMM_IPC_CONFIGURE && *length == sizeof(uds_config_t))
        err = ecu_comm_uds_configure(instance, (const uds_config_t *)payload);
      else if(method == COMM_IPC_RESET && *length == 0)
        err = ecu_comm_uds_reset(instance);
      break;
    case ECU_COMM_TYPE_OBD2:
      if(instance >= ECU_COMM_OBD2_MAX) break;
      if(method == COMM_IPC_CONFIGURE && *length == sizeof(obd2_config_t))
        err = ecu_comm_obd2_configure(instance, (const obd2_config_t *)payload);
      else if(method == COMM_IPC_RESET && *length == 0)
        err = ecu_comm_obd2_reset(instance);
      break;
    case ECU_COMM_TYPE_ROUTER:
      if(instance >= ECU_COMM_ROUTER_MAX) break;
      if(method == COMM_IPC_CONFIGURE && *length == sizeof(router_config_t))
        err = ecu_comm_router_configure(instance, (const router_config_t *)payload);
      else if(method == COMM_IPC_RESET && *length == 0)
        err = ecu_comm_router_reset(instance);
      break;
    case ECU_COMM_TYPE_SIGNALS:
      if(instance >= ECU_COMM_SIGNALS_MAX) break;
      if(method == COMM_IPC_CONFIGURE && *length == sizeof(signals_config_t))
        err = ecu_comm_signals_configure(instance, (const signals_config_t *)payload);
      else if(method == COMM_IPC_RESET && *length == 0)
        err = ecu_comm_signals_reset(instance);
      break;
    default: break;
  }
  if(method == COMM_IPC_RESET && err == E_OK) {
    ecu_comm_loop_comm();
    ecu_comm_loop_main();
  }
  __set_BASEPRI(saved_basepri);
  __DSB();
  __ISB();
  *length = 0;
  return err;
}
