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
#define BLEND(l,r,a) (((l) * (1.0f - (a))) + ((r) * (a)))

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#define ALIGNMENT_CACHE           (32u)

#define STATIC_INLINE             __attribute__((always_inline)) static inline
#define INLINE                    __attribute__((always_inline)) inline
#define ALIGNED(x)                __attribute__((aligned(x)))
#define ALIGNED_CACHE             ALIGNED(ALIGNMENT_CACHE)
#define BUFFER_DMA                __attribute__((section(".dmabss")))
#if defined(CORE_CM4)
#define RAM_SECTION
#define ITCM_FUNC
#else
#define RAM_SECTION               __attribute__((section(".ramdata")))
#define ITCM_FUNC                 __attribute__((section(".itcm_func")))
#endif
#define OPTIMIZE(flag)            __attribute__((optimize(flag)))
#define OPTIMIZE_FAST             OPTIMIZE("Ofast")
#define OPTIMIZE_SIZE             OPTIMIZE("Os")
#define OPTIMIZE_DEBUG            OPTIMIZE("Og")
#define OPTIMIZE_NONE             OPTIMIZE("O0")
#define IS_DEBUGGER_ATTACHED()    ((DBGMCU->CR & 0x07) > 0)
#define BREAKPOINT(x)             { if(IS_DEBUGGER_ATTACHED()) __BKPT((x)); }
#define PARITY_ODD_CHECK(value)   __builtin_parity((value))
#define POSITION_VAL(VAL)         (__CLZ(__RBIT(VAL)))

#define BREAK_IF_ACTION(condition, action)              { if((condition)) { {action;} break; } }
#define BREAK_IF(condition)                             { if((condition)) { break; } }
#define RETURN_IF_ACTION(condition, errcode, action)    { if((condition)) { {action;} return (errcode); } }
#define RETURN_IF(condition, errcode)                   { if((condition)) { return (errcode); } }
#define CONTINUE_IF(condition)                          { if((condition)) { continue; } }

#define ATOMIC_TRY_LOCK(var, val) (__atomic_exchange_n(&(var), (val), __ATOMIC_ACQUIRE) == 0u)
#define ATOMIC_RELEASE(var, val) (__atomic_store_n(&(var), (val), __ATOMIC_RELEASE))

#undef HAS_BIT
#undef SET_BIT
#undef RESET_BIT

#define HAS_BIT(field, bit)       (((field) & (bit)) == (bit))
#define SET_BIT(field, bit)       ((field) |= (bit))
#define RESET_BIT(field, bit)     ((field) &= ~(bit))

#if __CORTEX_M == (7)
OPTIMIZE_FAST
STATIC_INLINE void CacheInvalidate(const void * buffer, uint32_t size)
{
  uintptr_t address = (uintptr_t)buffer;
  uintptr_t aligned_address = address & ~(uintptr_t)(ALIGNMENT_CACHE - 1u);

  uint32_t aligned_size = size + (uint32_t)(address - aligned_address);

  SCB_InvalidateDCache_by_Addr((uint32_t *)aligned_address, aligned_size);
}

OPTIMIZE_FAST
STATIC_INLINE void CacheClean(const void * buffer, uint32_t size)
{
  uintptr_t address = (uintptr_t)buffer;
  uintptr_t aligned_address = address & ~(uintptr_t)(ALIGNMENT_CACHE - 1u);

  uint32_t aligned_size = size + (uint32_t)(address - aligned_address);

  SCB_CleanDCache_by_Addr((uint32_t *)aligned_address, aligned_size);
}
#endif

OPTIMIZE_FAST
STATIC_INLINE uint32_t EnterCritical(void)
{
  uint32_t prim = __get_PRIMASK();

  __disable_irq();

  return prim;
}

OPTIMIZE_FAST
STATIC_INLINE void ExitCritical(uint32_t prim)
{
  if(!prim) {
    __enable_irq();
  }
}


#endif /* COMMON_INC_COMPILER_H_ */
