/* Real OBD2 PID processing over production IPC parameter proxies. */
#include "comm_ipc.h"
#include "config_obd2.h"
#include <assert.h>
#include <stdio.h>
uint32_t test_now;
uint32_t test_time_now_us(void) { return test_now; }
static ecu_core_runtime_value_ctx_t rpm = {3000,true};
static bool enabled;
void test_enter_critical(void) { }
void cm4_poll(void);
void cm7_poll(void);
error_t engine_get_ptr(ecu_config_parameter_id_t id, ecu_core_runtime_value_ctx_t **p)
{
  if(id.bitfield.parameter != 0) return E_PARAM;
  *p = &rpm;return E_OK;
}
error_t ecu_comm_set_comm_enabled(ecu_comm_type_t type, ecu_comm_instance_t instance, bool value)
{
  assert(type == ECU_COMM_TYPE_OBD2 && instance == ECU_COMM_OBD2_1);
  enabled = value;return E_OK;
}
static void exchange(void) { cm7_poll();cm4_poll(); }
static void check_rpm(obd2_ctx_t *ctx, unsigned expected)
{
  const uint8_t request[] = {0x01,0x0c};
  uint8_t response[OBD2_DATA_LENGTH_MAX];uint16_t length = 0;
  assert(obd2_message_write_upstream(ctx,request,sizeof(request)) == E_OK);
  obd2_loop(ctx);
  assert(obd2_message_read_downstream(ctx,response,&length) == E_OK);
  assert(length == 4 && response[0] == 0x41 && response[1] == 0x0c);
  assert((((unsigned)response[2] << 8) | response[3]) == expected * 4);
}
int main(void)
{
  ipc_initialize();ipc_store(&g_ipc.cm4_ready,1);
  static obd2_ctx_t ctx;
  assert(ecu_comm_obd2_init(ECU_COMM_OBD2_1,&ctx) == E_OK);
  static obd2_config_t config;
  config.enabled = true;config.mode_01_linkage.supported = true;
  config.mode_01_linkage.parameters[OBD2_PID_01_ENGINE_RPM] = (obd2_config_mode_01_linkage_parameter_t){
    .supported = true,.source = OBD2_CONFIG_MODE_01_LINKAGE_SOURCE_COMMON,
    .common_ids = {{.bitfield = {.supported = true,.entity = ECU_COMMON_ENTITY_SENSOR,.parameter = 0}}}
  };
  assert(ecu_comm_obd2_configure(ECU_COMM_OBD2_1,&config) == E_AGAIN);
  assert(!enabled && !ctx.configured);
  exchange();
  assert(ecu_comm_obd2_configure(ECU_COMM_OBD2_1,&config) == E_OK);
  assert(enabled && ctx.configured);
  check_rpm(&ctx,3000);
  rpm.value = 4000;exchange();exchange();check_rpm(&ctx,4000);
  rpm.value = 0;rpm.valid = false;exchange();exchange();check_rpm(&ctx,4000);
  test_now = 100001;cm4_poll();check_rpm(&ctx,4000);
  rpm.value = 2500;rpm.valid = true;exchange();exchange();check_rpm(&ctx,2500);
  assert(ecu_comm_obd2_reset(ECU_COMM_OBD2_1) == E_OK);obd2_loop(&ctx);
  check_rpm(&ctx,2500);
  puts("OBD2 integration passed: initial snapshot gating, exact PID 0C bytes, live updates, invalid/stale source retains last valid RPM, reset");
}
