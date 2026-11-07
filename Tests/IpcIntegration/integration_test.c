#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include "ecu_ipc.h"

#define DECLARE(core) \
  void cm##core##_middlelayer_ipc_init(void); \
  void cm##core##_middlelayer_ipc_start(void); \
  void cm##core##_middlelayer_ipc_loop_comm(void); \
  error_t cm##core##_ecu_ipc_buffer_send(ecu_ipc_buffer_t, const void *, uint32_t); \
  error_t cm##core##_ecu_ipc_buffer_receive(ecu_ipc_buffer_t, void *, uint32_t *); \
  error_t cm##core##_ecu_ipc_transfer_send(ecu_ipc_transfer_t, uint32_t, const void *, uint32_t); \
  error_t cm##core##_ecu_ipc_transfer_receive(ecu_ipc_transfer_t, const void **, uint32_t *, uint32_t *);
DECLARE(4)
DECLARE(7)

static void ticks(void)
{
  cm4_middlelayer_ipc_loop_comm();
  cm7_middlelayer_ipc_loop_comm();
}

static void deterministic(void)
{
  unsigned char input[1024], output[1024];
  memset(input, 0x42, sizeof(input));
  memset(output, 0, sizeof(output));
  ecu_ipc_buffer_t b = ECU_IPC_BUFFER_CM7_TO_CM4;
  ecu_ipc_transfer_t t = ECU_IPC_TRANSFER_CM7_TO_CM4;
  uint32_t size = 8, endpoint = 0;
  const void *view = NULL;

  assert(cm7_ecu_ipc_buffer_send(b, input, 1) == E_NOTRDY);
  cm7_middlelayer_ipc_init();
  cm4_middlelayer_ipc_init();
  cm4_middlelayer_ipc_start();
  cm7_middlelayer_ipc_start();

  assert(cm4_ecu_ipc_buffer_send(b, input, 1) == E_INVALACT);
  assert(cm7_ecu_ipc_transfer_receive(t, &view, &size, &endpoint) == E_INVALACT);
  assert(cm7_ecu_ipc_buffer_send(ECU_IPC_BUFFER_MAX, input, 1) == E_PARAM);
  assert(cm7_ecu_ipc_buffer_send(b, input, 1024) == E_OVERFLOW);
  assert(cm7_ecu_ipc_transfer_send(t, 1, input, 1025) == E_OVERFLOW);
  assert(cm7_ecu_ipc_buffer_send(b, input, 1023) == E_AGAIN);
  assert(cm7_ecu_ipc_buffer_send(b, input + 1, 1023) == E_BUSY);
  ticks();
  assert(cm7_ecu_ipc_buffer_send(b, input, 1023) == E_OK);
  assert(cm7_ecu_ipc_buffer_send(b, input, 1023) == E_AGAIN);
  ticks();
  assert(cm7_ecu_ipc_buffer_send(b, input, 1023) == E_AGAIN);
  for(unsigned i = 0; i < 2; ++i) {
    assert(cm4_ecu_ipc_buffer_receive(b, output, &size) == E_AGAIN);
    ticks();
    assert(cm4_ecu_ipc_buffer_receive(b, output, &size) == E_OVERFLOW);
    assert(size == 8 && output[0] == 0);
  }
  size = sizeof(output);
  assert(cm4_ecu_ipc_buffer_receive(b, output, &size) == E_AGAIN);
  ticks();
  assert(cm4_ecu_ipc_buffer_receive(b, output, &size) == E_OK && size == 1023);
  assert(memcmp(input, output, size) == 0);
  assert(cm7_ecu_ipc_buffer_send(b, input, 1023) == E_OK);
  size = sizeof(output);
  assert(cm4_ecu_ipc_buffer_receive(b, output, &size) == E_AGAIN);
  ticks();
  assert(cm4_ecu_ipc_buffer_receive(b, output, &size) == E_OK && size == 1023);
  assert(memcmp(input, output, size) == 0);

  assert(cm7_ecu_ipc_transfer_send(t, 81, input, sizeof(input)) == E_AGAIN);
  for(unsigned i = 0; i < 16; ++i) ticks();
  assert(cm7_ecu_ipc_transfer_send(t, 81, input, sizeof(input)) == E_AGAIN);
  assert(cm4_ecu_ipc_transfer_receive(t, &view, &size, &endpoint) == E_AGAIN);
  for(unsigned i = 0; i < 8; ++i) ticks();
  assert(cm7_ecu_ipc_transfer_send(t, 81, input, sizeof(input)) == E_OK);
  assert(cm4_ecu_ipc_transfer_receive(t, &view, &size, &endpoint) == E_OK);
  assert(size == sizeof(input) && endpoint == 81 && memcmp(view, input, size) == 0);

  memset(input, 0x17, sizeof(input));
  assert(cm7_ecu_ipc_transfer_send(t, 82, input, sizeof(input)) == E_AGAIN);
  for(unsigned i = 0; i < 16; ++i) ticks();
  assert(((const unsigned char *)view)[0] == 0x42);
  assert(cm4_ecu_ipc_transfer_receive(t, &view, &size, &endpoint) == E_AGAIN);
  for(unsigned i = 0; i < 8; ++i) ticks();
  assert(cm7_ecu_ipc_transfer_send(t, 82, input, sizeof(input)) == E_OK);
  assert(cm4_ecu_ipc_transfer_receive(t, &view, &size, &endpoint) == E_OK);
  assert(size == sizeof(input) && endpoint == 82 && memcmp(view, input, size) == 0);
}

