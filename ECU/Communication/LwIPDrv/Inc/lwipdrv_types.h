/*
 * lwipdrv_types.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 */

#ifndef COMMUNICATION_LWIPDRV_INC_LWIPDRV_TYPES_H_
#define COMMUNICATION_LWIPDRV_INC_LWIPDRV_TYPES_H_

#include "common.h"

#include "rs422.h"
#include "usb.h"
#include "lwipdrv_arbiter.h"
#include "doip_lwip.h"
#include "tunerstudio_lwip.h"
#include "versioned_lwipdrv.h"
#include "config_comm_types.h"

#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/ip4_addr.h"
#include "lwip/err.h"
#include "netif/ppp/ppp.h"
#include "netif/ppp/pppos.h"

typedef struct lwipdrv_ctx_tag lwipdrv_ctx_t;

typedef struct {
    ecu_comm_rs422_t rs422_instance;
    rs422_ctx_t *rs422_ctx;
    usb_ctx_t *usb_ctx;
    doip_ctx_t *doip_ctx;
    tunerstudio_ctx_t *tunerstudio_ctx;
}lwipdrv_init_ctx_t;

typedef struct {
    struct netif ppp_netif;
    ppp_pcb *ppp;

    ip4_addr_t addr_our;
    ip4_addr_t addr_his;

    bool link_up;
    bool restart_pending;
    bool close_pending;
    bool no_carrier;
    bool rs422_reserved;
    bool rs422_switch_pending;
    uint32_t usb_generation;
    lwipdrv_arbiter_t arbiter;

}lwipdrv_lwip_ctx_t;

typedef struct lwipdrv_ctx_tag {
    lwipdrv_config_t config;
    lwipdrv_init_ctx_t init;
    bool initialized;
    bool configured;
    ATOMIC_LOCK_TYPE lock;

    bool reset_trigger;

    lwipdrv_lwip_ctx_t lwip;
    doip_lwip_ctx_t doip;
    tunerstudio_lwip_ctx_t tunerstudio;

}lwipdrv_ctx_t;

#endif /* COMMUNICATION_LWIPDRV_INC_LWIPDRV_TYPES_H_ */
