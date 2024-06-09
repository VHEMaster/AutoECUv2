/*
 * config_flexio.c
 *
 *  Created on: Apr 9, 2024
 *      Author: VHEMaster
 */

#include <string.h>
#include "config_gpio.h"
#include "config_flexio.h"
#include "middlelayer_spi.h"
#include "compiler.h"

typedef struct {
    ecu_spi_slave_enum_t slave_index;
    l9966_init_ctx_t init;
    l9966_config_t config_default;
    l9966_ctx_t *ctx;
    uint16_t int_exti_pin;
}ecu_devices_flexio_ctx_t;

static const l9966_config_t ecu_devices_flexio_config_default[ECU_DEVICE_FLEXIO_MAX] = {
    {
      .config_data = {
        .curr_src_ctrl = {
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO1
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO2
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO3
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO4
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO5
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO6
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO7
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO8
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO9
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO10
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO11
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO12
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO13
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO14
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO15
        },
        .switch_route = {
            .aux_even_channel = L9966_CFG_SR_AUX_EVENODD_CH_VRS,
            .aux_odd_channel = L9966_CFG_SR_AUX_EVENODD_CH_VRS,
            .sent_digital_filter_chx = L9966_CFG_SR_DF_EN_1US_CH1 | L9966_CFG_SR_DF_EN_1US_CH2 | L9966_CFG_SR_DF_EN_1US_CH3 | L9966_CFG_SR_DF_EN_1US_CH4,
            .aox_channel_src = L9966_CFG_SR_AOX_CH_SRC_5,
            .rsent_enabled_chx = L9966_CFG_SR_RSENT_DISABLED,
        },
        .dwt_volt_src_lsf_ctrl = {
            .lambda_enabled_chx = L9966_CFG_DVSLC_LE_DISABLED,
            .dewetting_timer_time = L9966_CFG_DVSLC_DTTC_2MS,
            .var_volt_source_sel = L9966_CFG_DVSLC_VVSS_1000MV,
        },
        .gtm_to_sent_route = {
            {
                .analog_filter = L9966_CFG_GTSR_AF_FAST,
                .digital_bypass = L9966_CFG_GTSR_BP_ON,
                .pullup = L9966_CFG_GTSR_PU_OFF,
                .sent_gtm_route = L9966_CFG_GTSR_SGR_CH1,
            }, //SENT1
            {
                .analog_filter = L9966_CFG_GTSR_AF_FAST,
                .digital_bypass = L9966_CFG_GTSR_BP_ON,
                .pullup = L9966_CFG_GTSR_PU_OFF,
                .sent_gtm_route = L9966_CFG_GTSR_SGR_CH2,
            }, //SENT2
            {
                .analog_filter = L9966_CFG_GTSR_AF_FAST,
                .digital_bypass = L9966_CFG_GTSR_BP_ON,
                .pullup = L9966_CFG_GTSR_PU_OFF,
                .sent_gtm_route = L9966_CFG_GTSR_SGR_CH3,
            }, //SENT3
            {
                .analog_filter = L9966_CFG_GTSR_AF_FAST,
                .digital_bypass = L9966_CFG_GTSR_BP_ON,
                .pullup = L9966_CFG_GTSR_PU_OFF,
                .sent_gtm_route = L9966_CFG_GTSR_SGR_CH4,
            }, //SENT4
        },
        .active_discharge_lsf = {
            .en_chx = L9966_CFG_ADLC_EN_DISABLED,
            .time_cfg = L9966_CFG_ADLC_TC_4MS,
        },
        .wak_mask = L9966_CFG_SC_WM_CH15,
        .sleep_config = L9966_CFG_SC_SL_CH15_LOW,
        .wake_config = {
            .polling_blanking_time = L9966_CFG_WC_PBT_160US,
            .polling_time_cfg = L9966_CFG_WC_PTC_32MS,
        },
        .vrs_config = {
            .double_edge_detection = L9966_CFG_VRS_DED_DISABLED,
            .diag_clr_cmd = L9966_CFG_VRS_DCC_READOUT_CLEAR,
            .min_hyst_force = L9966_CFG_VRS_MHF_NO_FORCING,
            .vrs_mode_sel = L9966_CFG_VRS_SEL_B_LIMITED,
            .conf_mode = L9966_CFG_VRS_CM_AF_EN_AH_EN,
            .hyst_conf = L9966_CFG_VRS_HM_HI3,
            .diag_en = L9966_CFG_VRS_DE_NORMAL,
            .hyst_fb_mode = L9966_CFG_VRS_HFBM_DIRECT_VRS,
            .fe_filt_en = L9966_CFG_VRS_FFE_MASKING_EN,
        },
        .adc_timing = {
            .adc_volt_setting_time = L9966_CFG_AT_AVST_16US,
            .pu1_setting_time = L9966_CFG_AT_PUST_200US,
            .pu2_setting_time = L9966_CFG_AT_PUST_400US,
            .pu3_setting_time = L9966_CFG_AT_PUST_600US,
        },
        .sequencer_config = {
            .interrupt_mask_flag = {
                .int_eu1_pc_conf = L9966_CFG_SQNCR_PC_CMD15,
                .int_eu2_pc_conf = L9966_CFG_SQNCR_PC_ENDLOOP,
                .interrupt_mask = L9966_CFG_SQNCR_IM_CLEAR,
            },
            .control = {
                .sqncr_cfg = {
                    {
                        .sync_pin_enabled = L9966_CFG_SQNCR_CONFIG_SYNC_DISABLED,
                        .init_pc = L9966_CFG_SQNCR_PC_CMD1,
                    }, //EU1
                    {
                        .sync_pin_enabled = L9966_CFG_SQNCR_CONFIG_SYNC_DISABLED,
                        .init_pc = L9966_CFG_SQNCR_PC_ENDLOOP,
                    }, //EU2
                },
                .sync_copy_cmd_en = L9966_CFG_SQNCR_CONFIG_SCC_ENABLED,
            },
            .cmd_config = {
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD2,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD1
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD3,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD2
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD4,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD3
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD5,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD4
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD6,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD5
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD7,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD6
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD8,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD7
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD9,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD8
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD10,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD9
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD11,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD10
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD12,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD11
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD13,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD12
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD14,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_INTERNAL,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 0.05f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD13
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD15,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_INTERNAL,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 0.05f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD14
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD1,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_INTERNAL,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 0.05f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD15
            }
        },
        .status = {
            .config_check = L9966_CONFIG_GS_CC_3,
            .calib_sel = L9966_CONFIG_GS_CS_ADC_CALIBRATED,
            .overtemperature_mask = L9966_CONFIG_GS_OTM_ENABLED,
        },
      },
      .rrx = { 220.0f, 2700.0f, 33000.0f },
      .rrx_switch_threshold_low = { 22.0f, 1350.0f, 12000.0f },
      .rrx_switch_threshold_high = { 1540.0f, 16200.0f, 250000.0f },
      .digital_poll_period = L9966_DEFAULT_DIGITAL_POLL_PERIOD,
      .eu_used = { true, false },
    },
    {
      .config_data = {
        .curr_src_ctrl = {
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO1
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO2
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO3
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO4
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_1,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_0,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO5
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_1,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_0,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO6
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_1,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_0,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO7
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_1,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_0,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO8
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_3,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_6,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_TRUE,
            }, // IO9
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO10
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO11
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO12
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO13
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO14
            {
                .control_channel_selection = L9966_CFG_CSC_CSS_FORCE0,
                .comparator_threshold = L9966_CFG_CSC_CT_1,
                .current_source_value = L9966_CFG_CSC_CSV_4,
                .csv_dewetting_phase = L9966_CFG_CSC_CSV_DW_CSV,
                .pu_pd_sel = L9966_CFG_CSC_PU_PD_SEL_5,
                .control_channel_invert = L9966_CFG_CSC_CCP_INVERT_FALSE,
            }, // IO15
        },
        .switch_route = {
            .aux_even_channel = L9966_CFG_SR_AUX_EVENODD_CH_VRS,
            .aux_odd_channel = L9966_CFG_SR_AUX_EVENODD_CH_VRS,
            .sent_digital_filter_chx = L9966_CFG_SR_DF_EN_1US_CH1 | L9966_CFG_SR_DF_EN_1US_CH2 | L9966_CFG_SR_DF_EN_1US_CH3 | L9966_CFG_SR_DF_EN_1US_CH4,
            .aox_channel_src = L9966_CFG_SR_AOX_CH_SRC_9,
            .rsent_enabled_chx = L9966_CFG_SR_RSENT_DISABLED,
        },
        .dwt_volt_src_lsf_ctrl = {
            .lambda_enabled_chx = L9966_CFG_DVSLC_LE_DISABLED,
            .dewetting_timer_time = L9966_CFG_DVSLC_DTTC_2MS,
            .var_volt_source_sel = L9966_CFG_DVSLC_VVSS_1000MV,
        },
        .gtm_to_sent_route = {
            {
                .analog_filter = L9966_CFG_GTSR_AF_FAST,
                .digital_bypass = L9966_CFG_GTSR_BP_ON,
                .pullup = L9966_CFG_GTSR_PU_OFF,
                .sent_gtm_route = L9966_CFG_GTSR_SGR_CH1,
            }, //SENT1
            {
                .analog_filter = L9966_CFG_GTSR_AF_FAST,
                .digital_bypass = L9966_CFG_GTSR_BP_ON,
                .pullup = L9966_CFG_GTSR_PU_OFF,
                .sent_gtm_route = L9966_CFG_GTSR_SGR_CH2,
            }, //SENT2
            {
                .analog_filter = L9966_CFG_GTSR_AF_FAST,
                .digital_bypass = L9966_CFG_GTSR_BP_ON,
                .pullup = L9966_CFG_GTSR_PU_OFF,
                .sent_gtm_route = L9966_CFG_GTSR_SGR_CH3,
            }, //SENT3
            {
                .analog_filter = L9966_CFG_GTSR_AF_FAST,
                .digital_bypass = L9966_CFG_GTSR_BP_ON,
                .pullup = L9966_CFG_GTSR_PU_OFF,
                .sent_gtm_route = L9966_CFG_GTSR_SGR_CH4,
            }, //SENT4
        },
        .active_discharge_lsf = {
            .en_chx = L9966_CFG_ADLC_EN_DISABLED,
            .time_cfg = L9966_CFG_ADLC_TC_4MS,
        },
        .wak_mask = L9966_CFG_SC_WM_CH15,
        .sleep_config = L9966_CFG_SC_SL_CH15_LOW,
        .wake_config = {
            .polling_blanking_time = L9966_CFG_WC_PBT_160US,
            .polling_time_cfg = L9966_CFG_WC_PTC_32MS,
        },
        .vrs_config = {
            .double_edge_detection = L9966_CFG_VRS_DED_DISABLED,
            .diag_clr_cmd = L9966_CFG_VRS_DCC_READOUT_CLEAR,
            .min_hyst_force = L9966_CFG_VRS_MHF_NO_FORCING,
            .vrs_mode_sel = L9966_CFG_VRS_SEL_B_LIMITED,
            .conf_mode = L9966_CFG_VRS_CM_AF_EN_AH_EN,
            .hyst_conf = L9966_CFG_VRS_HM_HI3,
            .diag_en = L9966_CFG_VRS_DE_NORMAL,
            .hyst_fb_mode = L9966_CFG_VRS_HFBM_DIRECT_VRS,
            .fe_filt_en = L9966_CFG_VRS_FFE_MASKING_EN,
        },
        .adc_timing = {
            .adc_volt_setting_time = L9966_CFG_AT_AVST_16US,
            .pu1_setting_time = L9966_CFG_AT_PUST_200US,
            .pu2_setting_time = L9966_CFG_AT_PUST_400US,
            .pu3_setting_time = L9966_CFG_AT_PUST_600US,
        },
        .sequencer_config = {
            .interrupt_mask_flag = {
                .int_eu1_pc_conf = L9966_CFG_SQNCR_PC_CMD15,
                .int_eu2_pc_conf = L9966_CFG_SQNCR_PC_ENDLOOP,
                .interrupt_mask = L9966_CFG_SQNCR_IM_CLEAR,
            },
            .control = {
                .sqncr_cfg = {
                    {
                        .sync_pin_enabled = L9966_CFG_SQNCR_CONFIG_SYNC_DISABLED,
                        .init_pc = L9966_CFG_SQNCR_PC_CMD1,
                    }, //EU1
                    {
                        .sync_pin_enabled = L9966_CFG_SQNCR_CONFIG_SYNC_DISABLED,
                        .init_pc = L9966_CFG_SQNCR_PC_ENDLOOP,
                    }, //EU2
                },
                .sync_copy_cmd_en = L9966_CFG_SQNCR_CONFIG_SCC_ENABLED,
            },
            .cmd_config = {
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD2,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD1
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD3,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD2
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD4,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD3
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD5,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD4
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD6,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD5
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD7,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD6
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD8,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD7
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD9,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD8
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD10,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD9
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD11,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD10
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD12,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD11
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD13,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_5V,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 1.0f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD12
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD14,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_INTERNAL,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 0.05f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD13
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD15,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_INTERNAL,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 0.05f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD14
                {
                    .next_pc = L9966_CFG_SQNCR_PC_CMD1,
                    .pu_div_sel = L9966_CFG_SQNCR_CMD_DIV_INTERNAL,
                    .r_volt_sel = L9966_CFG_SQNCR_CMD_RVM_VOLTAGE,
                    .lpf = 0.05f,
                    .v_gain = 1.0f,
                    .v_offset = 0.0f,
                }, //CMD15
            }
        },
        .status = {
            .config_check = L9966_CONFIG_GS_CC_1,
            .calib_sel = L9966_CONFIG_GS_CS_ADC_CALIBRATED,
            .overtemperature_mask = L9966_CONFIG_GS_OTM_ENABLED,
        },
      },
      .rrx = { 220.0f, 2700.0f, 33000.0f },
      .rrx_switch_threshold_low = { 22.0f, 1350.0f, 12000.0f },
      .rrx_switch_threshold_high = { 1540.0f, 16200.0f, 250000.0f },
      .digital_poll_period = L9966_DEFAULT_DIGITAL_POLL_PERIOD,
      .eu_used = { true, false },
    }
};

