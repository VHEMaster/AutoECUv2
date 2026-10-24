#include "ipc.h"
#include <string.h>

ipc_shared_t g_ipc __attribute__((section(".ipc_shared"), aligned(32)));

uint32_t ipc_load(const uint32_t *value)
{
  return __atomic_load_n(value, __ATOMIC_ACQUIRE);
}
void ipc_store(uint32_t *value, uint32_t data)
{
  __atomic_store_n(value, data, __ATOMIC_RELEASE);
}
void ipc_initialize(void)
{
  memset(&g_ipc, 0, sizeof(g_ipc));
  ipc_store(&g_ipc.abi, IPC_ABI);
  ipc_store(&g_ipc.cm7_ready, 1u);
}

static error_t ipc_rpc_call_locked(unsigned int core, uint32_t key, const void *request,
    uint32_t length, void *response, uint32_t *response_length)
{
  ipc_rpc_slot_t *free_slot = NULL;
  if(core > 1u || !key || length > IPC_RPC_BYTES || (length && !request)) return E_PARAM;
  if(ipc_load(&g_ipc.abi) != IPC_ABI ||
      !ipc_load(core ? &g_ipc.cm7_ready : &g_ipc.cm4_ready)) return E_AGAIN;
  for(unsigned int i = 0; i < IPC_RPC_SLOTS; ++i) {
    ipc_rpc_slot_t *slot = &g_ipc.requests[core][i];
    uint32_t state = ipc_load(&slot->state);
    if(state == IPC_FREE) { free_slot = slot; continue; }
    if(slot->key != key) continue;
    if(state != IPC_REPLY) return E_AGAIN;
    error_t result = (error_t)slot->result;
    if(slot->length > IPC_RPC_BYTES) result = E_FAULT;
    else if(response_length) {
      if(*response_length < slot->length || (slot->length && !response)) result = E_OVERFLOW;
      else if(slot->length) memcpy(response, slot->payload, slot->length);
      *response_length = slot->length;
    }
    ipc_store(&slot->state, IPC_FREE);
    return result;
  }
  if(!free_slot) return E_AGAIN;
  /* The local nonblocking producer guard serializes slot reservations. */
  free_slot->key = key;
  ipc_store(&free_slot->state, IPC_BUILDING);
  free_slot->length = length;
  if(length) memcpy(free_slot->payload, request, length);
  ipc_store(&free_slot->state, IPC_REQUEST);
  return E_AGAIN;
}

/* A preempting caller observes busy and retries; no interrupt masking and no
 * lock waiting is needed even while copying a full configuration object. */
static uint32_t client_busy[2];
error_t ipc_rpc_call(unsigned int core, uint32_t key, const void *request,
    uint32_t length, void *response, uint32_t *response_length)
{
  if(core > 1u) return E_PARAM;
  if(__atomic_exchange_n(&client_busy[core], 1u, __ATOMIC_ACQUIRE)) return E_AGAIN;
  error_t result = ipc_rpc_call_locked(core, key, request, length, response, response_length);
  __atomic_store_n(&client_busy[core], 0u, __ATOMIC_RELEASE);
  return result;
}

void ipc_rpc_service(unsigned int core, ipc_rpc_handler_t handler)
{
  if(core > 1u || !handler || ipc_load(&g_ipc.abi) != IPC_ABI) return;
  for(unsigned int i = 0; i < IPC_RPC_SLOTS; ++i) {
    ipc_rpc_slot_t *slot = &g_ipc.requests[1u - core][i];
    if(ipc_load(&slot->state) != IPC_REQUEST) continue;
    uint32_t length = slot->length;
    error_t result = length <= IPC_RPC_BYTES ? handler(slot->key, slot->payload, &length) : E_PARAM;
    if(result == E_AGAIN) continue;
    if(length > IPC_RPC_BYTES) { result = E_OVERFLOW; length = 0; }
    slot->length = length;
    slot->result = (int32_t)result;
    ipc_store(&slot->state, IPC_REPLY);
  }
}

error_t ipc_value_publish(ipc_value_t *mailbox, float value, uint32_t valid, uint32_t timestamp)
{
  if(ipc_load(&mailbox->pending)) return E_AGAIN;
  mailbox->value = value;
  mailbox->valid = valid;
  mailbox->timestamp = timestamp;
  ipc_store(&mailbox->pending, 1u);
  return E_OK;
}
error_t ipc_value_peek(ipc_value_t *mailbox, float *value, uint32_t *valid, uint32_t *timestamp)
{
  if(!ipc_load(&mailbox->pending)) return E_AGAIN;
  *value = mailbox->value;
  *valid = mailbox->valid;
  *timestamp = mailbox->timestamp;
  return E_OK;
}
error_t ipc_value_consume(ipc_value_t *mailbox, float *value, uint32_t *valid, uint32_t *timestamp)
{
  error_t err = ipc_value_peek(mailbox, value, valid, timestamp);
  if(err == E_OK) ipc_store(&mailbox->pending, 0u);
  return err;
}
