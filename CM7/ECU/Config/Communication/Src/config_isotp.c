/*
 * config_isotp.c
 *
 *  Created on: Oct 23, 2025
 *      Author: vhemaster
 */

#include <string.h>
#include "config_isotp.h"
#include "compiler.h"

typedef struct ecu_comm_isotp_ctx_tag ecu_comm_isotp_ctx_t;

static void ecu_comm_isotp_error_callback(isotp_ctx_t *ctx, isotp_error_code_t code, void *userdata);

typedef struct ecu_comm_isotp_ctx_tag {
    isotp_config_t config_default;
    isotp_init_ctx_t init;
    isotp_ctx_t *ctx;

    isotp_error_code_t error_flag;
}ecu_comm_isotp_ctx_t;

static const isotp_config_t ecu_comm_isotp_config_default = {
    .timeout = 1000 * TIME_US_IN_MS,
    .upstream_block_size = 20,
    .upstream_min_separation_time = 0,
    .upstream_separation_time_gap = 200,
    .downstream_pass_early_ovf = false,
};

static ecu_comm_isotp_ctx_t ecu_comm_isotp_ctx[ECU_COMM_ISOTP_MAX] = {
    {
      .init = {
          .error_callback = ecu_comm_isotp_error_callback,
          .callback_userdata = NULL,
      },
      .config_default = ecu_comm_isotp_config_default,
    }, // ECU_COMM_ISOTP_1
    {
      .init = {
          .error_callback = ecu_comm_isotp_error_callback,
          .callback_userdata = NULL,
      },
      .config_default = ecu_comm_isotp_config_default,
    }, //ECU_COMM_ISOTP_2
};

error_t ecu_comm_isotp_init(ecu_comm_isotp_t instance, isotp_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX || ctx == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];
    isotp_ctx->ctx = ctx;

    isotp_ctx->init.callback_userdata = (void *)isotp_ctx;

    err = isotp_init(isotp_ctx->ctx, &isotp_ctx->init);
    BREAK_IF(err != E_OK);

    isotp_ctx->error_flag = ISOTP_OK;

  } while(0);

  return err;
}

error_t ecu_comm_isotp_get_default_config(ecu_comm_isotp_t instance, isotp_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX || config == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    memcpy(config, &isotp_ctx->config_default, sizeof(isotp_config_t));

  } while(0);

  return err;
}

error_t ecu_comm_isotp_configure(ecu_comm_isotp_t instance, const isotp_config_t *config)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX || config == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    err = isotp_configure(isotp_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_comm_isotp_reset(ecu_comm_isotp_t instance)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    err = isotp_reset(isotp_ctx->ctx);

  } while(0);

  return err;
}

static void ecu_comm_isotp_error_callback(isotp_ctx_t *ctx, isotp_error_code_t code, void *userdata)
{
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF(ctx == NULL);
    BREAK_IF(userdata == NULL);

    isotp_ctx = (ecu_comm_isotp_ctx_t *)userdata;
    BREAK_IF(isotp_ctx == NULL);
    BREAK_IF(isotp_ctx->ctx != ctx);

    isotp_ctx->error_flag = code;

  } while(0);
}

error_t ecu_comm_isotp_frame_write_upstream(ecu_comm_isotp_t instance, const isotp_frame_t *frame)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(frame == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    err = isotp_frame_write_upstream(isotp_ctx->ctx, frame);

  } while(0);

  return err;
}

error_t ecu_comm_isotp_frame_read_downstream(ecu_comm_isotp_t instance, isotp_frame_t *frame)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(frame == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    err = isotp_frame_read_downstream(isotp_ctx->ctx, frame);

  } while(0);

  return err;
}

error_t ecu_comm_isotp_data_write_downstream(ecu_comm_isotp_t instance, const uint8_t *payload, uint16_t length)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    err = isotp_data_write_downstream(isotp_ctx->ctx, payload, length);

  } while(0);

  return err;
}

error_t ecu_comm_isotp_data_read_upstream(ecu_comm_isotp_t instance, uint8_t *payload, uint16_t *length)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(payload == NULL, err = E_PARAM);
    BREAK_IF_ACTION(length == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    err = isotp_data_read_upstream(isotp_ctx->ctx, payload, length);

  } while(0);

  return err;
}

error_t ecu_comm_isotp_get_error_flag(ecu_comm_isotp_t instance, isotp_error_code_t *code)
{
  error_t err = E_OK;
  ecu_comm_isotp_ctx_t *isotp_ctx;

  do {
    BREAK_IF_ACTION(instance >= ECU_COMM_ISOTP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(code == NULL, err = E_PARAM);

    isotp_ctx = &ecu_comm_isotp_ctx[instance];

    *code = isotp_ctx->error_flag;
    isotp_ctx->error_flag = ISOTP_OK;

  } while(0);

  return err;
}