static ecu_devices_flexio_ctx_t ecu_devices_flexio_ctx[ECU_DEVICE_FLEXIO_MAX] = {
    {
      .slave_index = ECU_SPI_SLAVE_FLEXIO1,
      .init = {
          .nrst_pin = { .port = FLEXIO_NRST_GPIO_Port, .pin = FLEXIO_NRST_Pin },
          .wake_pin = { .port = FLEXIO1_WAKE_GPIO_Port, .pin = FLEXIO1_WAKE_Pin },
          .sync_pin = { .port = FLEXIO1_SYNC_GPIO_Port, .pin = FLEXIO1_SYNC_Pin },
          .int_pin = { .port = FLEXIO1_INT_GPIO_Port, .pin = FLEXIO1_INT_Pin },
          .chip_address = L9966_FRAME_CFG_ADDR_LOW,
      },
      .config_default = ecu_devices_flexio_config_default[ECU_DEVICE_FLEXIO_1],
      .int_exti_pin = FLEXIO1_INT_Pin,
    },
    {
      .slave_index = ECU_SPI_SLAVE_FLEXIO2,
      .init = {
          .nrst_pin = { .port = FLEXIO_NRST_GPIO_Port, .pin = FLEXIO_NRST_Pin },
          .wake_pin = { .port = FLEXIO2_WAKE_GPIO_Port, .pin = FLEXIO2_WAKE_Pin },
          .sync_pin = { .port = FLEXIO2_SYNC_GPIO_Port, .pin = FLEXIO2_SYNC_Pin },
          .int_pin = { .port = FLEXIO2_INT_GPIO_Port, .pin = FLEXIO2_INT_Pin },
          .chip_address = L9966_FRAME_CFG_ADDR_HIGH,
      },
      .config_default = ecu_devices_flexio_config_default[ECU_DEVICE_FLEXIO_2],
      .int_exti_pin = FLEXIO2_INT_Pin,
    },
};

