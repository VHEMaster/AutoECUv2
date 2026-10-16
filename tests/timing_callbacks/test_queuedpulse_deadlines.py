#!/usr/bin/env python3
"""Compare real inline deadline helper with the original body at wrap/late boundaries."""
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
name = 'queuedpulses_internal_calculate_pulse_cplt_time'
header = (root/'CM7/ECU/Drivers/QueuedPulses/Inc/queuedpulses_internal.h').read_text()
original = subprocess.check_output(['git', '-C', str(root), 'show',
    '0ef0826:CM7/ECU/Drivers/QueuedPulses/Src/queuedpulses_internal.c'], text=True)
a = re.search(r'STATIC_INLINE time_delta_us_t '+name+r'\([^)]*\)\n\{.*?\n\}', header, re.S)[0]
b = re.search(r'ITCM_FUNC time_delta_us_t '+name+r'\([^)]*\)\n\{.*?\n\}', original, re.S)[0]
b = b.replace('ITCM_FUNC', 'STATIC_INLINE').replace(name, 'reference_'+name)
time = (root/'CM7/ECU/Drivers/Time/Inc/time.h').read_text()
helpers = '\n'.join(re.search(r'STATIC_INLINE \w+ '+n+r'\([^)]*\)\n\{.*?\n\}', time, re.S)[0]
                    for n in ['time_add', 'time_diff'])
mask = re.search(r'#define TIME_TIMEBASE_MASK\s+(\S+)', (root/'CM7/ECU/Drivers/Time/Inc/time.h').read_text())[1]
source = '''#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#define STATIC_INLINE static inline __attribute__((always_inline))
typedef uint32_t time_us_t, time_delta_us_t;
typedef struct {time_us_t time; time_delta_us_t pulse;} queuedpulse_entry_t;
static time_us_t g_time_mask;
#define ECU_TIMEBASE_MASK '''+mask+'\n#define TIME_TIMEBASE_MASK ECU_TIMEBASE_MASK\n'+helpers+'\n'+a+'\n'+b+'''
int main(void) {
 uint32_t rng=123;
 const uint32_t masks[]={ECU_TIMEBASE_MASK, 0xffffffffu, 0xffffu};
 const uint32_t offsets[]={0,1,(ECU_TIMEBASE_MASK>>1)-1,ECU_TIMEBASE_MASK>>1,
                          (ECU_TIMEBASE_MASK>>1)+1,ECU_TIMEBASE_MASK,0xffffffffu};
 for(unsigned m=0;m<3;m++) {
  g_time_mask=masks[m];
  for(unsigned i=0;i<100000;i++) {
   rng=rng*1664525u+1013904223u; queuedpulse_entry_t entry={rng,0};
   rng=rng*1664525u+1013904223u; entry.pulse=rng;
   rng=rng*1664525u+1013904223u; uint32_t now=rng;
   assert(queuedpulses_internal_calculate_pulse_cplt_time(&entry,now)==
          reference_queuedpulses_internal_calculate_pulse_cplt_time(&entry,now));
   for(unsigned j=0;j<7;j++) {
    now=(entry.time+entry.pulse-offsets[j])&g_time_mask;
    assert(queuedpulses_internal_calculate_pulse_cplt_time(&entry,now)==
           reference_queuedpulses_internal_calculate_pulse_cplt_time(&entry,now));
   }
  }
 }
 puts("Queue deadlines: 2400000 random/wrap/late boundary cases, identical results.");
}
'''
with tempfile.TemporaryDirectory(prefix='queue-deadlines-') as directory:
    path = Path(directory)/'test.c'; path.write_text(source)
    for optimization in ['-Og', '-Ofast']:
        binary = Path(directory)/'test'
        subprocess.run(['gcc', optimization, str(path), '-o', str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
