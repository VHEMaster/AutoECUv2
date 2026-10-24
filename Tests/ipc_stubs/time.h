#ifndef TEST_TIME_H
#define TEST_TIME_H
#include_next <time.h>
#include <stdint.h>
extern uint32_t test_now;
static inline uint32_t time_now_us(void) { return test_now; }
static inline uint32_t time_diff(uint32_t a, uint32_t b) { return (a-b) & 0x03ffffffu; }

#endif
