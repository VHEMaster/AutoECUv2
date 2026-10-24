#ifndef TEST_COMMON_H
#define TEST_COMMON_H
#include <stdbool.h>
#include <stdint.h>
#include "errors.h"
typedef struct { float value; bool valid; } ecu_core_runtime_value_ctx_t;
extern void test_enter_critical(void);
static inline uint32_t EnterCritical(void) { test_enter_critical(); return 0; }
static inline void ExitCritical(uint32_t prim) { (void)prim; }
#endif