static int stop;
static void *comm(void *arg)
{
  void (*const *tick)(void) = arg;
  while(!__atomic_load_n(&stop, __ATOMIC_RELAXED)) {
    (*tick)();
    sched_yield();
  }
  return NULL;
}

#define ROUNDS 5000u
static void threaded(void)
{
  pthread_t tx, rx;
  void (*const t4)(void) = cm4_middlelayer_ipc_loop_comm;
  void (*const t7)(void) = cm7_middlelayer_ipc_loop_comm;
  assert(pthread_create(&tx, NULL, comm, (void *)&t4) == 0);
  assert(pthread_create(&rx, NULL, comm, (void *)&t7) == 0);

  for(uint32_t i = 0; i < ROUNDS; ++i) {
    uint32_t a = i, b = ~i, a_rx = 0, b_rx = 0;
    uint32_t sa = sizeof(a_rx), sb = sizeof(b_rx), ta = 0, tb = 0, ea = 0, eb = 0;
    const void *va = NULL, *vb = NULL;
    error_t result[8] = {E_AGAIN, E_AGAIN, E_AGAIN, E_AGAIN, E_AGAIN, E_AGAIN, E_AGAIN, E_AGAIN};
    do {
      if(result[0] == E_AGAIN) result[0] = cm4_ecu_ipc_buffer_send(ECU_IPC_BUFFER_CM4_TO_CM7, &a, sizeof(a));
      if(result[1] == E_AGAIN) result[1] = cm7_ecu_ipc_buffer_send(ECU_IPC_BUFFER_CM7_TO_CM4, &b, sizeof(b));
      if(result[2] == E_AGAIN) result[2] = cm7_ecu_ipc_buffer_receive(ECU_IPC_BUFFER_CM4_TO_CM7, &a_rx, &sa);
      if(result[3] == E_AGAIN) result[3] = cm4_ecu_ipc_buffer_receive(ECU_IPC_BUFFER_CM7_TO_CM4, &b_rx, &sb);
      if(result[4] == E_AGAIN) result[4] = cm4_ecu_ipc_transfer_send(ECU_IPC_TRANSFER_CM4_TO_CM7, i, &a, sizeof(a));
      if(result[5] == E_AGAIN) result[5] = cm7_ecu_ipc_transfer_send(ECU_IPC_TRANSFER_CM7_TO_CM4, ~i, &b, sizeof(b));
      if(result[6] == E_AGAIN) result[6] = cm7_ecu_ipc_transfer_receive(ECU_IPC_TRANSFER_CM4_TO_CM7, &va, &ta, &ea);
      if(result[7] == E_AGAIN) result[7] = cm4_ecu_ipc_transfer_receive(ECU_IPC_TRANSFER_CM7_TO_CM4, &vb, &tb, &eb);
      for(unsigned j = 0; j < 8; ++j) assert(result[j] == E_OK || result[j] == E_AGAIN);
      sched_yield();
    } while(result[0] || result[1] || result[2] || result[3] || result[4] || result[5] || result[6] || result[7]);
    assert(sa == 4 && sb == 4 && a_rx == a && b_rx == b);
    assert(ta == 4 && tb == 4 && ea == i && eb == ~i);
    assert(memcmp(&a, va, ta) == 0 && memcmp(&b, vb, tb) == 0);
  }
  __atomic_store_n(&stop, 1, __ATOMIC_RELAXED);
  assert(pthread_join(tx, NULL) == 0);
  assert(pthread_join(rx, NULL) == 0);
}

int main(void)
{
  deterministic();
  threaded();
  printf("PASS: startup, short buffer retry, full/wrap, transfer view lifetime and %u bidirectional messages across four channels\n", 4 * ROUNDS);
  return 0;
}
