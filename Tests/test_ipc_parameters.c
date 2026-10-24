#include "comm_ipc.h"
#include "config_common.h"
#include <assert.h>
#include <stdio.h>
uint32_t test_now;
static ecu_core_runtime_value_ctx_t engine = {.value = 1234, .valid = true};
void cm4_poll(void);
void cm7_poll(void);
error_t cm4_write(ecu_core_runtime_value_ctx_t *);
error_t cm4_get_ptr(ecu_config_parameter_id_t, ecu_core_runtime_value_ctx_t **);
error_t cm4_get_value(ecu_config_parameter_id_t, ecu_core_runtime_value_ctx_t *);
error_t engine_get_ptr(ecu_config_parameter_id_t id, ecu_core_runtime_value_ctx_t **p)
{
  if(id.bitfield.parameter == 255) return E_PARAM;
  *p = &engine; return E_OK;
}
static void exchange(void) { cm7_poll(); cm4_poll(); }
int main(void)
{
  ipc_initialize();ipc_store(&g_ipc.cm4_ready,1);
  ecu_config_parameter_id_t id = { .bitfield = {.supported = true, .entity = ECU_COMMON_ENTITY_SENSOR} };
  ecu_core_runtime_value_ctx_t *local=NULL, value;
  assert(cm4_get_ptr(id,&local) == E_AGAIN);
  assert(g_ipc.parameter_count == 1);
  assert(cm4_get_ptr(id,&local) == E_AGAIN);
  exchange();
  assert(cm4_get_ptr(id,&local) == E_OK);
  assert(local != &engine && local->value == 1234 && local->valid);
  assert(cm4_get_value(id,&value) == E_OK && value.value == 1234);
  engine.value = 4321;exchange();assert(local->value == 4321);
  /* A CM7 snapshot queued before the CAN write cannot roll back CM4's value. */
  cm7_poll();local->value = 99;local->valid = true;assert(cm4_write(local) == E_OK);
  cm4_poll();cm7_poll();cm4_poll();assert(engine.value == 99 && local->value == 99);
  local->valid = false;assert(cm4_write(local) == E_OK);cm4_poll();cm7_poll();assert(!engine.valid);
  assert(cm4_get_value(id,&value) == E_NOTRDY);
  /* Freshness timestamps use the same wrap as the hardware TIM5 counter. */
  id.bitfield.parameter = 1;
  assert(cm4_get_ptr(id,&local) == E_AGAIN);
  engine.valid = true;test_now = 0x03fffff0u;exchange();
  assert(cm4_get_ptr(id,&local) == E_OK && local->valid);
  test_now = 20;cm4_poll();assert(local->valid);
  test_now += 100001;cm4_poll();assert(!local->valid);
  id.bitfield.parameter = 255;
  assert(cm4_get_ptr(id,&local) == E_AGAIN);exchange();
  assert(cm4_get_ptr(id,&local) == E_PARAM);
  assert(g_ipc.parameter_count == 3);
  printf("Parameter bridge tests passed: subscription, resolution, initial snapshot, bidirectional values/validity, stale data, timer wrap, invalid id, no shared application pointers\n");
}
