/*
 * obd2_private.c
 *
 *  Created on: Oct 30, 2025
 *      Author: VHEMaster
 */

#include "obd2_private.h"

static const uint8_t obd2_pid_type_item_length[OBD2_PID_TYPE_MAX] = {
    0, // OBD2_PID_TYPE_UNDEFINED
    1, // OBD2_PID_TYPE_RAW_SINGLE_BYTE
    2, // OBD2_PID_TYPE_RAW_SINGLE_WORD
    4, // OBD2_PID_TYPE_RAW_SINGLE_DWORD
    1, // OBD2_PID_TYPE_RAW_DUAL_BYTES
    2, // OBD2_PID_TYPE_RAW_DUAL_WORD
    1, // OBD2_PID_TYPE_RAW_QUAD_BYTES
    1, // OBD2_PID_TYPE_SINGLE_BYTE
    2, // OBD2_PID_TYPE_SINGLE_WORD
    4, // OBD2_PID_TYPE_SINGLE_DWORD
    1, // OBD2_PID_TYPE_DUAL_BYTES
    2, // OBD2_PID_TYPE_DUAL_WORDS
    1, // OBD2_PID_TYPE_QUAD_BYTES
};

static const uint8_t obd2_pid_type_item_count[OBD2_PID_TYPE_MAX] = {
    0, // OBD2_PID_TYPE_UNDEFINED
    1, // OBD2_PID_TYPE_RAW_SINGLE_BYTE
    1, // OBD2_PID_TYPE_RAW_SINGLE_WORD
    1, // OBD2_PID_TYPE_RAW_SINGLE_DWORD
    2, // OBD2_PID_TYPE_RAW_DUAL_BYTES
    2, // OBD2_PID_TYPE_RAW_DUAL_WORD
    4, // OBD2_PID_TYPE_RAW_QUAD_BYTES
    1, // OBD2_PID_TYPE_SINGLE_BYTE
    1, // OBD2_PID_TYPE_SINGLE_WORD
    1, // OBD2_PID_TYPE_SINGLE_DWORD
    2, // OBD2_PID_TYPE_DUAL_BYTES
    2, // OBD2_PID_TYPE_DUAL_WORDS
    4, // OBD2_PID_TYPE_QUAD_BYTES
};

static const uint8_t obd2_pid_type_item_isfloat[OBD2_PID_TYPE_MAX] = {
    false, // OBD2_PID_TYPE_UNDEFINED
    false, // OBD2_PID_TYPE_RAW_SINGLE_BYTE
    false, // OBD2_PID_TYPE_RAW_SINGLE_WORD
    false, // OBD2_PID_TYPE_RAW_SINGLE_DWORD
    false, // OBD2_PID_TYPE_RAW_DUAL_BYTES
    false, // OBD2_PID_TYPE_RAW_DUAL_WORD
    false, // OBD2_PID_TYPE_RAW_QUAD_BYTES
    true, // OBD2_PID_TYPE_SINGLE_BYTE
    true, // OBD2_PID_TYPE_SINGLE_WORD
    true, // OBD2_PID_TYPE_SINGLE_DWORD
    true, // OBD2_PID_TYPE_DUAL_BYTES
    true, // OBD2_PID_TYPE_DUAL_WORDS
    true, // OBD2_PID_TYPE_QUAD_BYTES
};

static const uint8_t obd2_pid_type_item_israw[OBD2_PID_TYPE_MAX] = {
    false, // OBD2_PID_TYPE_UNDEFINED
    true, // OBD2_PID_TYPE_RAW_SINGLE_BYTE
    true, // OBD2_PID_TYPE_RAW_SINGLE_WORD
    true, // OBD2_PID_TYPE_RAW_SINGLE_DWORD
    true, // OBD2_PID_TYPE_RAW_DUAL_BYTES
    true, // OBD2_PID_TYPE_RAW_DUAL_WORD
    true, // OBD2_PID_TYPE_RAW_QUAD_BYTES
    false, // OBD2_PID_TYPE_SINGLE_BYTE
    false, // OBD2_PID_TYPE_SINGLE_WORD
    false, // OBD2_PID_TYPE_SINGLE_DWORD
    false, // OBD2_PID_TYPE_DUAL_BYTES
    false, // OBD2_PID_TYPE_DUAL_WORDS
    false, // OBD2_PID_TYPE_QUAD_BYTES
};

static void obd2_sid_current_data(obd2_ctx_t *ctx);
static void obd2_sid_freeze_frame_data(obd2_ctx_t *ctx);
static void obd2_sid_diagnostic_trouble_codes(obd2_ctx_t *ctx);
static void obd2_sid_clear_diagnostic_information(obd2_ctx_t *ctx);
static void obd2_sid_oxygen_sensor_monitoring(obd2_ctx_t *ctx);
static void obd2_sid_on_board_monitoring_tests(obd2_ctx_t *ctx);
static void obd2_sid_pending_diagnostic_trouble_codes(obd2_ctx_t *ctx);
static void obd2_sid_vehicle_information(obd2_ctx_t *ctx);
static void obd2_sid_permament_diagnostic_trouble_codes(obd2_ctx_t *ctx);

