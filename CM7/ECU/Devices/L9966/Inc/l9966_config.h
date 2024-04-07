/*
 * l9966_config.h
 *
 *  Created on: Apr 7, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_L9966_INC_L9966_CONFIG_H_
#define DEVICES_L9966_INC_L9966_CONFIG_H_

#include "l9966_reg.h"

#define L9966_CHANNELS        15u
#define L9966_SQNCR_CMDS      15u
#define L9966_SQNCR_COUNT     2u
#define L9966_SENT_CHANNELS   4u

typedef enum {
  L9966_CFG_CSC_CSS_FORCE0 = 0,
  L9966_CFG_CSC_CSS_CH1,
  L9966_CFG_CSC_CSS_CH2,
  L9966_CFG_CSC_CSS_CH3,
  L9966_CFG_CSC_CSS_CH4,
  L9966_CFG_CSC_CSS_CH5,
  L9966_CFG_CSC_CSS_CH6,
  L9966_CFG_CSC_CSS_CH7,
  L9966_CFG_CSC_CSS_CH8,
  L9966_CFG_CSC_CSS_CH9,
  L9966_CFG_CSC_CSS_CH10,
  L9966_CFG_CSC_CSS_CH11,
  L9966_CFG_CSC_CSS_CH12,
  L9966_CFG_CSC_CSS_CH13,
  L9966_CFG_CSC_CSS_CH14,
  L9966_CFG_CSC_CSS_CH15,
  L9966_CFG_CSC_CSS_MAX,
}l9966_config_csc_ccs_t;

typedef enum {
  L9966_CFG_CSC_CT_1 = 0,
  L9966_CFG_CSC_CT_2,
  L9966_CFG_CSC_CT_3,
  L9966_CFG_CSC_CT_RATIOMETRIC,
  L9966_CFG_CSC_CT_MAX,
}l9966_config_csc_ct_t;

typedef enum {
  L9966_CFG_CSC_CSV_0 = 0,
  L9966_CFG_CSC_CSV_1,
  L9966_CFG_CSC_CSV_2,
  L9966_CFG_CSC_CSV_3,
  L9966_CFG_CSC_CSV_4,
  L9966_CFG_CSC_CSV_5,
  L9966_CFG_CSC_CSV_6,
  L9966_CFG_CSC_CSV_7,
  L9966_CFG_CSC_CSV_MAX,
}l9966_config_csc_csv_t;

typedef enum {
  L9966_CFG_CSC_CSV_DW_5MA = 0,
  L9966_CFG_CSC_CSV_DW_10MA,
  L9966_CFG_CSC_CSV_DW_20MA,
  L9966_CFG_CSC_CSV_DW_CSV,
  L9966_CFG_CSC_CSV_DW_MAX,
}l9966_config_csc_csv_dw_t;

typedef enum {
  L9966_CFG_CSC_PU_PD_SEL_0 = 0,
  L9966_CFG_CSC_PU_PD_SEL_1,
  L9966_CFG_CSC_PU_PD_SEL_2,
  L9966_CFG_CSC_PU_PD_SEL_3,
  L9966_CFG_CSC_PU_PD_SEL_4,
  L9966_CFG_CSC_PU_PD_SEL_5,
  L9966_CFG_CSC_PU_PD_SEL_6,
  L9966_CFG_CSC_PU_PD_SEL_7,
  L9966_CFG_CSC_PU_PD_SEL_MAX,
}l9966_config_csc_pu_pd_sel_t;

typedef enum {
  L9966_CFG_CSC_CCP_INVERT_FALSE = 0,
  L9966_CFG_CSC_CCP_INVERT_TRUE,
  L9966_CFG_CSC_CCP_INVERT_MAX,
}l9966_config_csc_ccp_invert_t;

typedef struct {
    l9966_config_csc_ccs_t control_channel_selection;
    l9966_config_csc_ct_t comparator_threshold;
    l9966_config_csc_csv_t current_source_value;
    l9966_config_csc_csv_dw_t csv_dewetting_phase;
    l9966_config_csc_pu_pd_sel_t pu_pd_sel;
    l9966_config_csc_ccp_invert_t control_channel_invert;
}l9966_config_csc_t;

typedef enum {
  L9966_CFG_SR_AUX_EVENODD_CH_VRS = 0,
  L9966_CFG_SR_AUX_EVENODD_CH_CTRL_CFG,
  L9966_CFG_SR_AUX_EVENODD_CH_MAX,
}l9966_config_sr_aux_evenoddch_t;

typedef enum {
  L9966_CFG_SR_DF_DISABLED = 0,
  L9966_CFG_SR_DF_EN_1US_CH1 = 1,
  L9966_CFG_SR_DF_EN_1US_CH2 = 2,
  L9966_CFG_SR_DF_EN_1US_CH3 = 4,
  L9966_CFG_SR_DF_EN_1US_CH4 = 8,
  L9966_CFG_SR_DF_MAX = 16,
}l9966_config_sr_df_t;

typedef enum {
  L9966_CFG_SR_AOX_CH_SRC_HIZ = 0,
  L9966_CFG_SR_AOX_CH_SRC_1,
  L9966_CFG_SR_AOX_CH_SRC_2,
  L9966_CFG_SR_AOX_CH_SRC_3,
  L9966_CFG_SR_AOX_CH_SRC_4,
  L9966_CFG_SR_AOX_CH_SRC_5,
  L9966_CFG_SR_AOX_CH_SRC_6,
  L9966_CFG_SR_AOX_CH_SRC_7,
  L9966_CFG_SR_AOX_CH_SRC_8,
  L9966_CFG_SR_AOX_CH_SRC_9,
  L9966_CFG_SR_AOX_CH_SRC_10,
  L9966_CFG_SR_AOX_CH_SRC_11,
  L9966_CFG_SR_AOX_CH_SRC_12,
  L9966_CFG_SR_AOX_CH_SRC_BANDGAP,
  L9966_CFG_SR_AOX_CH_SRC_ADC_REF,
  L9966_CFG_SR_AOX_CH_SRC_VRSP,
  L9966_CFG_SR_AOX_CH_SRC_MAX,
}l9966_config_sr_aox_ch_src_t;

typedef enum {
  L9966_CFG_SR_RSENT_DISABLED = 0,
  L9966_CFG_SR_RSENT_ENABLED_CH1 = 1,
  L9966_CFG_SR_RSENT_ENABLED_CH2 = 2,
  L9966_CFG_SR_RSENT_ENABLED_CH3 = 4,
  L9966_CFG_SR_RSENT_ENABLED_CH4 = 8,
  L9966_CFG_SR_RSENT_MAX = 16,
}l9966_config_sr_rsent_en_t;

typedef struct {
    l9966_config_sr_aux_evenoddch_t aux_even_channel;
    l9966_config_sr_aux_evenoddch_t aux_odd_channel;
    l9966_config_sr_df_t sent_digital_filter_chx;
    l9966_config_sr_aox_ch_src_t aox_channel_src;
    l9966_config_sr_rsent_en_t rsent_enabled_chx;
}l9966_config_sr_t;

typedef enum {
  L9966_CFG_DVSLC_LE_DISABLED = 0,
  L9966_CFG_DVSLC_LE_ENABLED_CH1 = 1,
  L9966_CFG_DVSLC_LE_ENABLED_CH2 = 2,
  L9966_CFG_DVSLC_LE_ENABLED_CH3 = 4,
  L9966_CFG_DVSLC_LE_ENABLED_CH4 = 8,
  L9966_CFG_DVSLC_LE_MAX = 16,
}l9966_config_dvslc_le_t;

typedef enum {
  L9966_CFG_DVSLC_DTTC_2MS = 0,
  L9966_CFG_DVSLC_DTTC_16MS,
  L9966_CFG_DVSLC_DTTC_64MS,
  L9966_CFG_DVSLC_DTTC_128MS,
  L9966_CFG_DVSLC_DTTC_256MS,
  L9966_CFG_DVSLC_DTTC_512MS,
  L9966_CFG_DVSLC_DTTC_1024MS,
  L9966_CFG_DVSLC_DTTC_2048MS,
  L9966_CFG_DVSLC_DTTC_MAX,
}l9966_config_dvslc_dttc_t;

typedef enum {
  L9966_CFG_DVSLC_VVSS_800MV = 0,
  L9966_CFG_DVSLC_VVSS_850MV,
  L9966_CFG_DVSLC_VVSS_900MV,
  L9966_CFG_DVSLC_VVSS_950MV,
  L9966_CFG_DVSLC_VVSS_1000MV,
  L9966_CFG_DVSLC_VVSS_1050MV,
  L9966_CFG_DVSLC_VVSS_1100MV,
  L9966_CFG_DVSLC_VVSS_1150MV,
  L9966_CFG_DVSLC_VVSS_1200MV,
  L9966_CFG_DVSLC_VVSS_1250MV,
  L9966_CFG_DVSLC_VVSS_1300MV,
  L9966_CFG_DVSLC_VVSS_1350MV,
  L9966_CFG_DVSLC_VVSS_1400MV,
  L9966_CFG_DVSLC_VVSS_1450MV,
  L9966_CFG_DVSLC_VVSS_1500MV,
  L9966_CFG_DVSLC_VVSS_1550MV,
  L9966_CFG_DVSLC_VVSS_1600MV,
  L9966_CFG_DVSLC_VVSS_1650MV,
  L9966_CFG_DVSLC_VVSS_1700MV,
  L9966_CFG_DVSLC_VVSS_1750MV,
  L9966_CFG_DVSLC_VVSS_1800MV,
  L9966_CFG_DVSLC_VVSS_1850MV,
  L9966_CFG_DVSLC_VVSS_1900MV,
  L9966_CFG_DVSLC_VVSS_MAX,
}l9966_config_dvslc_vvss_t;

typedef struct {
    l9966_config_dvslc_le_t lambda_enabled_chx;
    l9966_config_dvslc_dttc_t dewetting_timer_time;
    l9966_config_dvslc_vvss_t var_volt_source_sel;
}l9966_config_dvslc_t;

typedef enum {
  L9966_CFG_GTSR_AF_SLOW = 0,
  L9966_CFG_GTSR_AF_FAST,
  L9966_CFG_GTSR_AF_MAX,
}l9966_config_gtsr_af_t;

typedef enum {
  L9966_CFG_GTSR_BP_OFF = 0,
  L9966_CFG_GTSR_BP_ON,
  L9966_CFG_GTSR_BP_MAX,
}l9966_config_gtsr_dbp_t;

typedef enum {
  L9966_CFG_GTSR_PU_OFF = 0,
  L9966_CFG_GTSR_PU_20K,
  L9966_CFG_GTSR_PU_MAX,
}l9966_config_gtsr_pu_t;

typedef enum {
  L9966_CFG_GTSR_SGR_HIZ,
  L9966_CFG_GTSR_SGR_CH1,
  L9966_CFG_GTSR_SGR_CH2,
  L9966_CFG_GTSR_SGR_CH3,
  L9966_CFG_GTSR_SGR_CH4,
  L9966_CFG_GTSR_SGR_CH5,
  L9966_CFG_GTSR_SGR_CH6,
  L9966_CFG_GTSR_SGR_CH7,
  L9966_CFG_GTSR_SGR_CH8,
  L9966_CFG_GTSR_SGR_CH9,
  L9966_CFG_GTSR_SGR_CH10,
  L9966_CFG_GTSR_SGR_CH11,
  L9966_CFG_GTSR_SGR_CH12,
  L9966_CFG_GTSR_SGR_CH13,
  L9966_CFG_GTSR_SGR_CH14,
  L9966_CFG_GTSR_SGR_CH15,
  L9966_CFG_GTSR_SGR_MAX,
}l9966_config_gtsr_sgr_t;

typedef struct {
    l9966_config_gtsr_af_t analog_filter;
    l9966_config_gtsr_dbp_t digital_bypass;
    l9966_config_gtsr_pu_t pullup;
    l9966_config_gtsr_sgr_t sent_gtm_route;
}l9966_config_gtsr_t;

typedef enum {
  L9966_CFG_ADLC_EN_DISABLED = 0,
  L9966_CFG_ADLC_EN_CH9 = 1,
  L9966_CFG_ADLC_EN_CH10 = 2,
  L9966_CFG_ADLC_EN_CH11 = 4,
  L9966_CFG_ADLC_EN_CH12 = 8,
  L9966_CFG_ADLC_EN_MAX = 16,
}l9966_config_adlc_en_t;

typedef enum {
  L9966_CFG_ADLC_TC_0MS = 0,
  L9966_CFG_ADLC_TC_1MS,
  L9966_CFG_ADLC_TC_2MS,
  L9966_CFG_ADLC_TC_3MS,
  L9966_CFG_ADLC_TC_4MS,
  L9966_CFG_ADLC_TC_5MS,
  L9966_CFG_ADLC_TC_6MS,
  L9966_CFG_ADLC_TC_7MS,
  L9966_CFG_ADLC_TC_8MS,
  L9966_CFG_ADLC_TC_9MS,
  L9966_CFG_ADLC_TC_10MS,
  L9966_CFG_ADLC_TC_11MS,
  L9966_CFG_ADLC_TC_12MS,
  L9966_CFG_ADLC_TC_13MS,
  L9966_CFG_ADLC_TC_14MS,
  L9966_CFG_ADLC_TC_15MS,
  L9966_CFG_ADLC_TC_MAX,
}l9966_config_adlc_tc_t;

typedef struct {
    l9966_config_adlc_en_t en_chx;
    l9966_config_adlc_tc_t time_cfg;
}l9966_config_adlc_t;

typedef enum {
  L9966_CFG_SC_WM_CLEAR = 0,
  L9966_CFG_SC_WM_CH1 = 1,
  L9966_CFG_SC_WM_CH2 = 2,
  L9966_CFG_SC_WM_CH3 = 4,
  L9966_CFG_SC_WM_CH4 = 8,
  L9966_CFG_SC_WM_CH5 = 16,
  L9966_CFG_SC_WM_CH6 = 32,
  L9966_CFG_SC_WM_CH7 = 64,
  L9966_CFG_SC_WM_CH8 = 128,
  L9966_CFG_SC_WM_CH9 = 256,
  L9966_CFG_SC_WM_CH10 = 512,
  L9966_CFG_SC_WM_CH11 = 1024,
  L9966_CFG_SC_WM_CH12 = 2048,
  L9966_CFG_SC_WM_CH13 = 4096,
  L9966_CFG_SC_WM_CH14 = 8192,
  L9966_CFG_SC_WM_CH15 = 16384,
  L9966_CFG_SC_WM_MAX = 32768,
}l9966_config_wm_t;

typedef enum {
  L9966_CFG_SC_SL_CLEAR = 0,

  L9966_CFG_SC_SL_CH1_LOW = 0,
  L9966_CFG_SC_SL_CH2_LOW = 0,
  L9966_CFG_SC_SL_CH3_LOW = 0,
  L9966_CFG_SC_SL_CH4_LOW = 0,
  L9966_CFG_SC_SL_CH5_LOW = 0,
  L9966_CFG_SC_SL_CH6_LOW = 0,
  L9966_CFG_SC_SL_CH7_LOW = 0,
  L9966_CFG_SC_SL_CH8_LOW = 0,
  L9966_CFG_SC_SL_CH9_LOW = 0,
  L9966_CFG_SC_SL_CH10_LOW = 0,
  L9966_CFG_SC_SL_CH11_LOW = 0,
  L9966_CFG_SC_SL_CH12_LOW = 0,
  L9966_CFG_SC_SL_CH13_LOW = 0,
  L9966_CFG_SC_SL_CH14_LOW = 0,
  L9966_CFG_SC_SL_CH15_LOW = 0,

  L9966_CFG_SC_SL_CH1_HIGH = 1,
  L9966_CFG_SC_SL_CH2_HIGH = 2,
  L9966_CFG_SC_SL_CH3_HIGH = 4,
  L9966_CFG_SC_SL_CH4_HIGH = 8,
  L9966_CFG_SC_SL_CH5_HIGH = 16,
  L9966_CFG_SC_SL_CH6_HIGH = 32,
  L9966_CFG_SC_SL_CH7_HIGH = 64,
  L9966_CFG_SC_SL_CH8_HIGH = 128,
  L9966_CFG_SC_SL_CH9_HIGH = 256,
  L9966_CFG_SC_SL_CH10_HIGH = 512,
  L9966_CFG_SC_SL_CH11_HIGH = 1024,
  L9966_CFG_SC_SL_CH12_HIGH = 2048,
  L9966_CFG_SC_SL_CH13_HIGH = 4096,
  L9966_CFG_SC_SL_CH14_HIGH = 8192,
  L9966_CFG_SC_SL_CH15_HIGH = 16384,
  L9966_CFG_SC_SL_MAX = 32768,
}l9966_config_sc_t;

typedef enum {
  L9966_CFG_WC_PBT_160US = 0,
  L9966_CFG_WC_PBT_80US,
  L9966_CFG_WC_PBT_40US,
  L9966_CFG_WC_PBT_16US,
  L9966_CFG_WC_PBT_MAX,
}l9966_config_wc_pbt_t;

typedef enum {
  L9966_CFG_WC_PTC_8MS = 0,
  L9966_CFG_WC_PTC_16MS,
  L9966_CFG_WC_PTC_24MS,
  L9966_CFG_WC_PTC_32MS,
  L9966_CFG_WC_PTC_MAX,
}l9966_config_wc_ptc_t;

typedef enum {
  L9966_CFG_WC_AC_FIRST_FRAME = 0x5,
  L9966_CFG_WC_AC_SECOND_FRAME = 0xA,
  L9966_CFG_WC_AC_THIRD_FRAME = 0xC,
  L9966_CFG_WC_AC_MAX,
}l9966_config_wc_ac_t;

typedef struct {
    l9966_config_wc_pbt_t polling_blanking_time;
    l9966_config_wc_ptc_t polling_time_cfg;
    l9966_config_wc_ac_t activation_code;
}l9966_config_wc_t;

typedef enum {
  L9966_CFG_VRS_DCC_DISABLED = 0,
  L9966_CFG_VRS_DCC_READOUT_CLEAR,
  L9966_CFG_VRS_DCC_MAX,
}l9966_config_vrs_dcc_t;

typedef enum {
  L9966_CFG_VRS_MHF_NO_FORCING = 0,
  L9966_CFG_VRS_MHF_ENABLED,
  L9966_CFG_VRS_MHF_MAX,
}l9966_config_vrs_mhf_t;

typedef enum {
  L9966_CFG_VRS_SEL_B_LIMITED = 0,
  L9966_CFG_VRS_SEL_A_FULLY,
  L9966_CFG_VRS_SEL_MAX,
}l9966_config_vrs_sel_t;

typedef enum {
  L9966_CFG_VRS_CM_AF_DIS_AH_DIS = 0,
  L9966_CFG_VRS_CM_AF_DIS_AH_EN,
  L9966_CFG_VRS_CM_AF_EN_AH_DIS,
  L9966_CFG_VRS_CM_AF_EN_AH_EN,
  L9966_CFG_VRS_CM_MAX,
}l9966_config_vrs_cm_t;

typedef enum {
  L9966_CFG_VRS_HM_HI3A_HI1B = 0,
  L9966_CFG_VRS_HM_HI1,
  L9966_CFG_VRS_HM_HI2,
  L9966_CFG_VRS_HM_HI3,
  L9966_CFG_VRS_HM_HI4,
  L9966_CFG_VRS_HM_HI5,
  L9966_CFG_VRS_HM_HI3A_HI1B_ALT,
  L9966_CFG_VRS_HM_HI_OFF,
  L9966_CFG_VRS_HM_MAX,
}l9966_config_vrs_hm_t;

typedef enum {
  L9966_CFG_VRS_DE_NORMAL = 0,
  L9966_CFG_VRS_DE_DIAG_ENABLED,
  L9966_CFG_VRS_DE_MAX,
}l9966_config_vrs_de_t;

typedef enum {
  L9966_CFG_VRS_HFBM_DIRECT_VRS = 0,
  L9966_CFG_VRS_HFBM_FILTER_VRS,
  L9966_CFG_VRS_HFBM_MAX,
}l9966_config_vrs_hfbm_t;

typedef enum {
  L9966_CFG_VRS_FFE_MASKING_EN = 0,
  L9966_CFG_VRS_FFE_FILTER_EN,
  L9966_CFG_VRS_FFE_MAX,
}l9966_config_vrs_ffe_t;

typedef struct {
    l9966_config_vrs_dcc_t diag_clr_cmd;
    l9966_config_vrs_mhf_t min_hyst_force;
    l9966_config_vrs_sel_t vrs_mode_sel;
    l9966_config_vrs_cm_t conf_mode;
    l9966_config_vrs_hm_t hyst_conf;
    l9966_config_vrs_de_t diag_en;
    l9966_config_vrs_hfbm_t hyst_fb_mode;
    l9966_config_vrs_ffe_t fe_filt_en;
}l9966_config_vrs_t;

typedef enum {
  L9966_CFG_SQNCR_PC_ENDLOOP = 0,
  L9966_CFG_SQNCR_PC_CMD1,
  L9966_CFG_SQNCR_PC_CMD2,
  L9966_CFG_SQNCR_PC_CMD3,
  L9966_CFG_SQNCR_PC_CMD4,
  L9966_CFG_SQNCR_PC_CMD5,
  L9966_CFG_SQNCR_PC_CMD6,
  L9966_CFG_SQNCR_PC_CMD7,
  L9966_CFG_SQNCR_PC_CMD8,
  L9966_CFG_SQNCR_PC_CMD9,
  L9966_CFG_SQNCR_PC_CMD10,
  L9966_CFG_SQNCR_PC_CMD11,
  L9966_CFG_SQNCR_PC_CMD12,
  L9966_CFG_SQNCR_PC_CMD13,
  L9966_CFG_SQNCR_PC_CMD14,
  L9966_CFG_SQNCR_PC_CMD15,
  L9966_CFG_SQNCR_PC_MAX,
}l9966_config_sqncr_pc_t;

typedef enum {
  L9966_CFG_SQNCR_IM_CLEAR = 0,
  L9966_CFG_SQNCR_IM_SC_MASK = 1,
  L9966_CFG_SQNCR_IM_EU1_MASK = 2,
  L9966_CFG_SQNCR_IM_EU2_MASK = 4,
  L9966_CFG_SQNCR_IM_MAX = 7,
}l9966_config_sqncr_im_t;

typedef enum {
  L9966_CFG_SQNCR_EOOF_CLEAR = 0,
  L9966_CFG_SQNCR_EOOF_SC_CPLT = 1,
  L9966_CFG_SQNCR_EOOF_EU1_EL_CL_REACH = 2,
  L9966_CFG_SQNCR_EOOF_EU2_EL_CL_REACH = 4,
  L9966_CFG_SQNCR_EOOF_MAX = 7,
}l9966_config_sqncr_eoof_t;

typedef struct {
    l9966_config_sqncr_pc_t int_eu1_pc_conf;
    l9966_config_sqncr_pc_t int_eu2_pc_conf;
    l9966_config_sqncr_im_t interrupt_mask;
    l9966_config_sqncr_eoof_t end_of_operation_flag;
    l9966_config_sqncr_eoof_t end_of_operation_flag;
}l9966_config_sqncr_imf_t;

typedef enum {
  L9966_CFG_AT_AVST_0US = 0,
  L9966_CFG_AT_AVST_8US,
  L9966_CFG_AT_AVST_16US,
  L9966_CFG_AT_AVST_24US,
  L9966_CFG_AT_AVST_32US,
  L9966_CFG_AT_AVST_40US,
  L9966_CFG_AT_AVST_48US,
  L9966_CFG_AT_AVST_56US,
  L9966_CFG_AT_AVST_MAX,
}l9966_config_at_avst_t;

typedef enum {
  L9966_CFG_AT_PUST_0US = 0,
  L9966_CFG_AT_PUST_200US,
  L9966_CFG_AT_PUST_400US,
  L9966_CFG_AT_PUST_600US,
  L9966_CFG_AT_PUST_800US,
  L9966_CFG_AT_PUST_1000US,
  L9966_CFG_AT_PUST_1200US,
  L9966_CFG_AT_PUST_1400US,
  L9966_CFG_AT_PUST_1600US,
  L9966_CFG_AT_PUST_1800US,
  L9966_CFG_AT_PUST_2000US,
  L9966_CFG_AT_PUST_2200US,
  L9966_CFG_AT_PUST_2400US,
  L9966_CFG_AT_PUST_2600US,
  L9966_CFG_AT_PUST_2800US,
  L9966_CFG_AT_PUST_3000US,
  L9966_CFG_AT_PUST_MAX,
}l9966_config_at_pust_t;

typedef struct {
    l9966_config_at_avst_t adc_volt_setting_time;
    l9966_config_at_pust_t pu1_setting_time;
    l9966_config_at_pust_t pu2_setting_time;
    l9966_config_at_pust_t pu3_setting_time;
}l9966_config_at_t;

typedef enum {
  L9966_CFG_SQNCR_CMD_PU_DISABLED = 0,
  L9966_CFG_SQNCR_CMD_PU_RR1 = 1,
  L9966_CFG_SQNCR_CMD_PU_RR2 = 2,
  L9966_CFG_SQNCR_CMD_PU_RR3 = 3,
  L9966_CFG_SQNCR_CMD_PU_MAX = 4,

  L9966_CFG_SQNCR_CMD_DIV_5V = 0,
  L9966_CFG_SQNCR_CMD_DIV_20V = 1,
  L9966_CFG_SQNCR_CMD_DIV_40V = 2,
  L9966_CFG_SQNCR_CMD_DIV_1V25 = 3,
  L9966_CFG_SQNCR_CMD_DIV_MAX = 4,
}l9966_config_sqncr_cmd_pd_t;

typedef enum {
  L9966_CFG_SQNCR_CMD_RVM_RESISTANCE = 0,
  L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
  L9966_CFG_SQNCR_CMD_RVM_MAX,
}l9966_config_sqncr_cmd_rvm_t;

typedef struct {
    l9966_config_sqncr_pc_t next_pc;
    l9966_config_sqncr_cmd_pd_t pu_div_sel;
    l9966_config_sqncr_cmd_rvm_t r_volt_sel;
}l9966_config_sqncr_cmd_t;

typedef enum {
  L9966_CFG_SQNCR_CTRL_SYNC_DISABLED = 0,
  L9966_CFG_SQNCR_CTRL_SYNC_ENABLED,
  L9966_CFG_SQNCR_CTRL_SYNC_MAX,
}l9966_config_sqncr_ctrl_se_t;

typedef enum {
  L9966_CFG_SQNCR_CTRL_SCC_DISABLED = 0,
  L9966_CFG_SQNCR_CTRL_SCC_ENABLED,
  L9966_CFG_SQNCR_CTRL_SCC_MAX,
}l9966_config_sqncr_scce_t;

typedef struct {
    l9966_config_sqncr_ctrl_se_t sync_pin_enabled;
    l9966_config_sqncr_pc_t init_pc;
}l9966_config_sqncr_ctrl_eu_t;

typedef struct {
    l9966_config_sqncr_ctrl_eu_t sqncr_cfg[L9966_SQNCR_COUNT];
    l9966_config_sqncr_scce_t sync_copy_cmd_en;
}l9966_config_sqncr_ctrl_t;

typedef struct {
    l9966_config_sqncr_imf_t interrupt_mask_flag;
    l9966_config_sqncr_ctrl_t ctrl_config;
    l9966_config_sqncr_cmd_t cmd_config[L9966_SQNCR_CMDS];
}l9966_config_sqncr_t;

typedef struct {
    l9966_config_csc_t curr_src_ctrl[L9966_CHANNELS];
    l9966_config_sr_t switch_route;
    l9966_config_dvslc_t dwt_volt_src_lsf_ctrl;
    l9966_config_gtsr_t gtm_to_sent_route[L9966_SENT_CHANNELS];
    l9966_config_adlc_t active_discharge_lsf;
    l9966_config_wm_t wak_mask;
    l9966_config_sc_t sleep_config;
    l9966_config_wc_t wake_config;
    l9966_config_vrs_t vrs_config;
    l9966_config_at_t adc_timing;
    l9966_config_sqncr_t sequencer_config;

}l9966_config_t;

#endif /* DEVICES_L9966_INC_L9966_CONFIG_H_ */
