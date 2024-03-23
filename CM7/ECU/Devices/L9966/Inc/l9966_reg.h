/*
 * l9966_reg.h
 *
 *  Created on: Mar 17, 2024
 *      Author: VHEMaster
 */

#ifndef DRIVERS_L9966_INC_L9966_REG_H_
#define DRIVERS_L9966_INC_L9966_REG_H_

#include <stdint.h>

#define L9966_FRAME_CFG_ADDR_LOW            0x8000
#define L9966_FRAME_CFG_ADDR_HIGH           0xC000
#define L9966_FRAME_CFG_ADDR_MASK           0xC000

#define L9966_FRAME_READ                    0x2000
#define L9966_FRAME_WRITE                   0x0000
#define L9966_FRAME_CLK_MON_ON              0x1000
#define L9966_FRAME_CLK_MON_OFF             0x0000
#define L9966_FRAME_PAR_MOSI_INST           0x0001

#define L9966_FRAME_REG_SHIFT               4

#define L9966_REG_GEN_STATUS                0x01
#define L9966_REG_DEV_V                     0x02
#define L9966_REG_HW_REV                    0x03
#define L9966_REG_DEV_ID                    0x04
#define L9966_REG_CURR_SRC_CTRL_x           0x21
#define L9966_REG_SWITCH_ROUTE              0x30
#define L9966_REG_DWT_VOLT_SRC_LSF_CTRL     0x31
#define L9966_REG_DIN_IN_STAT_LTC           0x33
#define L9966_REG_GTM_TO_SENT_ROUTE_x       0x34
#define L9966_REG_ACTIVE_DISCHARGE_LSF_CTRL 0x36
#define L9966_REG_WAK_MSK                   0x40
#define L9966_REG_SLEEP_CONFIG              0x41
#define L9966_REG_WAK_CONFIG                0x42
#define L9966_REG_SOFT_RST_CMD              0x43
#define L9966_REG_VRS                       0x51
#define L9966_REG_SQNCR_INT_MSK_FLG         0x80
#define L9966_REG_SC_CONF                   0x81
#define L9966_REG_ADC_TIMING                0x82
#define L9966_REG_SC_RESULT                 0x83
#define L9966_REG_SQNCR_CMD_x               0xC1
#define L9966_REG_SQNCR_CTRL                0xC0
#define L9966_REG_SQNCR_RSLT_COPY_CMD       0xCF
#define L9966_REG_DIN_IN_STAT               0xE0
#define L9966_REG_SQNCR_RESULT_x            0xE1

#define L9966_FRAME_PAR_MOSI_DATA           0x8000