void obd2_loop_handler(obd2_ctx_t *ctx)
{
  time_us_t now;

  do {
    now = time_now_us();

    if(ctx->reset_trigger) {
      ctx->downstream_available = false;
      ctx->upstream_available = false;
      ctx->error_code = OBD2_OK;
      ctx->reset_trigger = false;
      break;
    }

    if(!ctx->downstream_available) {
      if(ctx->upstream_available) {

        ctx->downstream_data_len = 0;
        ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_POSITIVE_OFFSET + ctx->upstream_data[0];

        switch(ctx->upstream_data[0]) {
          case OBD2_SID_CURRENT_DATA:
            obd2_sid_current_data(ctx);
            break;
          case OBD2_SID_FREEZE_FRAME_DATA:
            obd2_sid_freeze_frame_data(ctx);
            break;
          case OBD2_SID_DIAGNOSTIC_TROUBLE_CODES:
            obd2_sid_diagnostic_trouble_codes(ctx);
            break;
          case OBD2_SID_CLEAR_DIAGNOSTIC_INFORMATION:
            obd2_sid_clear_diagnostic_information(ctx);
            break;
          case OBD2_SID_OXYGEN_SENSOR_MONITORING:
            obd2_sid_oxygen_sensor_monitoring(ctx);
            break;
          case OBD2_SID_ON_BOARD_MONITORING_TESTS:
            obd2_sid_on_board_monitoring_tests(ctx);
            break;
          case OBD2_SID_PENDING_DIAGNOSTIC_TROUBLE_CODES:
            obd2_sid_pending_diagnostic_trouble_codes(ctx);
            break;
          case OBD2_SID_VEHICLE_INFORMATION:
            obd2_sid_vehicle_information(ctx);
            break;
          case OBD2_SID_PERMANENT_DIAGNOSTIC_TROUBLE_CODES:
            obd2_sid_permament_diagnostic_trouble_codes(ctx);
            break;
          default:
            ctx->downstream_data_len = 0;
            ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
            ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
            ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SERVICE_NOT_SUPPORTED;
            ctx->downstream_available = true;
            break;
        }

        ctx->upstream_available = false;
        ctx->downstream_time = now;
      }
    } else if(time_diff(now, ctx->downstream_time) >= OBD2_DOWNSTREAM_TIMEOUT) {
      ctx->reset_trigger = true;
      continue;
    }
  } while(0);
}

