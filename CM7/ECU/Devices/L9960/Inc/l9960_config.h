/*
 * l9960_config.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_L9960_INC_L9960_CONFIG_H_
#define DEVICES_L9960_INC_L9960_CONFIG_H_

typedef enum {
  L9960_CONFIG_CC_DISABLED = 0,
  L9960_CONFIG_CC_ENABLED,
  L9960_CONFIG_CC_MAX,
}l9960_config_cc_t;

typedef enum {
  L9960_CONFIG_DCE_DISABLED = 0,
  L9960_CONFIG_DCE_ENABLED,
  L9960_CONFIG_DCE_MAX,
}l9960_config_dce_t;

typedef enum {
  L9960_CONFIG_TLC_GATEFB_ONLY = 0,
  L9960_CONFIG_TLC_GATEFB_OR_OUTON,
  L9960_CONFIG_TLC_MAX,
}l9960_config_tlc_t;

typedef enum {
  L9960_CONFIG_TD1_9US = 0,
  L9960_CONFIG_TD1_14US,
  L9960_CONFIG_TD1_20US,
  L9960_CONFIG_TD1_25US,
  L9960_CONFIG_TD1_30US,
  L9960_CONFIG_TD1_35US,
  L9960_CONFIG_TD1_40US,
  L9960_CONFIG_TD1_45US,
  L9960_CONFIG_TD1_MAX,
}l9960_config_td1_t;

typedef enum {
  L9960_CONFIG_VSR_SLOW = 0,
  L9960_CONFIG_VSR_FAST,
  L9960_CONFIG_VSR_MAX,
}l9960_config_vsr_t;

typedef enum {
  L9960_CONFIG_ISR_SLOW = 0,
  L9960_CONFIG_ISR_FAST,
  L9960_CONFIG_ISR_MAX,
}l9960_config_isr_t;

typedef enum {
  L9960_CONFIG_NOSR_NOT_ALLOWED = 0,
  L9960_CONFIG_NOSR_ALLOWED,
  L9960_CONFIG_NOSR_MAX,
}l9960_config_nosr_t;

typedef enum {
  L9960_CONFIG_TDSR_NOT_ALLOWED = 0,
  L9960_CONFIG_TDSR_ALLOWED,
  L9960_CONFIG_TDSR_MAX,
}l9960_config_tdsr_t;

typedef enum {
  L9960_CONFIG_CL_RANGE0 = 0,
  L9960_CONFIG_CL_RANGE1,
  L9960_CONFIG_CL_RANGE2,
  L9960_CONFIG_CL_RANGE3,
  L9960_CONFIG_CL_MAX,
}l9960_config_cl_t;

typedef enum {
  L9960_CONFIG_UVW_20US = 0,
  L9960_CONFIG_UVW_40US,
  L9960_CONFIG_UVW_MAX,
}l9960_config_uvw_t;

typedef enum {
  L9960_CONFIG_UVP_DISABLED = 0,
  L9960_CONFIG_UVP_ENABLED,
  L9960_CONFIG_UVP_MAX,
}l9960_config_uvp_t;

typedef enum {
  L9960_CONFIG_SS_ENABLED = 0,
  L9960_CONFIG_SS_DISABLED,
  L9960_CONFIG_SS_MAX,
}l9960_config_ss_t;

typedef enum {
  L9960_CONFIG_OTW_0 = 0,
  L9960_CONFIG_OTW_M5,
  L9960_CONFIG_OTW_NOT_ALLOWED1,
  L9960_CONFIG_OTW_NOT_ALLOWED2,
  L9960_CONFIG_OTW_P5,
  L9960_CONFIG_OTW_P10,
  L9960_CONFIG_OTW_P15,
  L9960_CONFIG_OTW_P20,
  L9960_CONFIG_OTW_MAX,
}l9960_config_otw_t;

typedef enum {
  L9960_CONFIG_OTSD_0 = 0,
  L9960_CONFIG_OTSD_M5,
  L9960_CONFIG_OTSD_M10,
  L9960_CONFIG_OTSD_M15,
  L9960_CONFIG_OTSD_P5,
  L9960_CONFIG_OTSD_P10,
  L9960_CONFIG_OTSD_P15,
  L9960_CONFIG_OTSD_P20,
  L9960_CONFIG_OTSD_MAX,
}l9960_config_otsd_t;

typedef enum {
  L9960_CONFIG_IIF_PWM_DIR = 0,
  L9960_CONFIG_IIF_IN1_IN2,
  L9960_CONFIG_IIF_MAX,
}l9960_config_iif_t;

typedef enum {
  L9960_CONFIG_OTWTSE_DISABLED = 0,
  L9960_CONFIG_OTWTSE_ENABLED,
  L9960_CONFIG_OTWTSE_MAX,
}l9960_config_otwtse_t;

typedef enum {
  L9960_CONFIG_TVVL_0US = 0,
  L9960_CONFIG_TVVL_6US4,
  L9960_CONFIG_TVVL_12US6,
  L9960_CONFIG_TVVL_24US8,
  L9960_CONFIG_TVVL_50US4,
  L9960_CONFIG_TVVL_100US,
  L9960_CONFIG_TVVL_200US,
  L9960_CONFIG_TVVL_400US,
  L9960_CONFIG_TVVL_MAX,
}l9960_config_tvvl_t;

typedef enum {
  L9960_CONFIG_VVL_DISABLED = 0,
  L9960_CONFIG_VVL_ENABLED,
  L9960_CONFIG_VVL_MAX,
}l9960_config_vvl_t;

typedef enum {
  L9960_CONFIG_OLON_DISABLED = 0,
  L9960_CONFIG_OLON_ENABLED,
  L9960_CONFIG_OLON_MAX,
}l9960_config_olon_t;

typedef struct {
    l9960_config_cc_t comm_check;
    l9960_config_dce_t diag_clr_en;
    l9960_config_tlc_t tsw_low_current;
    l9960_config_td1_t tdiag1;
    l9960_config_vsr_t voltage_slew_rate;
    l9960_config_isr_t current_slow_rate;
    l9960_config_nosr_t no_slew_rate;
    l9960_config_tdsr_t temp_depent_slew_rate;
    l9960_config_cl_t current_limit;
    l9960_config_uvw_t undervoltage_window;
    l9960_config_ss_t spread_spectrum;
    l9960_config_uvp_t undervoltage_protection;
    l9960_config_otw_t otwarn_thr_var;
    l9960_config_otsd_t otsd_thr_var;
    l9960_config_iif_t in1_in2_if;
    l9960_config_otwtse_t otwarn_tsec_en;
    l9960_config_tvvl_t tvvl;
    l9960_config_vvl_t vvl_mode;
    l9960_config_olon_t ol_on;
}l9960_config_t;

#endif /* DEVICES_L9960_INC_L9960_CONFIG_H_ */
