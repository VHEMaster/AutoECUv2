/*
 * calcdata_proc.c
 *
 *  Created on: Jun 7, 2025
 *      Author: VHEMaster
 */

#include "calcdata_proc.h"
#include "interpolation.h"

static const unsigned table_pointers[ECU_CONFIG_TABLE_MAX] = {
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_0 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_1 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_2 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_3 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_4 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_5 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_6 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_7 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_8 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_9 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_10 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_11 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_12 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_13 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_14 - ECU_CONFIG_TABLE_2D_32X32_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_15 - ECU_CONFIG_TABLE_2D_32X32_START]),

    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_0 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_1 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_2 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_3 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_4 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_5 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_6 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_7 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_8 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_9 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_10 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_11 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_12 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_13 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_14 - ECU_CONFIG_TABLE_2D_16X16_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_15 - ECU_CONFIG_TABLE_2D_16X16_START]),

    OFFSETOF(ecu_config_tables_t, tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_0 - ECU_CONFIG_TABLE_2D_8X8_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_1 - ECU_CONFIG_TABLE_2D_8X8_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_2 - ECU_CONFIG_TABLE_2D_8X8_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_3 - ECU_CONFIG_TABLE_2D_8X8_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_4 - ECU_CONFIG_TABLE_2D_8X8_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_5 - ECU_CONFIG_TABLE_2D_8X8_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_6 - ECU_CONFIG_TABLE_2D_8X8_START]),
    OFFSETOF(ecu_config_tables_t, tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_7 - ECU_CONFIG_TABLE_2D_8X8_START]),

    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_0 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_1 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_2 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_3 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_4 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_5 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_6 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_7 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_8 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_9 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_10 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_11 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_12 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_13 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_14 - ECU_CONFIG_TABLE_1D_32_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_32[ECU_CONFIG_TABLE_1D_32_15 - ECU_CONFIG_TABLE_1D_32_START]),

    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_0 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_1 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_2 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_3 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_4 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_5 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_6 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_7 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_8 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_9 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_10 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_11 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_12 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_13 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_14 - ECU_CONFIG_TABLE_1D_16_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_16[ECU_CONFIG_TABLE_1D_16_15 - ECU_CONFIG_TABLE_1D_16_START]),

    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_0 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_1 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_2 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_3 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_4 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_5 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_6 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_7 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_8 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_9 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_10 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_11 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_12 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_13 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_14 - ECU_CONFIG_TABLE_1D_8_START]),
    OFFSETOF(ecu_config_tables_t, tables_1d_8[ECU_CONFIG_TABLE_1D_8_15 - ECU_CONFIG_TABLE_1D_8_START]),
};

static const uint32_t table_sizes[ECU_CONFIG_TABLE_MAX] = {
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    8, 8, 8, 8, 8, 8, 8, 8,
    32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
};

error_t core_calcdata_proc_input_interp_invalidate(ecu_core_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) {
      for(ecu_config_calcdata_relation_input_source_index_t i = 0; i < CALCDATA_RELATION_INPUT_SOURCE_MAX; i++) {
        for(ecu_config_calcdata_relation_input_varianted_index_t v = 0; v < CALCDATA_RELATION_INPUT_VARIANTED_ITEM_MAX; v++) {
          ctx->runtime.banked.source.banks[b].inputs[i].interpolation[v].valid = false;
        }
      }
    }
  } while(0);

  return err;
}

error_t core_calcdata_proc_output_invalidate(ecu_core_ctx_t *ctx)
{
  error_t err = E_OK;

  do {
    for(ecu_bank_t b = 0; b < ECU_BANK_MAX; b++) {
      for(ecu_config_calcdata_output_data_index_t o = 0; o < CALCDATA_OUTPUT_MAX; o++) {
        for(ecu_config_calcdata_output_varianted_index_t v = 0; v < CALCDATA_OUTPUT_VARIANTED_ITEM_MAX; v++) {
          ctx->runtime.banked.source.banks[b].outputs[o].variants[v].valid = false;
        }
      }
    }
  } while(0);

  return err;
}

