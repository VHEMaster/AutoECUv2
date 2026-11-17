/*
 * lwippools.h
 *
 *  Created on: 17 нояб. 2026 г.
 *      Author: VHEMaster
 *
 * Fixed-size malloc pools for CM4 PPPoS + Raw TCP (lwIP 2.2.1).
 *
 * Current configuration:
 *   NO_SYS = 1, PPP_SUPPORT = 1, PPP_SERVER = 1
 *   TCP_MSS = 536, TCP_SND_BUF = 1072
 *   MEMP_NUM_TCP_PCB = 16, MEMP_NUM_TCP_SEG = 16
 *   PBUF_POOL_BUFSIZE = 592, PBUF_POOL_SIZE = 16
 *
 * PBUF_RAM allocations include the pbuf header and the payload.
 * A 768-byte malloc pool block fits a full 536-byte TCP segment and
 * a PPPoS TX buffer, including pbuf headers and allocator metadata.
 * PBUF_POOL and protocol PCB objects remain in their own static memp pools.
 *
 * This file must NOT use an include guard: lwIP includes it more than once
 * to generate memory pool declarations, definitions and metadata.
 */

LWIP_MALLOC_MEMPOOL_START

LWIP_MALLOC_MEMPOOL(8, 128)
LWIP_MALLOC_MEMPOOL(8, 256)
LWIP_MALLOC_MEMPOOL(8, 768)
LWIP_MALLOC_MEMPOOL(1, 2048)

LWIP_MALLOC_MEMPOOL_END