error_t ecu_devices_flexio_init(ecu_device_flexio_t instance, l9966_ctx_t *ctx)
{
  error_t err = E_OK;
  ecu_devices_flexio_ctx_t *flexio_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_FLEXIO_MAX || ctx == NULL, err = E_PARAM);

    flexio_ctx = &ecu_devices_flexio_ctx[instance];
    flexio_ctx->ctx = ctx;

    err = middlelayer_spi_get_slave(&flexio_ctx->init.spi_slave, flexio_ctx->slave_index);
    BREAK_IF(err != E_OK);

    err = l9966_init(flexio_ctx->ctx, &flexio_ctx->init);
    BREAK_IF(err != E_OK);

    memcpy(&flexio_ctx->ctx->config, &flexio_ctx->config_default, sizeof(l9966_config_t));

    err = ecu_config_gpio_exti_register(flexio_ctx->int_exti_pin, (ecu_gpio_exti_cb_t)l9966_int_irq_handler, flexio_ctx->ctx);
    BREAK_IF(err != E_OK);

  } while(0);

  return err;
}

error_t ecu_devices_flexio_get_default_config(ecu_device_flexio_t instance, l9966_config_t *config)
{
  error_t err = E_OK;
  ecu_devices_flexio_ctx_t *flexio_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_FLEXIO_MAX || config == NULL, err = E_PARAM);

    flexio_ctx = &ecu_devices_flexio_ctx[instance];

    memcpy(config, &flexio_ctx->config_default, sizeof(l9966_config_t));

  } while(0);

  return err;
}

error_t ecu_devices_flexio_configure(ecu_device_flexio_t instance, const l9966_config_t *config)
{
  error_t err = E_OK;
  ecu_devices_flexio_ctx_t *flexio_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_FLEXIO_MAX || config == NULL, err = E_PARAM);

    flexio_ctx = &ecu_devices_flexio_ctx[instance];

    err = l9966_configure(flexio_ctx->ctx, config);

  } while(0);

  return err;
}

error_t ecu_devices_flexio_reset(ecu_device_flexio_t instance)
{
  error_t err = E_OK;
  ecu_devices_flexio_ctx_t *flexio_ctx;

  do {
    BREAK_IF_ACTION(instance <= ECU_DEVICE_NONE || instance >= ECU_DEVICE_FLEXIO_MAX, err = E_PARAM);

    flexio_ctx = &ecu_devices_flexio_ctx[instance];

    err = l9966_reset(flexio_ctx->ctx);

  } while(0);

  return err;
}
