/*
 * l9966_ctrl.h
 *
 *  Created on: Apr 7, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_L9966_INC_L9966_CTRL_H_
#define DEVICES_L9966_INC_L9966_CTRL_H_

#include "common.h"

typedef enum {
  L9966_CTRL_GS_CC_0 = 0x0,
  L9966_CTRL_GS_CC_1 = 0x1,
  L9966_CTRL_GS_CC_3 = 0x3,
  L9966_CTRL_GS_CC_LOST = 0x2,
  L9966_CTRL_GS_CC_MAX = 0x4,
}l9966_ctrl_gs_cc_t;

typedef enum {
  L9966_CTRL_GS_CS_ADC_RAW = 0,
  L9966_CTRL_GS_CS_ADC_CALIBRATED,
  L9966_CTRL_GS_CS_MAX
}l9966_ctrl_gs_cs_t;

typedef enum {
  L9966_CTRL_GS_OTM_DISABLED = 0,
  L9966_CTRL_GS_OTM_ENABLED,
  L9966_CTRL_GS_OTM_MAX
}l9966_ctrl_gs_otm_t;

typedef enum {
  L9966_CTRL_GS_CF_CLEAR = 0,
  L9966_CTRL_GS_CF_FAULT
}l9966_ctrl_gs_cf_t;

typedef enum {
  L9966_CTRL_GS_TF_CLEAR = 0,
  L9966_CTRL_GS_TF_FAULT
}l9966_ctrl_gs_tf_t;

typedef enum {
  L9966_CTRL_GS_OT_CLEAR = 0,
  L9966_CTRL_GS_OT_FAULT
}l9966_ctrl_gs_ot_t;

typedef enum {
  L9966_CTRL_GS_3V3_CLEAR = 0,
  L9966_CTRL_GS_3V3_FAULT
}l9966_ctrl_gs_3v3f_t;

typedef enum {
  L9966_CTRL_GS_WU_CLEAR = 0,
  L9966_CTRL_GS_WU_WAKEUP
}l9966_ctrl_gs_wu_t;

typedef struct {
    l9966_ctrl_gs_cc_t config_check;
    l9966_ctrl_gs_cs_t calib_sel;
    l9966_ctrl_gs_cf_t calib_fault;
    l9966_ctrl_gs_tf_t trim_fault;
    l9966_ctrl_gs_otm_t overtemperature_mask;
    l9966_ctrl_gs_wu_t wakeup_event;
    l9966_ctrl_gs_3v3f_t supply_3v3_fault;
    l9966_ctrl_gs_ot_t overtemperature_fault;
}l9966_ctrl_gs_t;

typedef enum {
  L9966_CTRL_DIG_IN_CH1 = 1,
  L9966_CTRL_DIG_IN_CH2 = 2,
  L9966_CTRL_DIG_IN_CH3 = 4,
  L9966_CTRL_DIG_IN_CH4 = 8,
  L9966_CTRL_DIG_IN_CH5 = 16,
  L9966_CTRL_DIG_IN_CH6 = 32,
  L9966_CTRL_DIG_IN_CH7 = 64,
  L9966_CTRL_DIG_IN_CH8 = 128,
  L9966_CTRL_DIG_IN_CH9 = 256,
  L9966_CTRL_DIG_IN_CH10 = 512,
  L9966_CTRL_DIG_IN_CH11 = 1024,
  L9966_CTRL_DIG_IN_CH12 = 2048,
  L9966_CTRL_DIG_IN_CH13 = 4096,
  L9966_CTRL_DIG_IN_CH14 = 8192,
  L9966_CTRL_DIG_IN_CH15 = 16384,
}l9966_ctrl_dig_inputs_t;

typedef enum {
  L9966_CTRL_SOFT_RESET_CMD_FIRST = 0x9,
  L9966_CTRL_SOFT_RESET_CMD_SECOND = 0x6,
  L9966_CTRL_SOFT_RESET_CMD_THIRD = 0x3,
}l9966_ctrl_src_t;

typedef enum {
  L9966_CTRL_SQNCR_EOOF_SC_CPLT = 1,
  L9966_CTRL_SQNCR_EOOF_EU1_EL_CL_REACH = 2,
  L9966_CTRL_SQNCR_EOOF_EU2_EL_CL_REACH = 4,
}l9966_ctrl_sqncr_eoof_t;

typedef enum {
  L9966_CTRL_SC_ADC_MUX_DEBUG = 0,
  L9966_CTRL_SC_ADC_MUX_CH1 = 1,
  L9966_CTRL_SC_ADC_MUX_CH2 = 2,
  L9966_CTRL_SC_ADC_MUX_CH3 = 3,
  L9966_CTRL_SC_ADC_MUX_CH4 = 4,
  L9966_CTRL_SC_ADC_MUX_CH5 = 5,
  L9966_CTRL_SC_ADC_MUX_CH6 = 6,
  L9966_CTRL_SC_ADC_MUX_CH7 = 7,
  L9966_CTRL_SC_ADC_MUX_CH8 = 8,
  L9966_CTRL_SC_ADC_MUX_CH9 = 9,
  L9966_CTRL_SC_ADC_MUX_CH10 = 10,
  L9966_CTRL_SC_ADC_MUX_CH11 = 11,
  L9966_CTRL_SC_ADC_MUX_CH12 = 12,
  L9966_CTRL_SC_ADC_MUX_UBSW = 13,
  L9966_CTRL_SC_ADC_MUX_VI5V = 14,
  L9966_CTRL_SC_ADC_MUX_VIX = 15,
  L9966_CTRL_SC_ADC_MUX_CH13 = 16,
  L9966_CTRL_SC_ADC_MUX_CH14 = 17,
  L9966_CTRL_SC_ADC_MUX_CH15 = 18,
  L9966_CTRL_SC_ADC_MUX_BG2 = 19,
  L9966_CTRL_SC_ADC_MUX_TJ = 20,
}l9966_ctrl_sc_adc_mux_t;

typedef enum {
  L9966_CTRL_SC_PU_DISABLED = 0,
  L9966_CTRL_SC_PU_RR1 = 1,
  L9966_CTRL_SC_PU_RR2 = 2,
  L9966_CTRL_SC_PU_RR3 = 3,
  L9966_CTRL_SC_PU_MAX = 4,

  L9966_CTRL_SC_DIV_5V = 0,
  L9966_CTRL_SC_DIV_20V = 1,
  L9966_CTRL_SC_DIV_40V = 2,
  L9966_CTRL_SC_DIV_1V25 = 3,
  L9966_CTRL_SC_DIV_MAX = 4,
}l9966_ctrl_sc_pd_t;

typedef enum {
  L9966_CTRL_SC_RVM_RESISTANCE = 0,
  L9966_CTRL_SC_RVM_VOLTAGE,
  L9966_CTRL_SC_RVM_MAX,
}l9966_ctrl_sc_rvm_t;

typedef struct {
    l9966_ctrl_sc_adc_mux_t adc_mux;
    l9966_ctrl_sc_pd_t pu_div_sel;
    l9966_ctrl_sc_rvm_t r_volt_sel;
}l9966_ctrl_sc_conf_t;

typedef struct {
    uint8_t dev_id;
    uint8_t dev_ver;
    uint8_t hw_rev;
}l9966_ctrl_ver_t;

#endif /* DEVICES_L9966_INC_L9966_CTRL_H_ */
