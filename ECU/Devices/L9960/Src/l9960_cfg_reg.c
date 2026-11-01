/*
 * l9960_cfg_reg.c
 *
 *  Created on: Apr 18, 2024
 *      Author: VHEMaster
 */

#include "l9960_cfg_reg.h"
#include "compiler.h"

error_t l9960_cfg_reg_translate(l9960_ctx_t *ctx)
{
  error_t err = E_OK;
  l9960_config_t *config;
  l9960_reg_cfg_t *reg;

  do {
    BREAK_IF_ACTION(ctx == NULL, err = E_PARAM);
    config = &ctx->config;
    reg = &ctx->reg_cfg;

    BREAK_IF_ACTION(config->comm_check >= L9960_CONFIG_CC_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->diag_clr_en >= L9960_CONFIG_DCE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->tsw_low_current >= L9960_CONFIG_TLC_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->tdiag1 >= L9960_CONFIG_TD1_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->voltage_slew_rate >= L9960_CONFIG_VSR_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->current_slow_rate >= L9960_CONFIG_ISR_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->no_slew_rate >= L9960_CONFIG_NOSR_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->temp_depent_slew_rate >= L9960_CONFIG_TDSR_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->current_limit >= L9960_CONFIG_CL_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->undervoltage_window >= L9960_CONFIG_UVW_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->spread_spectrum >= L9960_CONFIG_SS_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->undervoltage_protection >= L9960_CONFIG_UVP_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->otwarn_thr_var >= L9960_CONFIG_OTW_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->otsd_thr_var >= L9960_CONFIG_OTSD_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->in1_in2_if >= L9960_CONFIG_IIF_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->otwarn_tsec_en >= L9960_CONFIG_OTWTSE_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->tvvl >= L9960_CONFIG_TVVL_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->vvl_mode >= L9960_CONFIG_VVL_MAX, err = E_PARAM);
    BREAK_IF_ACTION(config->ol_on >= L9960_CONFIG_OLON_MAX, err = E_PARAM);

    reg->config1.bits.diag_clr_en = config->diag_clr_en;
    reg->config1.bits.tsw_low_current = config->tsw_low_current;
    reg->config1.bits.tdiag1 = config->tdiag1;
    reg->config1.bits.vsr = config->voltage_slew_rate;
    reg->config1.bits.isr = config->current_slow_rate;
    reg->config1.bits.nosr = config->no_slew_rate;
    reg->config1.bits.cl = config->current_limit;
    reg->config2.bits.uv_win = config->undervoltage_window;
    reg->config2.bits.nspread = config->spread_spectrum;
    reg->config2.bits.uv_prot_en = config->undervoltage_protection;
    reg->config2.bits.otwarn_thr_var = config->otwarn_thr_var;
    reg->config2.bits.otsd_thr_var = config->otsd_thr_var;
    reg->config2.bits.in1_in2_if = config->in1_in2_if;
    reg->config3.bits.otwarn_tsec_en = config->otwarn_tsec_en;
    reg->config3.bits.tvvl = config->tvvl;
    reg->config3.bits.vvl_mode = config->vvl_mode;
    reg->config4.bits.ol_on = config->ol_on;
    reg->config4.bits.tdsr = config->temp_depent_slew_rate;

  } while(0);

  return err;
}
