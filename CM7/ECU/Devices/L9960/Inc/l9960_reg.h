/*
 * l9960_reg.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_L9960_INC_L9960_REG_H_
#define DEVICES_L9960_INC_L9960_REG_H_

typedef enum {
  L9960_REG_ADDR_NOT_USED = 0,
  L9960_REG_ADDR_OC_MONITOR,
  L9960_REG_ADDR_RESTART_TRIGGER,
  L9960_REG_ADDR_CONFIG_1,
  L9960_REG_ADDR_CONFIG_2,
  L9960_REG_ADDR_CPMFOG_3,
  L9960_REG_ADDR_CONFIG_4,
  L9960_REG_ADDR_CONFIG_REQ,
  L9960_REG_ADDR_STATUS_REQ,
  L9960_REG_ADDR_OFF_STATE_DIAG,
  L9960_REG_ADDR_RSVD1,
  L9960_REG_ADDR_RSVD2,
  L9960_REG_ADDR_RSVD3,
  L9960_REG_ADDR_COMP_TRACE_REQ,
  L9960_REG_ADDR_RSVD4,
  L9960_REG_ADDR_ID_REQ,
  L9960_REG_ADDR_MAX,
}l9960_reg_addr_t;

typedef enum {
  L9960_REG_DATA_NONE = 0,
  L9960_REG_DATA_0 = 1,
  L9960_REG_DATA_1 = 2,
  L9960_REG_DATA_2 = 4,
  L9960_REG_DATA_3 = 8,
  L9960_REG_DATA_4 = 16,
  L9960_REG_DATA_5 = 32,
  L9960_REG_DATA_6 = 64,
  L9960_REG_DATA_7 = 128,
  L9960_REG_DATA_8 = 256,
  L9960_REG_DATA_9 = 512,
  L9960_REG_DATA_10 = 1024,
}l9960_reg_data_t;

typedef enum {
  L9960_REG_DIAGOFF_NOTUSED = 0,
  L9960_REG_DIAGOFF_OPENLOAD,
  L9960_REG_DIAGOFF_SHORTBATT,
  L9960_REG_DIAGOFF_SHORTGND,
  L9960_REG_DIAGOFF_NOFAILURE,
  L9960_REG_DIAGOFF_NOTRIGGER_INCORRECT,
  L9960_REG_DIAGOFF_NOTRIGGER_THZNOTEXPIRED,
  L9960_REG_DIAGOFF_NOTDONE,
}l9960_reg_diagoff_t;

typedef enum {
  L9960_REG_HWSC_NOTDONE0 = 0,
  L9960_REG_HWSC_NOTDONE1,
  L9960_REG_HWSC_NOTDONE2,
  L9960_REG_HWSC_NOTDONE3,
  L9960_REG_HWSC_FAIL,
  L9960_REG_HWSC_RUN_LBIST_PASS,
  L9960_REG_HWSC_FAIL_LBIST_PASS,
  L9960_REG_HWSC_PASS,
}l9960_reg_hwsc_t;

typedef union {
    uint16_t data;
    struct {
      uint16_t parity : 1;
      uint16_t data : 11;
      uint16_t addr : 4;
    }bits;
}l9960_request_payload_t;

typedef union {
    uint16_t data;
    struct {
      uint16_t data : 12;
      uint16_t addr : 4;
    }bits;
}l9960_response_payload_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t zero : 7;
        uint16_t config_cc : 1;
        uint16_t hwsc_libst_trig : 1;
        uint16_t sw_reset : 2;
    }bits;
}l9960_cmd_restart_trigger_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t diag_clr_en : 1;
        uint16_t always_one : 1;
        uint16_t tsw_low_current : 1;
        uint16_t tdiag1 : 3;
        uint16_t vsr : 1;
        uint16_t isr : 1;
        uint16_t nosr : 1;
        uint16_t cl : 2;
    }bits;
}l9960_cmd_config1_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t uv_win : 1;
        uint16_t always_zero : 1;
        uint16_t nspread : 1;
        uint16_t uv_prot_en : 1;
        uint16_t otwarn_thr_var : 3;
        uint16_t otsd_thr_var : 3;
        uint16_t in1_in2_if : 1;
    }bits;
}l9960_cmd_config2_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t otwarn_tsec_en : 1;
        uint16_t always_zero : 5;
        uint16_t tvvl : 4;
        uint16_t vvl_mode : 1;
    }bits;
}l9960_cmd_config3_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t reserved : 9;
        uint16_t ol_on : 1;
        uint16_t tdsr : 1;
    }bits;
}l9960_cmd_config4_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t trig : 1;
        uint16_t always_zero : 10;
    }bits;
}l9960_cmd_off_state_diag_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t ocl0 : 2;
        uint16_t always_zero1 : 1;
        uint16_t ocl1 : 2;
        uint16_t always_zero2 : 1;
        uint16_t och0 : 2;
        uint16_t always_zero3 : 1;
        uint16_t och1 : 2;
        uint16_t always_zero4 : 1;
    }bits;
}l9960_resp_oc_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t always_zero : 1;
        uint16_t diag_clr_en_echo : 1;
        uint16_t always_one : 1;
        uint16_t tsw_low_current_echo : 1;
        uint16_t tdiag1_echo : 3;
        uint16_t vsr_echo : 1;
        uint16_t isr_echo : 1;
        uint16_t nosr_echo : 1;
        uint16_t cl_echo : 2;
    }bits;
}l9960_resp_config1_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t uv_win_echo : 1;
        uint16_t always_zero : 1;
        uint16_t nspread_echo : 1;
        uint16_t uv_prot_en_echo : 1;
        uint16_t otwarn_thr_var_echo : 3;
        uint16_t otsd_thr_var_echo : 3;
        uint16_t in1_in2_if_latch : 1;
        uint16_t in1_in2_if_echo : 1;
    }bits;
}l9960_resp_config2_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t otwarn_tsec_en_echo : 1;
        uint16_t always_zero : 6;
        uint16_t tvvl_echo : 4;
        uint16_t vvl_mode_echo : 1;
    }bits;
}l9960_resp_config3_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t always_zero1 : 1;
        uint16_t reserved : 9;
        uint16_t always_zero2 : 1;
        uint16_t tdsr_echo : 1;
    }bits;
}l9960_resp_config4_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t always_zero : 9;
        uint16_t cc_latch_state : 1;
        uint16_t config_cc_echo : 1;
        uint16_t por_status : 1;
    }bits;
}l9960_resp_config5_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t vps_uv : 1;
        uint16_t vdd_uv_reg : 1;
        uint16_t vdd_ov_reg : 1;
        uint16_t ilim_reg : 1;
        uint16_t ngfail : 1;
        uint16_t vps_uv_reg : 1;
        uint16_t hwsc_lbist_status : 3;
        uint16_t bridge_en : 1;
        uint16_t dis_status : 1;
        uint16_t ndis_status : 1;
    }bits;
}l9960_resp_states1_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t ol_on_status : 2;
        uint16_t notsd_reg : 1;
        uint16_t notsd : 1;
        uint16_t otwarn_reg : 1;
        uint16_t otwarn : 1;
        uint16_t otsd_cnt : 6;
    }bits;
}l9960_resp_states2_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t always_zero1 : 1;
        uint16_t error_cnt : 4;
        uint16_t uv_cnt_reached : 1;
        uint16_t always_zero2 : 6;
    }bits;
}l9960_resp_states3_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t diag_off : 3;
        uint16_t always_zero : 9;
    }bits;
}l9960_resp_osd_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t reserved : 9;
        uint16_t vdd_ov_l : 3;
    }bits;
}l9960_resp_rsvd3_vol_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t vdd_uv : 1;
        uint16_t vdd_ov : 1;
        uint16_t ignored : 2;
        uint16_t reserved : 8;
    }bits;
}l9960_resp_rsvd3_vou_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t assp : 1;
        uint16_t always_zero : 1;
        uint16_t asic_name : 10;
    }bits;
}l9960_resp_eid_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t always_zero : 6;
        uint16_t silicon_version : 4;
        uint16_t supplier : 2;
    }bits;
}l9960_resp_sv_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t code_version : 8;
        uint16_t always_zero : 4;
    }bits;
}l9960_resp_lhwv_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t wafer_x : 6;
        uint16_t wafer_y : 6;
    }bits;
}l9960_resp_trace1_t;

typedef union {
    uint16_t data;
    struct {
        uint16_t wafer_number : 5;
        uint16_t reserved : 7;
    }bits;
}l9960_resp_trace2_t;

#endif /* DEVICES_L9960_INC_L9960_REG_H_ */
