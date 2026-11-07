#ifndef TEST_MAIN_H
#define TEST_MAIN_H
#include <stdint.h>
#define DBGMCU ((struct { uint32_t CR; } *)0)
#define __BKPT(x) __builtin_trap()
static inline uint32_t __get_PRIMASK(void) { return 0; }
static inline void __disable_irq(void) { }
static inline void __enable_irq(void) { }
#endif