error_t core_calcdata_proc_set_input(ecu_core_ctx_t *ctx, ecu_bank_t bank,
    ecu_config_calcdata_relation_input_source_index_t input_data_index,
    const ecu_core_runtime_value_ctx_t *value)
{
  error_t err = E_OK;

  const uint32_t banks_count = ctx->runtime.global.banks_count;
  ecu_core_runtime_banked_source_bank_input_ctx_t *input_data_item;

  do {
    BREAK_IF_ACTION(input_data_index >= CALCDATA_RELATION_INPUT_SOURCE_MAX, err = E_PARAM);

    for(ecu_bank_t b = bank >= ECU_BANK_MAX ? 0 : bank; b < banks_count; b++) {
      input_data_item = &ctx->runtime.banked.source.banks[b].inputs[input_data_index];
      if(value != NULL) {
        if(memcmp(&input_data_item->value, value, sizeof(*value)) != 0) {
          memcpy(&input_data_item->value, value, sizeof(*value));
          for(ecu_config_calcdata_relation_input_varianted_index_t v = 0;
              v < CALCDATA_RELATION_INPUT_VARIANTED_ITEM_MAX; v++) {
            input_data_item->interpolation[v].valid = false;
          }
        }
      } else {
        input_data_item->value.valid = false;
        for(ecu_config_calcdata_relation_input_varianted_index_t v = 0;
            v < CALCDATA_RELATION_INPUT_VARIANTED_ITEM_MAX; v++) {
          input_data_item->interpolation[v].valid = false;
        }
      }

      BREAK_IF(bank < ECU_BANK_MAX);
    }
  } while(0);

  return err;
}

INLINE error_t core_calcdata_proc_get_input(ecu_core_ctx_t *ctx, ecu_bank_t bank,
    ecu_config_calcdata_relation_input_source_index_t input_data_index,
    ecu_core_runtime_value_ctx_t *value)
{
  error_t err = E_OK;

  ecu_core_runtime_banked_source_bank_input_ctx_t *input_data_item;

  do {
    BREAK_IF_ACTION(input_data_index >= CALCDATA_RELATION_INPUT_SOURCE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(bank >= ECU_BANK_MAX, err = E_PARAM);
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);

    input_data_item = &ctx->runtime.banked.source.banks[bank].inputs[input_data_index];

    memcpy(value, &input_data_item->value, sizeof(*value));

  } while(0);

  return err;
}

INLINE error_t core_calcdata_proc_get_output(ecu_core_ctx_t *ctx, ecu_bank_t bank,
    ecu_config_calcdata_output_data_index_t output_data_index,
    ecu_config_calcdata_output_varianted_index_t variant,
    ecu_core_runtime_value_ctx_t *value)
{
  error_t err = E_OK;

  ecu_core_runtime_value_ctx_t *output_data_value;

  do {
    BREAK_IF_ACTION(output_data_index >= CALCDATA_OUTPUT_MAX, err = E_PARAM);
    BREAK_IF_ACTION(bank >= ECU_BANK_MAX, err = E_PARAM);
    BREAK_IF_ACTION(value == NULL, err = E_PARAM);

    output_data_value = &ctx->runtime.banked.source.banks[bank].outputs[output_data_index].variants[variant];

    memcpy(value, output_data_value, sizeof(*value));

  } while(0);

  return err;
}