static void obd2_sid_current_data(obd2_ctx_t *ctx)
{
  uint8_t adder;
  uint32_t payload;
  uint8_t pid;

  uint8_t dlen;
  obd2_mode1_pid_type_t dtype;
  float raw;

  do {
    for(int i = 1; i < ctx->upstream_data_len; i++) {
      pid = ctx->upstream_data[i];
      dtype = ctx->mode1_setup[pid].type;
      dlen = obd2_pid_type_item_length[dtype];

      if(pid < OBD2_PID_01_MAX && ctx->mode1_data[pid].supported && dtype != OBD2_PID_TYPE_UNDEFINED) {
        switch(pid) {
          case OBD2_PID_01_SUPPORTED_01_20:
          case OBD2_PID_01_SUPPORTED_21_40:
          case OBD2_PID_01_SUPPORTED_41_60:
          case OBD2_PID_01_SUPPORTED_61_80:
            ctx->downstream_data[ctx->downstream_data_len++] = pid;
            adder = pid;
            payload = 0;
            for(uint8_t p = 0, idx = adder + 1; p < 0x20; p++, idx++) {
              BREAK_IF(idx >= OBD2_PID_01_MAX);
              if(ctx->mode1_data[idx].supported && ctx->mode1_setup[pid].type != OBD2_PID_TYPE_UNDEFINED) {
                payload |= 1 << p;
              }
            }
            payload = __RBIT(payload);
            payload = __REV(payload);
            memcpy(&ctx->downstream_data[ctx->downstream_data_len], &payload, dlen);
            ctx->downstream_data_len += dlen;
            break;
          default:
            ctx->downstream_data[ctx->downstream_data_len++] = pid;
            for(int i = 0; i < obd2_pid_type_item_count[dtype]; i++) {
              if(obd2_pid_type_item_isfloat[dtype]) {
                raw = ctx->mode1_data[pid].value[i].flt;
                raw -= ctx->mode1_setup[pid].gain_offset[i].offset;
                raw /= ctx->mode1_setup[pid].gain_offset[i].gain;
                ctx->mode1_data[pid].value[i].raw = roundf(raw);
              }

              payload = ctx->mode1_data[pid].value[i].raw << (32 - dlen * 8);
              payload = __REV(payload);
              memcpy(&ctx->downstream_data[ctx->downstream_data_len], &payload, dlen);
              ctx->downstream_data_len += dlen;
            }
            break;
        }
      }
    }

    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SUBFUNCTION_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void obd2_sid_freeze_frame_data(obd2_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void obd2_sid_diagnostic_trouble_codes(obd2_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void obd2_sid_clear_diagnostic_information(obd2_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void obd2_sid_oxygen_sensor_monitoring(obd2_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void obd2_sid_on_board_monitoring_tests(obd2_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void obd2_sid_pending_diagnostic_trouble_codes(obd2_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void obd2_sid_vehicle_information(obd2_ctx_t *ctx)
{
  uint8_t adder;
  uint32_t payload;
  uint8_t pid;
  uint8_t idx;
  uint8_t dlen;

  do {
    pid = ctx->upstream_data[1];
    if(ctx->upstream_data_len >= 3) {
      idx = ctx->upstream_data[2];
    } else {
      idx = 1;
    }

    if(pid < OBD2_PID_09_MAX && ctx->mode9_data[pid].supported) {
      switch(pid) {
        case OBD2_PID_09_SUPPORTED_01_20:
        case OBD2_PID_09_SUPPORTED_21_40:
          dlen = 4;
          ctx->downstream_data[ctx->downstream_data_len++] = pid;
          adder = pid;
          payload = 0;
          for(uint8_t p = 0, idx = adder + 1; p < 0x20; p++, idx++) {
            BREAK_IF(idx >= OBD2_PID_09_MAX);
            if(ctx->mode9_data[idx].supported) {
              payload |= 1 << p;
            }
          }
          payload = __RBIT(payload);
          payload = __REV(payload);
          memcpy(&ctx->downstream_data[ctx->downstream_data_len], &payload, dlen);
          ctx->downstream_data_len += dlen;
          break;
        case OBD2_PID_09_VIN_MESSAGE_COUNT:
        case OBD2_PID_09_CALID_MESSAGE_COUNT:
        case OBD2_PID_09_CVN_MESSAGE_COUNT:
        case OBD2_PID_09_IUPR_MESSAGE_COUNT:
          ctx->downstream_data[ctx->downstream_data_len++] = pid;
          ctx->downstream_data[ctx->downstream_data_len++] = ctx->mode9_data[pid + 1].count;
          break;
        case OBD2_PID_09_VIN:
        case OBD2_PID_09_CALID:
        case OBD2_PID_09_CVN:
        case OBD2_PID_09_IUPR_SPARK:
        case OBD2_PID_09_ECU_NAME:
        case OBD2_PID_09_HARDWARE_NUMBER:
        case OBD2_PID_09_VIN_DUPLICATE_FRAME:
          if(idx - 1u < ctx->mode9_data[pid].count) {
            ctx->downstream_data[ctx->downstream_data_len++] = pid;
            ctx->downstream_data[ctx->downstream_data_len++] = idx;
            dlen = ctx->mode9_data[pid].length[idx - 1u];
            memcpy(&ctx->downstream_data[ctx->downstream_data_len], ctx->mode9_data[pid].value[idx - 1u], dlen);
            ctx->downstream_data_len += dlen;
          } else {
            ctx->downstream_data_len = 0;
            ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
            ctx->downstream_data[ctx->downstream_data_len++] = pid;
            ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_REQUEST_OUT_OF_RANGE;
          }
          break;
        default:
          if(ctx->mode9_data[pid].count > 0) {
            ctx->downstream_data[ctx->downstream_data_len++] = pid;
            dlen = ctx->mode9_data[pid].length[0];
            if(dlen > OBD2_DATA_LENGTH_MAX) {
              dlen = OBD2_DATA_LENGTH_MAX;
            }
            memcpy(&ctx->downstream_data[ctx->downstream_data_len], ctx->mode9_data[pid].value[0], dlen);
            ctx->downstream_data_len += dlen;
          }
          break;
      }
    }

    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SUBFUNCTION_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}

static void obd2_sid_permament_diagnostic_trouble_codes(obd2_ctx_t *ctx)
{
  do {
    if(ctx->downstream_data_len <= 1) {
      ctx->downstream_data_len = 0;
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_NEGATIVE_CODE;
      ctx->downstream_data[ctx->downstream_data_len++] = ctx->upstream_data[0];
      ctx->downstream_data[ctx->downstream_data_len++] = OBD2_RESPONSE_SERVICE_NOT_SUPPORTED;
    }
    ctx->downstream_available = true;
  } while(0);
}
