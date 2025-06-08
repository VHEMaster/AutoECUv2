/*
 * compiler.h
 *
 *  Created on: Mar 20, 2024
 *      Author: VHEMaster
 */

#ifndef COMMON_INC_COMPILER_H_
#define COMMON_INC_COMPILER_H_

#include "main.h"

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
#define ITEMSOF(ARRAY) (sizeof((ARRAY)) / sizeof((ARRAY)[0]))
#define CLAMP(val,min,max) ((val) < (min) ? (min) : (val) > (max) ? (max) : (val))
#define BLEND(l,r,a) (((l) * (a)) + ((r) * (1.0f - (a))))

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#define ALIGNMENT_CACHE           (__SCB_DCACHE_LINE_SIZE)

#define STATIC_INLINE             __attribute__((always_inline)) static inline
#define INLINE                    __attribute__((always_inline)) inline
#define ALIGNED(x)                __attribute__((aligned(x)))
#define ALIGNED_CACHE             ALIGNED(ALIGNMENT_CACHE)
#define BUFFER_DMA                __attribute__((section(".dmabss")))
#define ITCM_FUNC                 __attribute__((section(".itcm_func")))
#define OPTIMIZE(flag)            __attribute__((optimize(flag)))
#define OPTIMIZE_FAST             OPTIMIZE("Ofast")
#define OPTIMIZE_SIZE             OPTIMIZE("Os")
#define OPTIMIZE_DEBUG            OPTIMIZE("Og")
#define OPTIMIZE_NONE             OPTIMIZE("O0")
#define IS_DEBUGGER_ATTACHED()    ((DBGMCU->CR & 0x07) > 0)
#define BREAKPOINT(x)             __BKPT((x))
#define PARITY_ODD_CHECK(value)   __builtin_parity((value))

#define BREAK_IF_ACTION(condition, action)              { if((condition)) { {action;} break; } }
#define BREAK_IF(condition)                             { if((condition)) { break; } }
#define RETURN_IF_ACTION(condition, errcode, action)    { if((condition)) { {action;} return (errcode); } }
#define RETURN_IF(condition, errcode)                   { if((condition)) { return (errcode); } }


#if __CORTEX_M == (7)
STATIC_INLINE void CacheInvalidate(const void * buffer, uint32_t size)
{
  uint32_t aligned = (uint32_t)buffer & (ALIGNMENT_CACHE - 1);
  if(aligned == 0)
    SCB_InvalidateDCache_by_Addr((uint32_t*)buffer, size);
  else SCB_InvalidateDCache_by_Addr((uint32_t*)((uint32_t)buffer - aligned), size + aligned);
}

STATIC_INLINE void CacheClean(const void * buffer, uint32_t size)
{
  uint32_t aligned = (uint32_t)buffer & (ALIGNMENT_CACHE - 1);
  if(aligned == 0)
    SCB_CleanDCache_by_Addr((uint32_t*)buffer, size);
  else SCB_CleanDCache_by_Addr((uint32_t*)((uint32_t)buffer - aligned), size + aligned);
}
#endif

uint32_t EnterCritical(void);
void ExitCritical(uint32_t prim);

#endif /* COMMON_INC_COMPILER_H_ */
