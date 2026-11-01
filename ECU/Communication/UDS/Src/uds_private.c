/*
 * uds_private.c
 *
 *  Created on: Nov 2, 2025
 *      Author: VHEMaster
 */

#include "uds_private.h"

static const bool uds_did_type_item_isfloat[UDS_DID_TYPE_MAX] = {
    false, // UDS_DID_TYPE_UNDEFINED
    false, // UDS_DID_TYPE_STRING
    true, // UDS_DID_TYPE_FLOAT
    true, // UDS_DID_TYPE_DWORD
    true, // UDS_DID_TYPE_WORD
    true, // UDS_DID_TYPE_BYTE
    false, // UDS_DID_TYPE_RAW_DWORD
    false, // UDS_DID_TYPE_RAW_WORD
    false, // UDS_DID_TYPE_RAW_BYTE
    false, // UDS_DID_TYPE_RAW_BOOL
};

static const uint8_t uds_did_type_item_length[UDS_DID_TYPE_MAX] = {
    0, // UDS_DID_TYPE_UNDEFINED
    0, // UDS_DID_TYPE_STRING
    4, // UDS_DID_TYPE_FLOAT
    4, // UDS_DID_TYPE_DWORD
    2, // UDS_DID_TYPE_WORD
    1, // UDS_DID_TYPE_BYTE
    4, // UDS_DID_TYPE_RAW_DWORD
    2, // UDS_DID_TYPE_RAW_WORD
    1, // UDS_DID_TYPE_RAW_BYTE
    1, // UDS_DID_TYPE_RAW_BOOL
};

static void uds_sid_diagnostic_session_control(uds_ctx_t *ctx);
static void uds_sid_read_dtc_information(uds_ctx_t *ctx);
static void uds_sid_read_data_by_identifier(uds_ctx_t *ctx);
static void uds_sid_read_memory_by_address(uds_ctx_t *ctx);

void uds_loop_handler(uds_ctx_t *ctx)
{
  time_us_t now;

  do {
    now = time_now_us();

    if(ctx->reset_trigger) {
      ctx->downstream_available = false;
      ctx->upstream_available = false;
      ctx->error_code = UDS_OK;
      ctx->reset_trigger = false;
      break;
    }

    if(!ctx->downstream_available) {
      if(ctx->upstream_available) {

        ctx->downstream_data_len = 0;
        ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_POSITIVE_OFFSET + ctx->upstream_data[0];

        switch(ctx->upstream_data[0]) {
          case UDS_SID_DIAGNOSTIC_SESSION_CONTROL:
            uds_sid_diagnostic_session_control(ctx);
            break;
          case UDS_SID_READ_DTC_INFORMATION:
            uds_sid_read_dtc_information(ctx);
            break;
          case UDS_SID_READ_DATA_BY_IDENTIFIER:
            uds_sid_read_data_by_identifier(ctx);
            break;
          case UDS_SID_READ_MEMORY_BY_ADDRESS:
            uds_sid_read_memory_by_address(ctx);
            break;
          default:
            ctx->downstream_data_len = 0;
            ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_NEGATIVE_CODE;
            ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
            ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_SERVICE_NOT_SUPPORTED;
            ctx->downstream_available = true;
            break;
        }

        ctx->upstream_available = false;
        ctx->downstream_time = now;
      }
    } else if(time_diff(now, ctx->downstream_time) >= UDS_DOWNSTREAM_TIMEOUT) {
      ctx->reset_trigger = true;
      continue;
    }
  } while(0);
}

static void uds_sid_diagnostic_session_control(uds_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void uds_sid_read_dtc_information(uds_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void uds_sid_read_data_by_identifier(uds_ctx_t *ctx)
{
  uint32_t payload;
  uint16_t did;
  uint16_t did_rev;

  uint8_t dlen;
  uds_did_type_t dtype;
  const uds_setup_did_t *setup = NULL;
  uds_did_value_t *value = NULL;
  float raw;

  do {
    memcpy(&did_rev, &ctx->upstream_data[1], sizeof(did_rev));
    did = __REV16(did_rev);

    for(uint32_t i = 0; i < ctx->data_items_count; i++) {
      if(ctx->data_setup[i].did == did) {
        setup = &ctx->data_setup[i];
        value = &ctx->data_values[i];
        break;
      }
    }

    if(setup != NULL && value != NULL) {
      dtype = setup->type;
      dlen = uds_did_type_item_length[dtype];

      if(value->supported && dtype != UDS_DID_TYPE_UNDEFINED) {
        switch(did) {
          default:
            memcpy(&ctx->downstream_data[ctx->downstream_data_len], &did_rev, sizeof(did_rev));
            ctx->downstream_data_len += sizeof(did_rev);

            if(dtype == UDS_DID_TYPE_FLOAT) {
              memcpy(&value->raw, &value->flt, sizeof(float));
            } else if(uds_did_type_item_isfloat[dtype]) {
              raw = value->flt;
              raw -= setup->gain_offset.offset;
              raw /= setup->gain_offset.gain;
              value->raw = roundf(raw);
            }

            payload = value->raw << (32 - dlen * 8);
            payload = __REV(payload);
            memcpy(&ctx->downstream_data[ctx->downstream_data_len], &payload, dlen);
            ctx->downstream_data_len += dlen;
            break;
        }
      }
    }
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_REQUEST_OUT_OF_RANGE;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void uds_sid_read_memory_by_address(uds_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = UDS_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}
