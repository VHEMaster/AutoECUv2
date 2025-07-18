/*
 * versioned_tables.h
 *
 *  Created on: Feb 11, 2025
 *      Author: VHEMaster
 */

#ifndef CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_TABLES_H_
#define CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_TABLES_H_

#include "versioned_id.h"
#include "versioned_io.h"
#include "versioned_cylinders.h"
#include "versioned_injection.h"
#include "versioned_ignition.h"
#include "versioned_calcdata.h"

typedef enum {
  ECU_CONFIG_TABLES_VERSION_V1 = 0,
  ECU_CONFIG_TABLES_VERSION_MAX
}ecu_config_tables_versions_t;

typedef enum {
  ECU_CONFIG_TABLE_NONE = -1,

  ECU_CONFIG_TABLE_2D_32X32_START = 0,
  ECU_CONFIG_TABLE_2D_32X32_0 = ECU_CONFIG_TABLE_2D_32X32_START,
  ECU_CONFIG_TABLE_2D_32X32_1,
  ECU_CONFIG_TABLE_2D_32X32_2,
  ECU_CONFIG_TABLE_2D_32X32_3,
  ECU_CONFIG_TABLE_2D_32X32_4,
  ECU_CONFIG_TABLE_2D_32X32_5,
  ECU_CONFIG_TABLE_2D_32X32_6,
  ECU_CONFIG_TABLE_2D_32X32_7,
  ECU_CONFIG_TABLE_2D_32X32_8,
  ECU_CONFIG_TABLE_2D_32X32_9,
  ECU_CONFIG_TABLE_2D_32X32_10,
  ECU_CONFIG_TABLE_2D_32X32_11,
  ECU_CONFIG_TABLE_2D_32X32_12,
  ECU_CONFIG_TABLE_2D_32X32_13,
  ECU_CONFIG_TABLE_2D_32X32_14,
  ECU_CONFIG_TABLE_2D_32X32_15,
  ECU_CONFIG_TABLE_2D_32X32_END,
  ECU_CONFIG_TABLE_2D_32X32_MAX = ECU_CONFIG_TABLE_2D_32X32_END - ECU_CONFIG_TABLE_2D_32X32_START,

  ECU_CONFIG_TABLE_2D_16X16_START = ECU_CONFIG_TABLE_2D_32X32_END,
  ECU_CONFIG_TABLE_2D_16X16_0 = ECU_CONFIG_TABLE_2D_16X16_START,
  ECU_CONFIG_TABLE_2D_16X16_1,
  ECU_CONFIG_TABLE_2D_16X16_2,
  ECU_CONFIG_TABLE_2D_16X16_3,
  ECU_CONFIG_TABLE_2D_16X16_4,
  ECU_CONFIG_TABLE_2D_16X16_5,
  ECU_CONFIG_TABLE_2D_16X16_6,
  ECU_CONFIG_TABLE_2D_16X16_7,
  ECU_CONFIG_TABLE_2D_16X16_8,
  ECU_CONFIG_TABLE_2D_16X16_9,
  ECU_CONFIG_TABLE_2D_16X16_10,
  ECU_CONFIG_TABLE_2D_16X16_11,
  ECU_CONFIG_TABLE_2D_16X16_12,
  ECU_CONFIG_TABLE_2D_16X16_13,
  ECU_CONFIG_TABLE_2D_16X16_14,
  ECU_CONFIG_TABLE_2D_16X16_15,
  ECU_CONFIG_TABLE_2D_16X16_END,
  ECU_CONFIG_TABLE_2D_16X16_MAX = ECU_CONFIG_TABLE_2D_16X16_END - ECU_CONFIG_TABLE_2D_16X16_START,

  ECU_CONFIG_TABLE_2D_8X8_START = ECU_CONFIG_TABLE_2D_16X16_END,
  ECU_CONFIG_TABLE_2D_8X8_0 = ECU_CONFIG_TABLE_2D_8X8_START,
  ECU_CONFIG_TABLE_2D_8X8_1,
  ECU_CONFIG_TABLE_2D_8X8_2,
  ECU_CONFIG_TABLE_2D_8X8_3,
  ECU_CONFIG_TABLE_2D_8X8_4,
  ECU_CONFIG_TABLE_2D_8X8_5,
  ECU_CONFIG_TABLE_2D_8X8_6,
  ECU_CONFIG_TABLE_2D_8X8_7,
  ECU_CONFIG_TABLE_2D_8X8_8,
  ECU_CONFIG_TABLE_2D_8X8_9,
  ECU_CONFIG_TABLE_2D_8X8_10,
  ECU_CONFIG_TABLE_2D_8X8_11,
  ECU_CONFIG_TABLE_2D_8X8_12,
  ECU_CONFIG_TABLE_2D_8X8_13,
  ECU_CONFIG_TABLE_2D_8X8_14,
  ECU_CONFIG_TABLE_2D_8X8_15,
  ECU_CONFIG_TABLE_2D_8X8_END,
  ECU_CONFIG_TABLE_2D_8X8_MAX = ECU_CONFIG_TABLE_2D_8X8_END - ECU_CONFIG_TABLE_2D_8X8_START,


  ECU_CONFIG_TABLE_1D_32_START = ECU_CONFIG_TABLE_2D_8X8_END,
  ECU_CONFIG_TABLE_1D_32_0 = ECU_CONFIG_TABLE_1D_32_START,
  ECU_CONFIG_TABLE_1D_32_1,
  ECU_CONFIG_TABLE_1D_32_2,
  ECU_CONFIG_TABLE_1D_32_3,
  ECU_CONFIG_TABLE_1D_32_4,
  ECU_CONFIG_TABLE_1D_32_5,
  ECU_CONFIG_TABLE_1D_32_6,
  ECU_CONFIG_TABLE_1D_32_7,
  ECU_CONFIG_TABLE_1D_32_8,
  ECU_CONFIG_TABLE_1D_32_9,
  ECU_CONFIG_TABLE_1D_32_10,
  ECU_CONFIG_TABLE_1D_32_11,
  ECU_CONFIG_TABLE_1D_32_12,
  ECU_CONFIG_TABLE_1D_32_13,
  ECU_CONFIG_TABLE_1D_32_14,
  ECU_CONFIG_TABLE_1D_32_15,
  ECU_CONFIG_TABLE_1D_32_16,
  ECU_CONFIG_TABLE_1D_32_17,
  ECU_CONFIG_TABLE_1D_32_18,
  ECU_CONFIG_TABLE_1D_32_19,
  ECU_CONFIG_TABLE_1D_32_20,
  ECU_CONFIG_TABLE_1D_32_21,
  ECU_CONFIG_TABLE_1D_32_22,
  ECU_CONFIG_TABLE_1D_32_23,
  ECU_CONFIG_TABLE_1D_32_24,
  ECU_CONFIG_TABLE_1D_32_25,
  ECU_CONFIG_TABLE_1D_32_26,
  ECU_CONFIG_TABLE_1D_32_27,
  ECU_CONFIG_TABLE_1D_32_28,
  ECU_CONFIG_TABLE_1D_32_29,
  ECU_CONFIG_TABLE_1D_32_30,
  ECU_CONFIG_TABLE_1D_32_31,
  ECU_CONFIG_TABLE_1D_32_END,
  ECU_CONFIG_TABLE_1D_32_MAX = ECU_CONFIG_TABLE_1D_32_END - ECU_CONFIG_TABLE_1D_32_START,

  ECU_CONFIG_TABLE_1D_16_START = ECU_CONFIG_TABLE_1D_32_END,
  ECU_CONFIG_TABLE_1D_16_0 = ECU_CONFIG_TABLE_1D_16_START,
  ECU_CONFIG_TABLE_1D_16_1,
  ECU_CONFIG_TABLE_1D_16_2,
  ECU_CONFIG_TABLE_1D_16_3,
  ECU_CONFIG_TABLE_1D_16_4,
  ECU_CONFIG_TABLE_1D_16_5,
  ECU_CONFIG_TABLE_1D_16_6,
  ECU_CONFIG_TABLE_1D_16_7,
  ECU_CONFIG_TABLE_1D_16_8,
  ECU_CONFIG_TABLE_1D_16_9,
  ECU_CONFIG_TABLE_1D_16_10,
  ECU_CONFIG_TABLE_1D_16_11,
  ECU_CONFIG_TABLE_1D_16_12,
  ECU_CONFIG_TABLE_1D_16_13,
  ECU_CONFIG_TABLE_1D_16_14,
  ECU_CONFIG_TABLE_1D_16_15,
  ECU_CONFIG_TABLE_1D_16_16,
  ECU_CONFIG_TABLE_1D_16_17,
  ECU_CONFIG_TABLE_1D_16_18,
  ECU_CONFIG_TABLE_1D_16_19,
  ECU_CONFIG_TABLE_1D_16_20,
  ECU_CONFIG_TABLE_1D_16_21,
  ECU_CONFIG_TABLE_1D_16_22,
  ECU_CONFIG_TABLE_1D_16_23,
  ECU_CONFIG_TABLE_1D_16_24,
  ECU_CONFIG_TABLE_1D_16_25,
  ECU_CONFIG_TABLE_1D_16_26,
  ECU_CONFIG_TABLE_1D_16_27,
  ECU_CONFIG_TABLE_1D_16_28,
  ECU_CONFIG_TABLE_1D_16_29,
  ECU_CONFIG_TABLE_1D_16_30,
  ECU_CONFIG_TABLE_1D_16_31,
  ECU_CONFIG_TABLE_1D_16_32,
  ECU_CONFIG_TABLE_1D_16_33,
  ECU_CONFIG_TABLE_1D_16_34,
  ECU_CONFIG_TABLE_1D_16_35,
  ECU_CONFIG_TABLE_1D_16_36,
  ECU_CONFIG_TABLE_1D_16_37,
  ECU_CONFIG_TABLE_1D_16_38,
  ECU_CONFIG_TABLE_1D_16_39,
  ECU_CONFIG_TABLE_1D_16_40,
  ECU_CONFIG_TABLE_1D_16_41,
  ECU_CONFIG_TABLE_1D_16_42,
  ECU_CONFIG_TABLE_1D_16_43,
  ECU_CONFIG_TABLE_1D_16_44,
  ECU_CONFIG_TABLE_1D_16_45,
  ECU_CONFIG_TABLE_1D_16_46,
  ECU_CONFIG_TABLE_1D_16_47,
  ECU_CONFIG_TABLE_1D_16_48,
  ECU_CONFIG_TABLE_1D_16_49,
  ECU_CONFIG_TABLE_1D_16_50,
  ECU_CONFIG_TABLE_1D_16_51,
  ECU_CONFIG_TABLE_1D_16_52,
  ECU_CONFIG_TABLE_1D_16_53,
  ECU_CONFIG_TABLE_1D_16_54,
  ECU_CONFIG_TABLE_1D_16_55,
  ECU_CONFIG_TABLE_1D_16_56,
  ECU_CONFIG_TABLE_1D_16_57,
  ECU_CONFIG_TABLE_1D_16_58,
  ECU_CONFIG_TABLE_1D_16_59,
  ECU_CONFIG_TABLE_1D_16_60,
  ECU_CONFIG_TABLE_1D_16_61,
  ECU_CONFIG_TABLE_1D_16_62,
  ECU_CONFIG_TABLE_1D_16_63,
  ECU_CONFIG_TABLE_1D_16_END,
  ECU_CONFIG_TABLE_1D_16_MAX = ECU_CONFIG_TABLE_1D_16_END - ECU_CONFIG_TABLE_1D_16_START,

  ECU_CONFIG_TABLE_1D_8_START = ECU_CONFIG_TABLE_1D_16_END,
  ECU_CONFIG_TABLE_1D_8_0 = ECU_CONFIG_TABLE_1D_8_START,
  ECU_CONFIG_TABLE_1D_8_1,
  ECU_CONFIG_TABLE_1D_8_2,
  ECU_CONFIG_TABLE_1D_8_3,
  ECU_CONFIG_TABLE_1D_8_4,
  ECU_CONFIG_TABLE_1D_8_5,
  ECU_CONFIG_TABLE_1D_8_6,
  ECU_CONFIG_TABLE_1D_8_7,
  ECU_CONFIG_TABLE_1D_8_8,
  ECU_CONFIG_TABLE_1D_8_9,
  ECU_CONFIG_TABLE_1D_8_10,
  ECU_CONFIG_TABLE_1D_8_11,
  ECU_CONFIG_TABLE_1D_8_12,
  ECU_CONFIG_TABLE_1D_8_13,
  ECU_CONFIG_TABLE_1D_8_14,
  ECU_CONFIG_TABLE_1D_8_15,
  ECU_CONFIG_TABLE_1D_8_16,
  ECU_CONFIG_TABLE_1D_8_17,
  ECU_CONFIG_TABLE_1D_8_18,
  ECU_CONFIG_TABLE_1D_8_19,
  ECU_CONFIG_TABLE_1D_8_20,
  ECU_CONFIG_TABLE_1D_8_21,
  ECU_CONFIG_TABLE_1D_8_22,
  ECU_CONFIG_TABLE_1D_8_23,
  ECU_CONFIG_TABLE_1D_8_24,
  ECU_CONFIG_TABLE_1D_8_25,
  ECU_CONFIG_TABLE_1D_8_26,
  ECU_CONFIG_TABLE_1D_8_27,
  ECU_CONFIG_TABLE_1D_8_28,
  ECU_CONFIG_TABLE_1D_8_29,
  ECU_CONFIG_TABLE_1D_8_30,
  ECU_CONFIG_TABLE_1D_8_31,
  ECU_CONFIG_TABLE_1D_8_END,
  ECU_CONFIG_TABLE_1D_8_MAX = ECU_CONFIG_TABLE_1D_8_END - ECU_CONFIG_TABLE_1D_8_START,

  ECU_CONFIG_TABLE_MAX = ECU_CONFIG_TABLE_1D_8_END
}ecu_config_tables_table_index_t;

