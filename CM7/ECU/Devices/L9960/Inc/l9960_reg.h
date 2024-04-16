/*
 * l9960_reg.h
 *
 *  Created on: Apr 16, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_L9960_INC_L9960_REG_H_
#define DEVICES_L9960_INC_L9960_REG_H_

#define L9966_ELECTRONIC_ID     (0x03B)

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

#endif /* DEVICES_L9960_INC_L9960_REG_H_ */
