#include "comm_ipc.h"
#include "config_common.h"
#include "time.h"

#define PARAMETER_STALE_US 100000u
#define PARAMETER_POLL_BUDGET 32u
#if defined(CORE_CM4)
static ecu_core_runtime_value_ctx_t mirror[IPC_PARAMETER_MAX];
static bool dirty[IPC_PARAMETER_MAX];
static bool written[IPC_PARAMETER_MAX];
static bool received[IPC_PARAMETER_MAX];
static uint32_t updated[IPC_PARAMETER_MAX];
static uint32_t cursor;

error_t ecu_config_common_get_parameter_ptr_by_id(ecu_config_parameter_id_t id,
    ecu_core_runtime_value_ctx_t **parameter)
{
  if(!parameter || !id.bitfield.supported || id.bitfield.entity >= ECU_COMMON_ENTITY_MAX) return E_PARAM;
  uint32_t count = ipc_load(&g_ipc.parameter_count);
  if(count > IPC_PARAMETER_MAX) return E_FAULT;
  for(uint32_t i = 0; i < count; ++i) {
    if(g_ipc.parameters[i].id == id.id) {
      if(!ipc_load(&g_ipc.parameters[i].resolved)) return E_AGAIN;
      error_t err = (error_t)g_ipc.parameters[i].resolve_result;
      if(err == E_OK && !received[i]) return E_AGAIN;
      if(err == E_OK) *parameter = &mirror[i];
      return err;
    }
  }
  if(count == IPC_PARAMETER_MAX) return E_OVERFLOW;
  /* Only CM4's foreground registers subscriptions; publish id before count. */
  g_ipc.parameters[count].id = id.id;
  mirror[count].valid = false;
  updated[count] = time_now_us();
  *parameter = &mirror[count];
  ipc_store(&g_ipc.parameter_count, count + 1u);
  return E_AGAIN;
}
error_t ecu_config_common_get_parameter_value_by_id(ecu_config_parameter_id_t id,
    ecu_core_runtime_value_ctx_t *parameter)
{
  ecu_core_runtime_value_ctx_t *value;
  if(!parameter) return E_PARAM;
  error_t err = ecu_config_common_get_parameter_ptr_by_id(id, &value);
  if(err == E_OK) {
    if(!value->valid) return E_NOTRDY;
    *parameter = *value;
  }
  return err;
}
error_t comm_ipc_parameter_write(ecu_core_runtime_value_ctx_t *parameter)
{
  /* No pointer travels through IPC. Convert the local stable address to an id. */
  uint32_t count = ipc_load(&g_ipc.parameter_count);
  if(count > IPC_PARAMETER_MAX) return E_FAULT;
  for(uint32_t i = 0; i < count; ++i) {
    if(parameter == &mirror[i]) { dirty[i] = true; written[i] = true; return E_OK; }
  }
  return E_PARAM;
}
void comm_ipc_parameters_poll(void)
{
  uint32_t count = ipc_load(&g_ipc.parameter_count);
  if(count > IPC_PARAMETER_MAX) return;
  uint32_t now = time_now_us();
  for(uint32_t n = 0; n < PARAMETER_POLL_BUDGET && n < count; ++n) {
    if(cursor >= count) cursor = 0;
    uint32_t i = cursor++;
    ipc_parameter_t *item = &g_ipc.parameters[i];
    float value; uint32_t valid, timestamp;
    if(ipc_value_consume(&item->from_cm7, &value, &valid, &timestamp) == E_OK) {
      received[i] = true;
      /* CM4 owns incoming/write parameters; never overwrite pending CAN data. */
      if(!written[i]) {
        mirror[i].value = value;
        mirror[i].valid = valid != 0;
      }
      updated[i] = timestamp;
    }
    if(!written[i] && time_diff(now, updated[i]) > PARAMETER_STALE_US) mirror[i].valid = false;
    if(dirty[i] && ipc_value_publish(&item->from_cm4, mirror[i].value,
        mirror[i].valid, now) == E_OK) dirty[i] = false;
  }
}
#else
static uint32_t cursor;
void comm_ipc_parameters_poll(void)
{
  uint32_t now = time_now_us();
  ipc_store(&g_ipc.cm7_heartbeat, now);
  uint32_t count = ipc_load(&g_ipc.parameter_count);
  if(count > IPC_PARAMETER_MAX || !ipc_load(&g_ipc.cm4_ready)) return;
  for(uint32_t n = 0; n < PARAMETER_POLL_BUDGET && n < count; ++n) {
    if(cursor >= count) cursor = 0;
    ipc_parameter_t *item = &g_ipc.parameters[cursor++];
    ecu_config_parameter_id_t id = { .id = item->id };
    ecu_core_runtime_value_ctx_t *parameter;
    error_t err = ecu_config_common_get_parameter_ptr_by_id(id, &parameter);
    if(!ipc_load(&item->resolved)) {
      item->resolve_result = (int32_t)err;
      ipc_store(&item->resolved, 1u);
    }
    if(err != E_OK) {
      /* Unsupported ids are published as invalid; they never expose memory. */
      (void)ipc_value_publish(&item->from_cm7, 0.0f, false, now);
      float unused; uint32_t valid, timestamp;
      (void)ipc_value_consume(&item->from_cm4, &unused, &valid, &timestamp);
      continue;
    }
    float value; uint32_t valid, timestamp;
    if(ipc_value_consume(&item->from_cm4, &value, &valid, &timestamp) == E_OK) {
      /* The configured parameter id is the same authority used by the original
       * Signals implementation. Apply valid and invalid writes alike. */
      if(time_diff(now, timestamp) <= PARAMETER_STALE_US) {
        uint32_t prim = EnterCritical();
        parameter->value = value;
        parameter->valid = valid != 0;
        ExitCritical(prim);
      }
    }
    uint32_t prim = EnterCritical();
    ecu_core_runtime_value_ctx_t snapshot = *parameter;
    ExitCritical(prim);
    (void)ipc_value_publish(&item->from_cm7, snapshot.value, snapshot.valid, now);
  }
}
#endif
