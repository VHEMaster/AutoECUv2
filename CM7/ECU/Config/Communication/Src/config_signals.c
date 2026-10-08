/*
 * config_signals.c
 *
 *  Created for periodic CAN signals
 */

#include "config_signals.h"
#include "config_comm.h"
#include "config_ckp.h"
#include "config_map.h"
#include "config_tps.h"

static const signals_config_t ecu_comm_signals_default_config = {
    .messages_count = 1,
    .messages = {
        {
            .enabled = true,
            .message_id = 0x600,
            .period = 100u * TIME_US_IN_MS,
            .signals_count = 3,
            .signals = {
                {
                    .parameter_id = { .bitfield = {
                        .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                        .type = ECU_SENSOR_TYPE_CKP, .instance = ECU_SENSOR_CKP_1,
                        .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_CKP_READ_PARAM_DATA,
                    }},
                    .multiplier = 1.0f, .byte_offset = 0,
                },
                {
                    .parameter_id = { .bitfield = {
                        .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                        .type = ECU_SENSOR_TYPE_MAP, .instance = ECU_SENSOR_MAP_1,
                        .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_MAP_READ_PARAM_DATA,
                    }},
                    .multiplier = 1000.0f, .byte_offset = 2,
                },
                {
                    .parameter_id = { .bitfield = {
                        .supported = true, .entity = ECU_COMMON_ENTITY_SENSOR,
                        .type = ECU_SENSOR_TYPE_TPS, .instance = ECU_SENSOR_TPS_1,
                        .read_write = ECU_COMMON_READ, .parameter = ECU_SENSOR_TPS_READ_PARAM_DATA,
                    }},
                    .multiplier = 100.0f, .byte_offset = 4,
                },
            },
        },
    },
};

static RAM_SECTION signals_ctx_t ecu_comm_signals_ctx;

error_t ecu_comm_signals_init(void)
{
  error_t err = E_OK;
  signals_init_ctx_t init;
  router_ctx_t *router;

  do {
    err = ecu_comm_get_router_ctx(ECU_COMM_ROUTER_1, &router);
    BREAK_IF(err != E_OK);

    init.router = router;
    err = signals_init(&ecu_comm_signals_ctx, &init);
    BREAK_IF(err != E_OK);

    err = signals_configure(&ecu_comm_signals_ctx, &ecu_comm_signals_default_config);
  } while(0);

  return err;
}

void ecu_comm_signals_loop_comm(void)
{
  signals_loop_comm(&ecu_comm_signals_ctx);
}
