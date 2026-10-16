#!/usr/bin/env python3
"""Compare fixed 26-bit clock helpers/interpolation with the former configured mask."""
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
header = (root/'CM7/ECU/Drivers/Time/Inc/time.h').read_text()
current_c = (root/'CM7/ECU/Drivers/Time/Src/time.c').read_text()
original = subprocess.check_output(['git', '-C', str(root), 'show',
    '0ef0826:CM7/ECU/Drivers/Time/Src/time.c'], text=True)
names = ['time_now_us', 'time_mask_us', 'time_now_tick', 'time_tick_mask',
         'time_diff', 'time_add', 'time_sub', 'time_tick_diff',
         'time_interpolate_time', 'time_interpolate_value']
current, reference = [], []
for name in names:
    pattern = r'(?:STATIC_INLINE|ITCM_FUNC OPTIMIZE_FAST INLINE) [\w]+ '+name+r'\([^)]*\)\n\{.*?\n\}'
    a = re.search(pattern, header+'\n'+current_c, re.S)
    b = re.search(pattern, original, re.S)
    assert a and b, name
    current.append(a[0].replace('ITCM_FUNC OPTIMIZE_FAST INLINE', 'STATIC_INLINE'))
    text = b[0].replace('ITCM_FUNC OPTIMIZE_FAST INLINE', 'STATIC_INLINE')
    for symbol in names + ['g_time_timebase', 'g_time_mask', 'g_time_tickbase', 'g_tick_mask']:
        text = re.sub(r'\b'+symbol+r'\b', 'reference_'+symbol, text)
    reference.append(text)
macros = '\n'.join(re.findall(r'^#define TIME_(?:TICKBASE|TIMEBASE)_MASK\s+\S+', header, re.M))
source = '''#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#define STATIC_INLINE static inline __attribute__((always_inline))
typedef uint32_t time_us_t, time_tick_t, time_delta_us_t, time_delta_tick_t;
static volatile uint32_t clock_us, clock_tick;
static volatile uint32_t *g_time_timebase = &clock_us, *g_time_tickbase = &clock_tick;
static volatile uint32_t *reference_g_time_timebase = &clock_us, *reference_g_time_tickbase = &clock_tick;
'''+macros+'''
static uint32_t reference_g_time_mask=TIME_TIMEBASE_MASK, reference_g_tick_mask=TIME_TICKBASE_MASK;
'''+ '\n'.join(current+reference)+'''
static uint32_t random_value=123;
static uint32_t next_random(void) {return random_value=random_value*1664525u+1013904223u;}
int main(void) {
 for(unsigned i=0;i<250000;i++) {
  uint32_t a=next_random()&TIME_TIMEBASE_MASK, b=next_random()&TIME_TIMEBASE_MASK;
  clock_us=a; clock_tick=b;
  assert(time_now_us()==reference_time_now_us());
  assert(time_now_tick()==reference_time_now_tick());
  assert(time_mask_us()==reference_time_mask_us());
  assert(time_tick_mask()==reference_time_tick_mask());
  assert(time_diff(a,b)==reference_time_diff(a,b));
  assert(time_tick_diff(a,b)==reference_time_tick_diff(a,b));
  assert(time_add(a,b)==reference_time_add(a,b));
  assert(time_sub(a,b)==reference_time_sub(a,b));
 }
 for(unsigned i=0;i<100000;i++) {
  uint32_t left=(TIME_TIMEBASE_MASK-50000u+(next_random()%100000u))&TIME_TIMEBASE_MASK;
  uint32_t delta=1+next_random()%100000u;
  uint32_t right=i%7 ? time_add(left,delta) : left;
  int32_t offset=(int32_t)(next_random()%300000u)-100000;
  uint32_t mid=(left+offset)&TIME_TIMEBASE_MASK;
  float vl=((int)(next_random()%100000)-50000)*0.01f, vr=vl+17.3f;
  float current=time_interpolate_value(left,mid,right,vl,vr);
  float reference=reference_time_interpolate_value(left,mid,right,vl,vr);
  assert(memcmp(&current,&reference,sizeof(float))==0);
  float il=-130.0f, ir=i%7 ? il+31.5f : il;
  float im=il+((int)(next_random()%90000)-30000)*0.001f;
  assert(time_interpolate_time(il,im,ir,left,right)==reference_time_interpolate_time(il,im,ir,left,right));
 }
 puts("Time: 250000 helper cases + 100000 value/time interpolation pairs across the real 26-bit wrap; identical results.");
}
'''
with tempfile.TemporaryDirectory(prefix='fixed-time-') as directory:
    path = Path(directory)/'test.c'; path.write_text(source)
    for optimization in ['-Og', '-Ofast']:
        binary = Path(directory)/'test'
        subprocess.run(['gcc', optimization, str(path), '-lm', '-o', str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
