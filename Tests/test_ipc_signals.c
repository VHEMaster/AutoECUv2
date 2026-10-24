/* Exercise the real Signals codec, FIFO, timeout and CM4 configuration backend. */
#include "comm_ipc.h"
#include "config_signals.h"
#include <assert.h>
#include <stdio.h>
uint32_t test_now;
uint32_t test_time_now_us(void) { return test_now; }
static ecu_core_runtime_value_ctx_t engine[2] = {{0,false},{73,true}};
static bool enabled;
void test_enter_critical(void) { }
void cm4_poll(void);
void cm7_poll(void);
error_t engine_get_ptr(ecu_config_parameter_id_t id, ecu_core_runtime_value_ctx_t **p)
{
  if(id.bitfield.parameter >= 2) return E_PARAM;
  *p = &engine[id.bitfield.parameter]; return E_OK;
}
error_t ecu_comm_set_comm_enabled(ecu_comm_type_t type, ecu_comm_instance_t instance, bool value)
{
  assert(type == ECU_COMM_TYPE_SIGNALS && instance == ECU_COMM_SIGNALS_1);
  enabled = value; return E_OK;
}
static void exchange(void) { cm7_poll(); cm4_poll(); }
static void configure(const signals_config_t *config)
{
  for(unsigned n = 0; n < 20; ++n) {
    error_t err = ecu_comm_signals_configure(ECU_COMM_SIGNALS_1,config);
    if(err != E_AGAIN) { assert(err == E_OK); return; }
    cm4_poll(); cm7_poll(); cm4_poll();
  }
  assert(!"Signals configuration did not complete");
}
static void receive(signals_ctx_t *ctx, uint8_t value)
{
  signals_runtime_messages_fifo_item_t frame = {
    .message = {.id = 0x123, .len = 1, .payload = {value}}, .timestamp = test_now
  };
  assert(signals_data_write_upstream(ctx,&frame) == E_OK);
  signals_loop_comm(ctx);cm4_poll();cm7_poll();cm4_poll();
  assert(engine[0].value == value && engine[0].valid);
}
int main(void)
{
  ipc_initialize();ipc_store(&g_ipc.cm4_ready,1);
  static signals_ctx_t ctx;
  assert(ecu_comm_signals_init(ECU_COMM_SIGNALS_1,&ctx) == E_OK);
  signals_init_ctx_t init = ctx.init;
  static signals_config_t config;
  config.enabled = true;
  config.messages_lists.upstream_list[0] = (signals_config_message_description_t){
    .enabled = true, .message_id = 0x123, .message_len = 1, .upstream_timeout = 1000
  };
  config.messages_lists.downstream_list[0] = (signals_config_message_description_t){
    .enabled = true, .message_id = 0x456, .message_len = 1, .downstream_periodicity = 100
  };
  config.signals_lists.upstream_list[0] = (signals_config_signal_description_t){
    .enabled = true, .message_id = 0x123,
    .description = {.type = CAN_SIGNAL_TYPE_UINT, .byteorder = CAN_SIGNAL_BYTEORDER_INTEL, .bitlength = 8, .gain = 1},
    .parameter_id = {.bitfield = {.supported = true, .entity = ECU_COMMON_ENTITY_SENSOR, .parameter = 0}}
  };
  config.signals_lists.downstream_list[0] = config.signals_lists.upstream_list[0];
  config.signals_lists.downstream_list[0].message_id = 0x456;
  config.signals_lists.downstream_list[0].parameter_id.bitfield.parameter = 1;
  configure(&config);assert(enabled && ctx.configured && ctx.initialized);
  receive(&ctx,42);
  engine[1].value = 91;exchange();exchange();
  test_now = 200;signals_loop_comm(&ctx);
  signals_runtime_messages_fifo_item_t output;
  assert(signals_data_read_downstream(&ctx,&output) == E_OK);
  assert(output.message.id == 0x456 && output.message.len == 1 && output.message.payload[0] == 91);
  test_now = 1501;signals_loop_comm(&ctx);cm4_poll();cm7_poll();assert(!engine[0].valid);
  receive(&ctx,54);
  assert(ecu_comm_signals_reset(ECU_COMM_SIGNALS_1) == E_AGAIN);
  assert(!ctx.configured && ctx.initialized);
  for(unsigned n = 0; n < 10; ++n) {
    cm4_poll();cm7_poll();cm4_poll();
    error_t err = ecu_comm_signals_reset(ECU_COMM_SIGNALS_1);
    if(err == E_OK) break;
    assert(err == E_AGAIN && n < 9);
  }
  assert(!enabled && !engine[0].valid && ctx.initialized);
  assert(!memcmp(&init,&ctx.init,sizeof(init)));
  configure(&config);receive(&ctx,99);
  config.enabled = false;configure(&config);
  assert(!engine[0].valid && !enabled && !ctx.configured && ctx.initialized);
  printf("Signals integration passed: real CAN decode/encode, asynchronous configuration, parameter IPC, timeout, reset, disable and reconfiguration\n");
}
