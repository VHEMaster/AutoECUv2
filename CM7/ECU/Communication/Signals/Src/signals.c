/*
 * signals.c
 *
 *  Periodic runtime parameter CAN signals
 */

#include "signals.h"
#include "config_common.h"
#include <math.h>

static error_t signals_validate(const signals_config_t *config)
{
  error_t err = E_OK;
  const signals_message_config_t *msg;
  const signals_item_config_t *signal;

  do {
    BREAK_IF_ACTION(config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(config->messages_count > SIGNALS_MESSAGES_MAX, err = E_PARAM);

    for(uint8_t i = 0; i < config->messages_count; i++) {
      msg = &config->messages[i];
      if(!msg->enabled) {
        continue;
      }

      BREAK_IF_ACTION(msg->period == 0 || msg->signals_count == 0 || msg->signals_count > SIGNALS_ITEMS_MAX, err = E_PARAM);
      BREAK_IF_ACTION((msg->message_id & CAN_MESSAGE_EXTENDED_ID_FLAG) == 0 && msg->message_id > 0x7FFu, err = E_PARAM);
      BREAK_IF_ACTION((msg->message_id & CAN_MESSAGE_EXTENDED_ID_FLAG) != 0 &&
          (msg->message_id & ~CAN_MESSAGE_EXTENDED_ID_FLAG) > 0x1FFFFFFFu, err = E_PARAM);

      for(uint8_t s = 0; s < msg->signals_count; s++) {
        signal = &msg->signals[s];
        BREAK_IF_ACTION(signal->byte_offset > 4 || signal->parameter_id.bitfield.supported == false, err = E_PARAM);
        BREAK_IF_ACTION(!isfinite(signal->multiplier) || !isfinite(signal->offset), err = E_PARAM);
        for(uint8_t n = 0; n < s; n++) {
          uint8_t previous = msg->signals[n].byte_offset;
          uint8_t offset = signal->byte_offset;
          BREAK_IF_ACTION(offset == previous || offset + 1u == previous || previous + 1u == offset, err = E_PARAM);
        }
        BREAK_IF(err != E_OK);
      }
      BREAK_IF(err != E_OK);
    }
  } while(0);

  return err;
}

error_t signals_init(signals_ctx_t *ctx, const signals_init_ctx_t *init)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || init == NULL || init->router == NULL, err = E_PARAM);

    memset(ctx, 0, sizeof(*ctx));
    memcpy(&ctx->init, init, sizeof(*init));
    ctx->initialized = true;
  } while(0);

  return err;
}

error_t signals_configure(signals_ctx_t *ctx, const signals_config_t *config)
{
  error_t err = E_OK;

  do {
    BREAK_IF_ACTION(ctx == NULL || config == NULL, err = E_PARAM);
    BREAK_IF_ACTION(ctx->initialized == false, err = E_INVALACT);

    err = signals_validate(config);
    BREAK_IF(err != E_OK);

    ctx->configured = false;
    memcpy(&ctx->config, config, sizeof(*config));
    memset(ctx->messages, 0, sizeof(ctx->messages));
    ctx->configured = true;
  } while(0);

  return err;
}

void signals_loop_comm(signals_ctx_t *ctx)
{
  const signals_message_config_t *cfg;
  const signals_item_config_t *signal;
  signals_message_runtime_t *runtime;
  ecu_core_runtime_value_ctx_t value;
  can_message_t message;
  time_us_t now;
  float scaled;
  uint16_t raw;
  error_t err;

  do {
    BREAK_IF(ctx == NULL || ctx->configured == false);
    BREAK_IF(ctx->init.router == NULL || ctx->init.router->configured == false);

    now = time_now_us();
    for(uint8_t i = 0; i < ctx->config.messages_count; i++) {
      cfg = &ctx->config.messages[i];
      runtime = &ctx->messages[i];
      if(!cfg->enabled) {
        continue;
      }

      if(runtime->pending_valid) {
        err = router_signal_transmit(ctx->init.router, &runtime->pending);
        if(err == E_OK) {
          runtime->pending_valid = false;
        }
        continue;
      }

      if(runtime->started == false) {
        runtime->last_time = now;
        runtime->started = true;
        continue;
      }
      if(time_diff(now, runtime->last_time) < cfg->period) {
        continue;
      }

      runtime->last_time = now;
      memset(&message, 0, sizeof(message));
      message.id = cfg->message_id;
      message.len = CAN_MESSAGE_PAYLOAD_LEN_MAX;

      for(uint8_t s = 0; s < cfg->signals_count; s++) {
        signal = &cfg->signals[s];
        err = ecu_config_common_get_parameter_value_by_id(signal->parameter_id, &value);
        if(err != E_OK || !value.valid || !isfinite(value.value)) {
          continue;
        }
        scaled = value.value * signal->multiplier + signal->offset;
        if(!isfinite(scaled)) {
          continue;
        }
        scaled = MAX(0.0f, MIN(65535.0f, scaled));
        raw = (uint16_t)(scaled + 0.5f);
        message.payload[signal->byte_offset] = (uint8_t)raw;
        message.payload[signal->byte_offset + 1u] = (uint8_t)(raw >> 8);
        message.payload[6] |= (uint8_t)(1u << s);
      }

      err = router_signal_transmit(ctx->init.router, &message);
      if(err == E_AGAIN) {
        runtime->pending = message;
        runtime->pending_valid = true;
      }
    }
  } while(0);
}
