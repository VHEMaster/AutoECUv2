/*
 * l9966_cfg_reg.c
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#include "l9966_cfg_reg.h"
#include "compiler.h"

error_t l9966_cfg_reg_translate(l9966_ctx_t *ctx)
{
  error_t err = E_OK;
  l9966_config_t *config;
  l9966_reg_cfg_map_t *reg;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    config = &ctx->config;
    reg = &ctx->register_map;

    BREAK_IF_ACTION(config->status.calib_sel >= L9966_CONFIG_GS_CS_MAX, err = E_PARAM);
    reg->data.status.u.bits.CALIB_SEL = config->status.calib_sel;

    BREAK_IF_ACTION(config->status.config_check >= L9966_CONFIG_GS_CC_MAX || config->status.config_check == L9966_CONFIG_GS_CC_LOST, err = E_PARAM);
    reg->data.status.u.bits.CONFIG_CHECK = config->status.config_check;

    BREAK_IF_ACTION(config->status.overtemperature_mask >= L9966_CONFIG_GS_OTM_MAX, err = E_PARAM);
    reg->data.status.u.bits.OT_MASK = config->status.overtemperature_mask;

    for(int i = 0; i < ITEMSOF(config->curr_src_ctrl); i++) {
      if(i >= 4 || config->gtm_to_sent_route[i].pullup == L9966_CFG_GTSR_PU_OFF) {
        if((i == 8 && (config->dwt_volt_src_lsf_ctrl.lambda_enabled_chx & L9966_CFG_DVSLC_LE_ENABLED_CH1) != 0) ||
            (i == 9 && (config->dwt_volt_src_lsf_ctrl.lambda_enabled_chx & L9966_CFG_DVSLC_LE_ENABLED_CH2) != 0) ||
            (i == 10 && (config->dwt_volt_src_lsf_ctrl.lambda_enabled_chx & L9966_CFG_DVSLC_LE_ENABLED_CH3) != 0) ||
            (i == 11 && (config->dwt_volt_src_lsf_ctrl.lambda_enabled_chx & L9966_CFG_DVSLC_LE_ENABLED_CH4) != 0)) {
          BREAK_IF_ACTION(config->curr_src_ctrl[i].current_source_value < L9966_CFG_CSC_CSV_4, err = E_PARAM);
          BREAK_IF_ACTION(config->curr_src_ctrl[i].pu_pd_sel != L9966_CFG_CSC_PU_PD_SEL_7, err = E_PARAM);
        }

        BREAK_IF_ACTION(config->curr_src_ctrl[i].control_channel_selection >= L9966_CFG_CSC_CSS_MAX, err = E_PARAM);
        reg->data.curr_src_ctrl[i].u.bits.CIS = config->curr_src_ctrl[i].control_channel_selection;

        BREAK_IF_ACTION(config->curr_src_ctrl[i].comparator_threshold >= L9966_CFG_CSC_CT_MAX, err = E_PARAM);
        reg->data.curr_src_ctrl[i].u.bits.CTR = config->curr_src_ctrl[i].comparator_threshold;

        BREAK_IF_ACTION(config->curr_src_ctrl[i].current_source_value >= L9966_CFG_CSC_CSV_MAX, err = E_PARAM);
        reg->data.curr_src_ctrl[i].u.bits.CV = config->curr_src_ctrl[i].current_source_value;

        BREAK_IF_ACTION(config->curr_src_ctrl[i].csv_dewetting_phase >= L9966_CFG_CSC_CSV_DW_MAX, err = E_PARAM);
        reg->data.curr_src_ctrl[i].u.bits.CV_DW = config->curr_src_ctrl[i].csv_dewetting_phase;

        BREAK_IF_ACTION(config->curr_src_ctrl[i].pu_pd_sel >= L9966_CFG_CSC_PU_PD_SEL_MAX, err = E_PARAM);
        reg->data.curr_src_ctrl[i].u.bits.SEL = config->curr_src_ctrl[i].pu_pd_sel;

        BREAK_IF_ACTION(config->curr_src_ctrl[i].control_channel_invert >= L9966_CFG_CSC_CCP_INVERT_MAX, err = E_PARAM);
        reg->data.curr_src_ctrl[i].u.bits.MODE = config->curr_src_ctrl[i].control_channel_invert;

      } else if(config->gtm_to_sent_route[i].pullup == L9966_CFG_GTSR_PU_20K) {
        BREAK_IF_ACTION(config->curr_src_ctrl[i].comparator_threshold >= L9966_CFG_CSC_CT_MAX, err = E_PARAM);
        reg->data.curr_src_ctrl[i].u.bits.CIS = L9966_CFG_CSC_CSS_FORCE0;
        reg->data.curr_src_ctrl[i].u.bits.CTR = config->curr_src_ctrl[i].comparator_threshold;
        reg->data.curr_src_ctrl[i].u.bits.CV = L9966_CFG_CSC_CSV_0;
        reg->data.curr_src_ctrl[i].u.bits.CV_DW = L9966_CFG_CSC_CSV_DW_CSV;
        reg->data.curr_src_ctrl[i].u.bits.SEL = L9966_CFG_CSC_PU_PD_SEL_0;
        reg->data.curr_src_ctrl[i].u.bits.MODE = L9966_CFG_CSC_CCP_INVERT_FALSE;
      }
    }
    BREAK_IF(err != E_OK);

    BREAK_IF_ACTION(config->switch_route.aux_even_channel >= L9966_CFG_SR_AUX_EVENODD_CH_MAX, err = E_PARAM);
    reg->data.switch_route.u.bits.AUX_EVENCH = config->switch_route.aux_even_channel;

    BREAK_IF_ACTION(config->switch_route.aux_odd_channel >= L9966_CFG_SR_AUX_EVENODD_CH_MAX, err = E_PARAM);
    reg->data.switch_route.u.bits.AUX_ODDCH = config->switch_route.aux_odd_channel;

    BREAK_IF_ACTION(config->switch_route.sent_digital_filter_chx >= L9966_CFG_SR_DF_MAX, err = E_PARAM);
    reg->data.switch_route.u.bits.DIG_FILx = config->switch_route.sent_digital_filter_chx;

    BREAK_IF_ACTION(config->switch_route.aox_channel_src >= L9966_CFG_SR_AOX_CH_SRC_MAX, err = E_PARAM);
    reg->data.switch_route.u.bits.RAOX = config->switch_route.aox_channel_src;

    BREAK_IF_ACTION(config->switch_route.rsent_enabled_chx >= L9966_CFG_SR_RSENT_MAX, err = E_PARAM);
    reg->data.switch_route.u.bits.RSENTx = config->switch_route.rsent_enabled_chx;


    BREAK_IF_ACTION(config->dwt_volt_src_lsf_ctrl.lambda_enabled_chx >= L9966_CFG_DVSLC_LE_MAX, err = E_PARAM);
    reg->data.dwt_volt_src_lsf_ctrl.u.bits.LSF_MDx = config->dwt_volt_src_lsf_ctrl.lambda_enabled_chx;

    BREAK_IF_ACTION(config->dwt_volt_src_lsf_ctrl.dewetting_timer_time >= L9966_CFG_DVSLC_DTTC_MAX, err = E_PARAM);
    reg->data.dwt_volt_src_lsf_ctrl.u.bits.DWTC = config->dwt_volt_src_lsf_ctrl.dewetting_timer_time;

    BREAK_IF_ACTION(config->dwt_volt_src_lsf_ctrl.var_volt_source_sel >= L9966_CFG_DVSLC_VVSS_MAX, err = E_PARAM);
    reg->data.dwt_volt_src_lsf_ctrl.u.bits.VVAR_VF = config->dwt_volt_src_lsf_ctrl.var_volt_source_sel;


    BREAK_IF_ACTION(config->gtm_to_sent_route[0].analog_filter >= L9966_CFG_GTSR_AF_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[1].analog_filter >= L9966_CFG_GTSR_AF_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[2].analog_filter >= L9966_CFG_GTSR_AF_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[3].analog_filter >= L9966_CFG_GTSR_AF_MAX, err = E_PARAM);
    reg->data.gtm_to_sent_route[0].u.bits.AN_FIL = config->gtm_to_sent_route[0].analog_filter == L9966_CFG_GTSR_AF_FAST ? 0x1 : 0;
    reg->data.gtm_to_sent_route[0].u.bits.AN_FIL |= config->gtm_to_sent_route[1].analog_filter == L9966_CFG_GTSR_AF_FAST ? 0x2 : 0;
    reg->data.gtm_to_sent_route[1].u.bits.AN_FIL = config->gtm_to_sent_route[2].analog_filter == L9966_CFG_GTSR_AF_FAST ? 0x1 : 0;
    reg->data.gtm_to_sent_route[1].u.bits.AN_FIL |= config->gtm_to_sent_route[3].analog_filter == L9966_CFG_GTSR_AF_FAST ? 0x2 : 0;

    BREAK_IF_ACTION(config->gtm_to_sent_route[0].digital_bypass >= L9966_CFG_GTSR_BP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[1].digital_bypass >= L9966_CFG_GTSR_BP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[2].digital_bypass >= L9966_CFG_GTSR_BP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[3].digital_bypass >= L9966_CFG_GTSR_BP_MAX, err = E_PARAM);
    reg->data.gtm_to_sent_route[0].u.bits.DIG_BP = config->gtm_to_sent_route[0].digital_bypass == L9966_CFG_GTSR_BP_ON ? 0x1 : 0;
    reg->data.gtm_to_sent_route[0].u.bits.DIG_BP |= config->gtm_to_sent_route[1].digital_bypass == L9966_CFG_GTSR_BP_ON ? 0x2 : 0;
    reg->data.gtm_to_sent_route[1].u.bits.DIG_BP = config->gtm_to_sent_route[2].digital_bypass == L9966_CFG_GTSR_BP_ON ? 0x1 : 0;
    reg->data.gtm_to_sent_route[1].u.bits.DIG_BP |= config->gtm_to_sent_route[3].digital_bypass == L9966_CFG_GTSR_BP_ON ? 0x2 : 0;

    BREAK_IF_ACTION(config->gtm_to_sent_route[0].pullup >= L9966_CFG_GTSR_PU_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[1].pullup >= L9966_CFG_GTSR_PU_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[2].pullup >= L9966_CFG_GTSR_PU_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->gtm_to_sent_route[3].pullup >= L9966_CFG_GTSR_PU_MAX, err = E_PARAM);
    reg->data.gtm_to_sent_route[0].u.bits.R20kPU = config->gtm_to_sent_route[0].pullup == L9966_CFG_GTSR_PU_20K ? 0x1 : 0;
    reg->data.gtm_to_sent_route[0].u.bits.R20kPU |= config->gtm_to_sent_route[1].pullup == L9966_CFG_GTSR_PU_20K ? 0x2 : 0;
    reg->data.gtm_to_sent_route[1].u.bits.R20kPU = config->gtm_to_sent_route[2].pullup == L9966_CFG_GTSR_PU_20K ? 0x1 : 0;
    reg->data.gtm_to_sent_route[1].u.bits.R20kPU |= config->gtm_to_sent_route[3].pullup == L9966_CFG_GTSR_PU_20K ? 0x2 : 0;

    BREAK_IF_ACTION(config->gtm_to_sent_route[0].sent_gtm_route >= L9966_CFG_GTSR_SGR_MAX, err = E_PARAM);
    reg->data.gtm_to_sent_route[0].u.bits.SENTo_GTM_ROUTE = config->gtm_to_sent_route[0].sent_gtm_route;
    BREAK_IF_ACTION(config->gtm_to_sent_route[1].sent_gtm_route >= L9966_CFG_GTSR_SGR_MAX, err = E_PARAM);
    reg->data.gtm_to_sent_route[0].u.bits.SENTe_GTM_ROUTE = config->gtm_to_sent_route[1].sent_gtm_route;
    BREAK_IF_ACTION(config->gtm_to_sent_route[2].sent_gtm_route >= L9966_CFG_GTSR_SGR_MAX, err = E_PARAM);
    reg->data.gtm_to_sent_route[1].u.bits.SENTo_GTM_ROUTE = config->gtm_to_sent_route[2].sent_gtm_route;
    BREAK_IF_ACTION(config->gtm_to_sent_route[3].sent_gtm_route >= L9966_CFG_GTSR_SGR_MAX, err = E_PARAM);
    reg->data.gtm_to_sent_route[1].u.bits.SENTe_GTM_ROUTE = config->gtm_to_sent_route[3].sent_gtm_route;


    BREAK_IF_ACTION(config->active_discharge_lsf.en_chx >= L9966_CFG_ADLC_EN_MAX, err = E_PARAM);
    reg->data.active_discharge_lsf_ctrl.u.bits.EN_CHx = config->active_discharge_lsf.en_chx;

    BREAK_IF_ACTION(config->active_discharge_lsf.time_cfg >= L9966_CFG_ADLC_TC_MAX, err = E_PARAM);
    reg->data.active_discharge_lsf_ctrl.u.bits.TIME_CFG = config->active_discharge_lsf.time_cfg;


    BREAK_IF_ACTION(config->wak_mask >= L9966_CFG_SC_WM_MAX, err = E_PARAM);
    reg->data.wak_msk.u.bits.MSK_INx = config->wak_mask;

    BREAK_IF_ACTION(config->sleep_config >= L9966_CFG_SC_SL_MAX, err = E_PARAM);
    reg->data.sleep_config.u.bits.TGT_INx = config->sleep_config;

    BREAK_IF_ACTION(config->sequencer_config.interrupt_mask_flag.int_eu1_pc_conf >= L9966_CFG_SQNCR_PC_MAX, err = E_PARAM);
    reg->data.sqncr_int_msk_flg.u.bits.INT_EU1_CONF = config->sequencer_config.interrupt_mask_flag.int_eu1_pc_conf;

    BREAK_IF_ACTION(config->sequencer_config.interrupt_mask_flag.int_eu2_pc_conf >= L9966_CFG_SQNCR_PC_MAX, err = E_PARAM);
    reg->data.sqncr_int_msk_flg.u.bits.INT_EU2_CONF = config->sequencer_config.interrupt_mask_flag.int_eu2_pc_conf;

    BREAK_IF_ACTION(config->sequencer_config.interrupt_mask_flag.interrupt_mask >= L9966_CFG_SQNCR_IM_MAX, err = E_PARAM);
    reg->data.sqncr_int_msk_flg.u.bits.CFGx = config->sequencer_config.interrupt_mask_flag.interrupt_mask;

    BREAK_IF_ACTION(config->vrs_config.double_edge_detection >= L9966_CFG_VRS_DED_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.LIM_ADAP_DOUBLE_EDGE = config->vrs_config.double_edge_detection;

    BREAK_IF_ACTION(config->vrs_config.diag_clr_cmd >= L9966_CFG_VRS_DCC_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.DIAG_CLR_CMD = config->vrs_config.diag_clr_cmd;

    BREAK_IF_ACTION(config->vrs_config.min_hyst_force >= L9966_CFG_VRS_MHF_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.MIN_HYST_FORCE = config->vrs_config.min_hyst_force;

    BREAK_IF_ACTION(config->vrs_config.vrs_mode_sel >= L9966_CFG_VRS_SEL_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.VRS_SEL = config->vrs_config.vrs_mode_sel;

    BREAK_IF_ACTION(config->vrs_config.conf_mode >= L9966_CFG_VRS_CM_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.VRS_CONF_MODE = config->vrs_config.conf_mode;

    BREAK_IF_ACTION(config->vrs_config.hyst_conf >= L9966_CFG_VRS_HM_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.VRS_HYST_CONF = config->vrs_config.hyst_conf;

    BREAK_IF_ACTION(config->vrs_config.diag_en >= L9966_CFG_VRS_DE_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.VRS_EN_DIAG = config->vrs_config.diag_en;

    BREAK_IF_ACTION(config->vrs_config.hyst_fb_mode >= L9966_CFG_VRS_HFBM_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.VRS_HYST_FB_MODE = config->vrs_config.hyst_fb_mode;

    BREAK_IF_ACTION(config->vrs_config.fe_filt_en >= L9966_CFG_VRS_FFE_MAX, err = E_PARAM);
    reg->data.reg_vrs.u.bits.VRS_FE_FILT_EN = config->vrs_config.fe_filt_en;


    BREAK_IF_ACTION(config->adc_timing.adc_volt_setting_time >= L9966_CFG_AT_AVST_MAX, err = E_PARAM);
    reg->data.adc_timing.u.bits.CT_AD = config->adc_timing.adc_volt_setting_time;

    BREAK_IF_ACTION(config->adc_timing.pu1_setting_time >= L9966_CFG_AT_PUST_MAX, err = E_PARAM);
    reg->data.adc_timing.u.bits.CT_PU1 = config->adc_timing.pu1_setting_time;

    BREAK_IF_ACTION(config->adc_timing.pu2_setting_time >= L9966_CFG_AT_PUST_MAX, err = E_PARAM);
    reg->data.adc_timing.u.bits.CT_PU2 = config->adc_timing.pu2_setting_time;

    BREAK_IF_ACTION(config->adc_timing.pu3_setting_time >= L9966_CFG_AT_PUST_MAX, err = E_PARAM);
    reg->data.adc_timing.u.bits.CT_PU3 = config->adc_timing.pu3_setting_time;

    for(int i = 0; i < ITEMSOF(config->sequencer_config.control.sqncr_cfg); i++) {
      BREAK_IF_ACTION(config->sequencer_config.control.sqncr_cfg[i].init_pc >= L9966_CFG_SQNCR_PC_MAX, err = E_PARAM);
      BREAK_IF_ACTION(config->sequencer_config.control.sqncr_cfg[i].sync_pin_enabled >= L9966_CFG_SQNCR_CONFIG_SYNC_MAX, err = E_PARAM);
    }
    BREAK_IF_ACTION(config->sequencer_config.control.sync_copy_cmd_en >= L9966_CFG_SQNCR_CONFIG_SCC_MAX, err = E_PARAM);
    reg->data.sqncr_ctrl.u.bits.INIT_PC_EU1 = config->sequencer_config.control.sqncr_cfg[0].init_pc;
    reg->data.sqncr_ctrl.u.bits.INIT_PC_EU2 = config->sequencer_config.control.sqncr_cfg[1].init_pc;
    reg->data.sqncr_ctrl.u.bits.SYNC_COPY_CMD_EN = config->sequencer_config.control.sync_copy_cmd_en;
    reg->data.sqncr_ctrl.u.bits.EU1_EN = 0;
    reg->data.sqncr_ctrl.u.bits.EU2_EN = 0;
    reg->data.sqncr_ctrl.u.bits.EU1_SYNC_EN = 0;
    reg->data.sqncr_ctrl.u.bits.EU2_SYNC_EN = 0;

    for(int i = 0; i < ITEMSOF(config->sequencer_config.cmd_config); i++) {
      BREAK_IF_ACTION(config->sequencer_config.cmd_config[i].next_pc >= L9966_CFG_SQNCR_PC_MAX, err = E_PARAM);
      reg->data.sqncr_cmd[i].u.bits.NXT_PC = config->sequencer_config.cmd_config[i].next_pc;

      BREAK_IF_ACTION(config->sequencer_config.cmd_config[i].r_volt_sel >= L9966_CFG_SQNCR_CMD_RVM_MAX, err = E_PARAM);
      reg->data.sqncr_cmd[i].u.bits.R_VOLT_MEAS_SELECT = config->sequencer_config.cmd_config[i].r_volt_sel;

      if(config->sequencer_config.cmd_config[i].r_volt_sel == L9966_CFG_SQNCR_CMD_RVM_RESISTANCE) {
        BREAK_IF_ACTION(config->sequencer_config.cmd_config[i].pu_div_sel >= L9966_CFG_SQNCR_CMD_PU_MAX, err = E_PARAM);
        reg->data.sqncr_cmd[i].u.bits.PUP_DIV = config->sequencer_config.cmd_config[i].pu_div_sel;
      } else if(config->sequencer_config.cmd_config[i].r_volt_sel == L9966_CFG_SQNCR_CMD_RVM_VOLTAGE) {
        BREAK_IF_ACTION(config->sequencer_config.cmd_config[i].pu_div_sel >= L9966_CFG_SQNCR_CMD_DIV_MAX, err = E_PARAM);
        reg->data.sqncr_cmd[i].u.bits.PUP_DIV = config->sequencer_config.cmd_config[i].pu_div_sel;
      }
    }
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}
