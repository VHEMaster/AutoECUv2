/*
 * config_obd2.c
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#include <string.h>
#include "config_obd2.h"
#include "compiler.h"

#include "config_global.h"

// TODO: remove me
#define ECU_SENSOR_READ_PARAM_DATA (0)

typedef struct ecu_comm_obd2_ctx_tag ecu_comm_obd2_ctx_t;

static void ecu_comm_obd2_error_callback(obd2_ctx_t *ctx, obd2_error_code_t code, void *userdata);

typedef struct ecu_comm_obd2_ctx_tag {
    obd2_init_ctx_t init;
    obd2_ctx_t *ctx;
}ecu_comm_obd2_ctx_t;





static RAM_SECTION ecu_comm_obd2_ctx_t ecu_comm_obd2_ctx[ECU_COMM_OBD2_MAX] = {
    {
      .init = {
          .error_callback = ecu_comm_obd2_error_callback,
          .callback_userdata = NULL,
      },
    },
    {
      .init = {
          .error_callback = ecu_comm_obd2_error_callback,
          .callback_userdata = NULL,
      },
    },
    {
      .init = {
          .error_callback = ecu_comm_obd2_error_callback,
          .callback_userdata = NULL,
      },
    },
};

error_t ecu_comm_obd2_init(ecu_comm_obd2_t instance, obd2_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_obd2_ctx_t *obd2_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_OBD2_MAX || ctx == NULL, err = E_PARAM);

    obd2_ctx = &ecu_comm_obd2_ctx[instance];
    obd2_ctx->ctx = ctx;

    obd2_ctx->init.callback_userdata = (void *)obd2_ctx;

    err = obd2_init(obd2_ctx->ctx, &obd2_ctx->init);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_OBD2, instance, false);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}



error_t ecu_comm_obd2_configure(ecu_comm_obd2_t instance, const obd2_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_obd2_ctx_t *obd2_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_OBD2_MAX || config == NULL, err = E_PARAM);

    obd2_ctx = &ecu_comm_obd2_ctx[instance];

    err = obd2_configure(obd2_ctx->ctx, config);
    BREAK_IF(err != E_OK);

    err = ecu_comm_set_comm_enabled(ECU_COMM_TYPE_OBD2, instance, obd2_ctx->ctx->config.enabled);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_comm_obd2_reset(ecu_comm_obd2_t instance)
{
  error_t err = E_OK;
  ecu_comm_obd2_ctx_t *obd2_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_OBD2_MAX, err = E_PARAM);

    obd2_ctx = &ecu_comm_obd2_ctx[instance];

    err = obd2_reset(obd2_ctx->ctx);

  } while(0);

  return err;
}

static void ecu_comm_obd2_error_callback(obd2_ctx_t *ctx, obd2_error_code_t code, void *userdata)
{
  // TODO: IMPLEMENT
}