#define L9966_FRAME_MISO_VERIFY_MASK        0xFE00
#define L9966_FRAME_MISO_VERIFY_VALUE       0xEA00
#define L9966_FRAME_MISO_TRANS_F            0x0100
#define L9966_FRAME_MISO_ECHO_REG_MASK      0x00FF
#define L9966_FRAME_MISO_WRONG_INSTRUCTION  0xFFFF

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t OT_FLT : 1;
            uint16_t V33_FLT : 1;
            uint16_t WAK_UP : 1;
            uint16_t OT_MASK : 1;
            uint16_t TRIM_FLT : 1;
            uint16_t CALIB_FLT : 1;
            uint16_t CALIB_SEL : 1;
            uint16_t : 6;
            uint16_t CONFIG_CHECK : 2;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_gen_status_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t VER_ID : 8;
            uint16_t : 7;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_dev_v_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t HW_REV : 8;
            uint16_t : 7;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_hw_rev_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t DEV_ID : 8;
            uint16_t : 7;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_dev_id_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t MODE : 1;
            uint16_t SEL : 3;
            uint16_t CV_DW : 2;
            uint16_t CV : 3;
            uint16_t CTR : 2;
            uint16_t CIS : 4;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_curr_src_ctrl_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t RSENTx : 4;
            uint16_t RAOX : 4;
            uint16_t DIG_FILx : 4;
            uint16_t AUX_ODDCH : 1;
            uint16_t AUX_EVENCH : 1;
            uint16_t : 1;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_switch_route_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t VVAR_VF : 5;
            uint16_t DWT : 3;
            uint16_t : 1;
            uint16_t LSF_MDx : 4;
            uint16_t : 2;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_dwt_volt_src_lsf_ctrl_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t DIG_IN_LTCx : 15;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_dig_in_stat_ltc_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t SENTo_GTM_ROUTE : 4;
            uint16_t SENTe_GTM_ROUTE : 4;
            uint16_t R20kPU : 2;
            uint16_t DIG_BP : 2;
            uint16_t AN_FIL : 2;
            uint16_t : 1;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_gtm_to_sent_route_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t TIME_CFG : 4;
            uint16_t : 5;
            uint16_t EN_CHx : 4;
            uint16_t : 2;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_active_discharge_lsf_ctrl_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t MSK_INx : 15;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_wak_msk_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t TGT_INx : 15;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sleep_config_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t AC : 4;
            uint16_t PT : 2;
            uint16_t PB : 2;
            uint16_t : 7;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_wak_config_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t AC : 4;
            uint16_t : 11;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_soft_rst_cmd_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t VRS_FE_FILT_EN : 1;
            uint16_t VRS_HYST_FB_MODE : 1;
            uint16_t VRS_EN_DIAG : 1;
            uint16_t VRS_HYST_CONF : 3;
            uint16_t VRS_CONF_MODE : 2;
            uint16_t VRS_SEL : 1;
            uint16_t MIN_HYST_FORCE : 1;
            uint16_t DIAG_CLR_CMD : 1;
            uint16_t LIM_ADAP_DOUBLE_EDGE : 1;
            uint16_t : 1;
            uint16_t VRS_FAULT : 2;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_vrs_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t INTx : 3;
            uint16_t CFGx : 3;
            uint16_t INT_EU1_CONF : 4;
            uint16_t INT_EU2_CONF : 4;
            uint16_t : 1;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sqncr_int_msk_flg_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t R_VOLT_MEAS_SELECT : 1;
            uint16_t PUP_DIV : 2;
            uint16_t ADC_MUX : 5;
            uint16_t ADC_RUN : 1;
            uint16_t : 6;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sc_conf_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t CT_PU1 : 4;
            uint16_t CT_PU2 : 4;
            uint16_t CT_PU3 : 4;
            uint16_t CT_AD : 3;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_adc_timing_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t ADC_RESULT : 15;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sc_result_resistor_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t ADC_RESULT : 12;
            uint16_t : 2;
            uint16_t NEW_RESULT_FLAG : 1;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sc_result_voltage_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t R_VOLT_MEAS_SELECT : 1;
            uint16_t PUP_DIV : 2;
            uint16_t NXT_PC : 4;
            uint16_t : 8;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sqncr_cmd_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t EU1_EN : 1;
            uint16_t INIT_PC_EU1 : 4;
            uint16_t : 1;
            uint16_t EU1_SYNC_EN : 1;
            uint16_t SYNC_COPY_CMD_EN : 1;
            uint16_t EU2_EN : 1;
            uint16_t INIT_PC_EU2 : 4;
            uint16_t : 1;
            uint16_t EU2_SYNC_EN : 1;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sqncr_ctrl_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t SQNCR_RESULTx : 15;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sqncr_rslt_copy_cmd_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t DIN_INx : 15;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_dig_in_stat_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t ADC_RESULT : 15;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sqncr_result_resistor_t;

typedef struct {
    union {
        uint16_t data;
        struct {
            uint16_t ADC_RESULT : 12;
            uint16_t : 2;
            uint16_t NEW_RESULT_FLAG : 1;
            uint16_t Parity : 1;
        }bits;
    }u;
}l9966_reg_sqncr_result_voltage_t;

#endif /* DRIVERS_L9966_INC_L9966_REG_H_ */