error_t core_calcdata_proc_calc_output(ecu_core_ctx_t *ctx,
    ecu_config_calcdata_output_data_index_t output_data_index,
    ecu_config_calcdata_output_varianted_index_t variant,
    ecu_core_runtime_value_ctx_t *banks_output)
{
  error_t err = E_OK;

  const uint32_t banks_count = ctx->runtime.global.banks_count;
  const ecu_config_calcdata_output_data_item_t *output_data_item;
  const ecu_config_calcdata_output_data_varianted_item_t *output_data_item_base;
  const ecu_config_calcdata_relation_input_varianted_item_t *input_data_item_base_x;
  const ecu_config_calcdata_relation_input_varianted_item_t *input_data_item_base_y;
  const ecu_config_calcdata_relation_input_item_t *input_data_item_x;
  const ecu_config_calcdata_relation_input_item_t *input_data_item_y;
  const ecu_config_calcdata_relation_input_select_t *source_x, *source_y;
  const float *source_data_x, *source_data_y, *dest_data;

  ecu_core_runtime_banked_source_bank_input_ctx_t *source_values_x[ECU_BANK_MAX];
  ecu_core_runtime_banked_source_bank_input_ctx_t *source_values_y[ECU_BANK_MAX];
  ecu_core_runtime_interpolation_ctx_t *source_interp_x[ECU_BANK_MAX];
  ecu_core_runtime_interpolation_ctx_t *source_interp_y[ECU_BANK_MAX];
  uint32_t source_data_size_x, source_data_size_y;
  uint32_t dest_data_size;
  bool banks_equal = true;

  ecu_core_runtime_value_ctx_t *dests[ECU_BANK_MAX];
  ecu_core_runtime_value_ctx_t output_value[ECU_BANK_MAX];

  do {
    BREAK_IF_ACTION(output_data_index >= CALCDATA_OUTPUT_MAX, err = E_PARAM);


    output_data_item_base = &ctx->calibration->calcdata.output_data.items[output_data_index];
    for(ecu_config_calcdata_output_varianted_index_t v =
        variant >= CALCDATA_OUTPUT_VARIANTED_ITEM_MAX ? 0 : variant;
        v < output_data_item_base->variations; v++) {

      for(ecu_bank_t b = 0; b < banks_count; b++) {
        dests[b] = &ctx->runtime.banked.source.banks[b].outputs[output_data_index].variants[v];
      }

      output_data_item = &output_data_item_base->variants[v];

      if(output_data_item->type == CALCDATA_OUTPUT_TYPE_FAILSAFE) {
        output_value[0].value = output_data_item_base->data_failsafe.value;
        output_value[0].valid = true;
        banks_equal = true;

      } else if(output_data_item->type == CALCDATA_OUTPUT_TYPE_1D) {
        source_x = &output_data_item->data_1d.source_x;
        input_data_item_base_x = &ctx->calibration->calcdata.relation_sources.items[source_x->source];
        input_data_item_x = &input_data_item_base_x->variants[source_x->variant];
        source_data_size_x = table_sizes[input_data_item_x->table_index];
        source_data_x = (const float *)((uintptr_t)&ctx->calibration->tables + table_pointers[input_data_item_x->table_index]);
        dest_data_size = table_sizes[output_data_item->data_1d.table_index];
        dest_data = (const float *)((uintptr_t)&ctx->calibration->tables + table_pointers[output_data_item->data_1d.table_index]);

        if(source_data_size_x == dest_data_size) {
          for(ecu_bank_t b = 0; b < banks_count; b++) {
            source_values_x[b] = &ctx->runtime.banked.source.banks[b].inputs[source_x->source];
            source_interp_x[b] = &source_values_x[b]->interpolation[source_x->variant];

            if(banks_equal && b && memcmp(source_values_x[0], source_values_x[b], sizeof(**source_values_x)) != 0) {
              banks_equal = false;
            }
          }

          for(ecu_bank_t b = 0; b < banks_count; b++) {
            if(source_values_x[b]->value.valid) {
              if(!source_interp_x[b]->valid) {
                source_interp_x[b]->ctx = math_interpolate_input(source_values_x[b]->value.value, source_data_x, source_data_size_x);
                source_values_x[b]->value.valid = true;
              }
              output_value[b].value = math_interpolate_1d(source_interp_x[b]->ctx, dest_data);
              output_value[b].valid = true;
            } else {
              source_interp_x[b]->valid = false;
              output_value[b].value = output_data_item_base->data_failsafe.value;
              output_value[b].valid = false;
            }
            *dests[b] = output_value[b];

            BREAK_IF(banks_equal);
          }
        } else {
          output_value[0].value = output_data_item_base->data_failsafe.value;
          output_value[0].valid = false;
          banks_equal = true;
        }

      } else if(output_data_item->type == CALCDATA_OUTPUT_TYPE_2D) {
        source_x = &output_data_item->data_2d.source_x;
        source_y = &output_data_item->data_2d.source_y;
        input_data_item_base_x = &ctx->calibration->calcdata.relation_sources.items[source_x->source];
        input_data_item_base_y = &ctx->calibration->calcdata.relation_sources.items[source_y->source];
        input_data_item_x = &input_data_item_base_x->variants[source_x->variant];
        input_data_item_y = &input_data_item_base_y->variants[source_y->variant];
        source_data_size_x = table_sizes[input_data_item_x->table_index];
        source_data_size_y = table_sizes[input_data_item_y->table_index];
        source_data_x = (const float *)((uintptr_t)&ctx->calibration->tables + table_pointers[input_data_item_x->table_index]);
        source_data_y = (const float *)((uintptr_t)&ctx->calibration->tables + table_pointers[input_data_item_y->table_index]);
        dest_data_size = table_sizes[output_data_item->data_2d.table_index];
        dest_data = (const float *)((uintptr_t)&ctx->calibration->tables + table_pointers[output_data_item->data_2d.table_index]);

        if(source_data_size_x == dest_data_size && source_data_size_y == dest_data_size) {
          for(ecu_bank_t b = 0; b < banks_count; b++) {
            source_values_x[b] = &ctx->runtime.banked.source.banks[b].inputs[source_x->source];
            source_values_y[b] = &ctx->runtime.banked.source.banks[b].inputs[source_y->source];
            source_interp_x[b] = &source_values_x[b]->interpolation[source_x->variant];
            source_interp_y[b] = &source_values_y[b]->interpolation[source_y->variant];

            if(banks_equal && b && (memcmp(source_values_x[0], source_values_x[b], sizeof(**source_values_x)) != 0 ||
                memcmp(source_values_y[0], source_values_y[b], sizeof(**source_values_y)) != 0)) {
              banks_equal = false;
            }
          }

          for(ecu_bank_t b = 0; b < banks_count; b++) {
            if(source_values_x[b]->value.valid && source_values_y[b]->value.valid) {
              if(!source_interp_x[b]->valid) {
                source_interp_x[b]->ctx = math_interpolate_input(source_values_x[b]->value.value, source_data_x, source_data_size_x);
                source_interp_x[b]->valid = true;
              }

              if(!source_interp_y[b]->valid) {
                source_interp_y[b]->ctx = math_interpolate_input(source_values_y[b]->value.value, source_data_y, source_data_size_y);
                source_interp_y[b]->valid = true;
              }
              output_value[b].value = math_interpolate_2d(
                  source_interp_x[b]->ctx, source_interp_y[b]->ctx,
                  source_data_size_x, (const float (*)[source_data_size_x])dest_data);
              output_value[b].valid = true;
            } else {
              source_interp_x[b]->valid = false;
              source_interp_y[b]->valid = false;
              output_value[b].value = output_data_item_base->data_failsafe.value;
              output_value[b].valid = false;
            }
            *dests[b] = output_value[b];

            BREAK_IF(banks_equal);
          }
        } else {
          output_value[0].value = output_data_item_base->data_failsafe.value;
          output_value[0].valid = false;
          banks_equal = true;
        }

      }

      if(banks_equal) {
        for(ecu_bank_t b = 1; b < banks_count; b++) {
          if(dests[b]) {
            *dests[b] = output_value[0];
          }
        }
      }

      if(banks_output != NULL) {
        if(banks_equal) {
          for(ecu_bank_t b = 0; b < banks_count; b++) {
            banks_output[b] = output_value[0];
          }
        } else {
          for(ecu_bank_t b = 0; b < banks_count; b++) {
            banks_output[b] = output_value[b];
          }
        }
      }

      BREAK_IF(variant < CALCDATA_OUTPUT_VARIANTED_ITEM_MAX);
    }
  } while(0);

  return err;
}
