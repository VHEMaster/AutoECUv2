#ifndef TEST_COMMON_H
#define TEST_COMMON_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "errors.h"
#define ALIGNED_CACHE __attribute__((aligned(32)))
#define RAM_SECTION
#define INLINE __attribute__((always_inline)) inline
#define STATIC_INLINE __attribute__((always_inline)) static inline
#define ITEMSOF(a) (sizeof(a)/sizeof((a)[0]))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define BREAK_IF_ACTION(c,a) { if(c) { a; break; } }
#define BREAK_IF(c) { if(c) { break; } }
#define CONTINUE_IF(c) { if(c) { continue; } }
#define RETURN_IF(c,e) { if(c) return (e); }
typedef struct { float value; bool valid; } ecu_core_runtime_value_ctx_t;
static inline uint32_t __REV(uint32_t v) { return __builtin_bswap32(v); }
static inline uint32_t __RBIT(uint32_t v)
{
  uint32_t r = 0;
  for(unsigned i = 0; i < 32; ++i) { r = (r << 1) | (v & 1u); v >>= 1; }
  return r;
}
extern void test_enter_critical(void);
static inline uint32_t EnterCritical(void) { test_enter_critical(); return 0; }
static inline void ExitCritical(uint32_t prim) { (void)prim; }
#endif
