#ifndef TEST_TIME_H
#define TEST_TIME_H
#include_next <time.h>
#include <stdint.h>
#define TIME_US_IN_MS 1000u
typedef uint32_t time_us_t;
typedef uint32_t time_delta_us_t;
extern uint32_t test_now;
uint32_t test_time_now_us(void);
static inline uint32_t time_now_us(void) { return test_time_now_us(); }
static inline uint32_t time_diff(uint32_t a, uint32_t b) { return (a-b) & 0x03ffffffu; }

#endif
