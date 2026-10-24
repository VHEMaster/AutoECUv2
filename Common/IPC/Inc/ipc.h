#ifndef AUTOECU_IPC_H
#define AUTOECU_IPC_H
#include <stdint.h>
#include <stddef.h>
#include "errors.h"

#define IPC_ABI 0x45435502u
#define IPC_RPC_SLOTS 2u
#define IPC_RPC_BYTES 16384u
#define IPC_PARAMETER_MAX 512u
#define IPC_SHARED_BYTES (96u * 1024u)

typedef enum { IPC_FREE, IPC_BUILDING, IPC_REQUEST, IPC_REPLY } ipc_state_t;
typedef struct {
  uint32_t state;
  uint32_t key;
  uint32_t length;
  int32_t result;
  uint8_t payload[IPC_RPC_BYTES] __attribute__((aligned(32)));
} ipc_rpc_slot_t;
/* A one-element SPSC mailbox: producer writes only while pending == 0;
 * consumer acknowledges only after copying the entire value. */
typedef struct {
  uint32_t pending;
  uint32_t timestamp;
  float value;
  uint32_t valid;
} ipc_value_t;
typedef struct {
  uint32_t id;
  uint32_t resolved;
  int32_t resolve_result;
  ipc_value_t from_cm7;
  ipc_value_t from_cm4;
} ipc_parameter_t;
typedef struct {
  uint32_t abi;
  uint32_t cm7_ready;
  uint32_t cm4_ready;
  uint32_t parameter_count;
  uint32_t cm7_heartbeat;
  uint32_t cm4_heartbeat;
  uint32_t parameter_errors;
  uint32_t reserved;
  ipc_rpc_slot_t requests[2][IPC_RPC_SLOTS];
  ipc_parameter_t parameters[IPC_PARAMETER_MAX];
} ipc_shared_t;
_Static_assert(sizeof(ipc_shared_t) <= IPC_SHARED_BYTES, "IPC exceeds reserved SRAM");
extern ipc_shared_t g_ipc;

/* Called by CM7 before releasing CM4, never while CM4 is running. */
void ipc_initialize(void);
/* Acquire/release publication on noncacheable shared Normal memory. */
uint32_t ipc_load(const uint32_t *value);
void ipc_store(uint32_t *value, uint32_t data);
/* Each key identifies one outstanding operation. Retry with unchanged input
 * until a terminal result is returned. E_AGAIN never means completion. Calls
 * copy their arguments; no caller pointers or callbacks enter shared memory.
 * Full queues or a preempted producer return E_AGAIN without accepting the
 * operation. One logical caller owns each key until it collects its reply. */
error_t ipc_rpc_call(unsigned int core, uint32_t key, const void *request,
    uint32_t length, void *response, uint32_t *response_length);
typedef error_t (*ipc_rpc_handler_t)(uint32_t key, void *payload, uint32_t *length);
void ipc_rpc_service(unsigned int core, ipc_rpc_handler_t handler);
/* Explicit bounded backpressure for value mailboxes. */
error_t ipc_value_publish(ipc_value_t *mailbox, float value, uint32_t valid, uint32_t timestamp);
error_t ipc_value_consume(ipc_value_t *mailbox, float *value, uint32_t *valid, uint32_t *timestamp);
#endif