typedef struct {
    float table[32][32];
}ecu_config_tables_table_2d_32x32_t;

typedef struct {
    float table[16][16];
}ecu_config_tables_table_2d_16x16_t;

typedef struct {
    float table[8][8];
}ecu_config_tables_table_2d_8x8_t;

typedef struct {
    float table[32];
}ecu_config_tables_table_1d_32_t;

typedef struct {
    float table[16];
}ecu_config_tables_table_1d_16_t;

typedef struct {
    float table[8];
}ecu_config_tables_table_1d_8_t;

typedef struct {
    ecu_config_tables_table_2d_32x32_t tables_2d_32x32[ECU_CONFIG_TABLE_2D_32X32_MAX];
    ecu_config_tables_table_2d_16x16_t tables_2d_16x16[ECU_CONFIG_TABLE_2D_16X16_MAX];
    ecu_config_tables_table_2d_8x8_t tables_2d_8x8[ECU_CONFIG_TABLE_2D_8X8_MAX];

    ecu_config_tables_table_1d_32_t tables_1d_32[ECU_CONFIG_TABLE_1D_32_MAX];
    ecu_config_tables_table_1d_16_t tables_1d_16[ECU_CONFIG_TABLE_1D_16_MAX];
    ecu_config_tables_table_1d_8_t tables_1d_8[ECU_CONFIG_TABLE_1D_8_MAX];

    uint32_t align ALIGNED_CACHE;
}ecu_config_tables_v1_t ALIGNED_CACHE;

typedef ecu_config_tables_v1_t ecu_config_tables_t;

#endif /* CONFIG_VERSIONED_CALIBRATION_INC_VERSIONED_TABLES_H_ */
