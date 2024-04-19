/*
 * cj125_reg.h
 *
 *  Created on: Apr 19, 2024
 *      Author: VHEMaster
 */

#ifndef DEVICES_CJ125_INC_CJ125_REG_H_
#define DEVICES_CJ125_INC_CJ125_REG_H_

#define CJ125_REG_VERSION       (0x0C)
#define CJ125_REG_CMD_VALID     (0x00)
#define CJ125_REG_CMD_NOT_VALID (0xFF)
#define CJ125_REG_RESP_CHECK    (0x05)

#define CJ125_REG_RD_IDENT      (0x48)
#define CJ125_REG_RD_DIAG       (0x78)

#define CJ125_REG_RD_INIT1      (0x6C)
#define CJ125_REG_WR_INIT1      (0x56)

#define CJ125_REG_RD_INIT2      (0x7E)
#define CJ125_REG_WR_INIT2      (0x5A)

typedef union {
    uint8_t data;
    struct {
        uint8_t instr_f : 1;
        uint8_t parity : 1;
        uint8_t last_rd_wr : 1;
        uint8_t check_value : 3;
    }bits;
}cj125_reg_resp_t;

typedef union {
    uint8_t data;
    struct {
        uint8_t diag_vm : 2;
        uint8_t diag_un : 2;
        uint8_t diag_iaip : 2;
        uint8_t diag_heat : 2;
    }bits;
}cj125_reg_diag_t;

typedef union {
    uint8_t data;
    struct {
        uint8_t version : 3;
        uint8_t ic_number : 5;
    }bits;
}cj125_reg_ident_t;

typedef union {
    uint8_t data;
    struct {
        uint8_t vl : 1;
        uint8_t always_zero1 : 1;
        uint8_t la : 1;
        uint8_t en_f3k : 1;
        uint8_t ra : 1;
        uint8_t always_zero2 : 1;
        uint8_t pa : 1;
        uint8_t en_hold : 1;
    }bits;
}cj125_reg_init1_t;

typedef union {
    uint8_t data;
    struct {
        uint8_t pr : 4;
        uint8_t enscun : 1;
        uint8_t set_dia_q : 1;
        uint8_t sreset : 1;
        uint8_t reserved : 1;
    }bits;
}cj125_reg_init2_t;

typedef struct {
    cj125_reg_ident_t ident;
    cj125_reg_diag_t diag;
    cj125_reg_init1_t init1;
    cj125_reg_init2_t init2;
}cj125_regs_t;

#endif /* DEVICES_CJ125_INC_CJ125_REG_H_ */
