#!/usr/bin/env python3
"""Run real 60-2/36-2 decoders against the pre-audit revision, edge by edge."""
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
BASELINE = '0ef0826'
with tempfile.TemporaryDirectory(prefix='ckp-decoders-') as directory:
    tmp = Path(directory)
    host = (ROOT/'CM7/ECU/Common/Inc/compiler.h').read_text()
    host = re.sub(r'STATIC_INLINE uint32_t EnterCritical\(void\)\n\{.*?\n\}', 'uint32_t EnterCritical(void);', host, flags=re.S)
    host = re.sub(r'STATIC_INLINE void ExitCritical\(uint32_t prim\)\n\{.*?\n\}', 'void ExitCritical(uint32_t prim);', host, flags=re.S)
    host = re.sub(r'#define ITCM_FUNC .*', '#define ITCM_FUNC', host)
    host = host.replace('#if __CORTEX_M == (7)', '#if 0')
    (tmp/'host.h').write_text(host)
    incs = [p for p in ROOT.rglob('*') if p.is_dir() and 'CM4' not in p.parts and
            (p.name in ('Inc', 'Include', 'Math', 'ThreadSafe') or str(p).endswith('Inc/Legacy'))]
    flags = ['-std=gnu11', '-w', '-DCORE_CM7', '-DUSE_HAL_DRIVER', '-DSTM32H745xx',
             '-DSTM32_THREAD_SAFE_STRATEGY=2', '-ffunction-sections', '-fdata-sections',
             '-include', str(tmp/'host.h')] + ['-I'+str(p) for p in incs]
    for optimization in ['-Og', '-Ofast']:
        objects = []
        declarations = []
        cases = []
        for teeth, edges in [(60, 116), (36, 68)]:
            name = f'ckp_signal_regular_{teeth}_2'
            path = f'CM7/ECU/Sensors/CKP/Src/{name}.c'
            for prefix in ['', 'reference_']:
                source = (subprocess.check_output(['git', '-C', str(ROOT), 'show', BASELINE+':'+path], text=True)
                          if prefix else (ROOT/path).read_text())
                probe = prefix+name+'_runtime'
                source += f'\nconst void *{probe}(void) {{ return {name}_ctx; }}\n'
                source += f'unsigned {probe}_size(void) {{ return sizeof({name}_ctx); }}\n'
                file = tmp/(prefix+name+'.c'); file.write_text(source)
                obj = file.with_suffix('.o'); objects.append(str(obj))
                rename = ['-D'+name+'_'+suffix+'='+prefix+name+'_'+suffix
                          for suffix in ['init', 'signal', 'loop_main', 'loop_fast', 'loop_slow']] if prefix else []
                subprocess.run(['gcc', *flags, optimization, *rename, '-c', str(file), '-o', str(obj)], check=True)
                declarations += [f'error_t {prefix}{name}_init(ckp_ctx_t *, ckp_instance_t, void **);',
                    f'void {prefix}{name}_signal(ckp_ctx_t *, ecu_gpio_input_level_t, void *);',
                    f'void {prefix}{name}_loop_slow(ckp_ctx_t *, void *);',
                    f'const void *{probe}(void); unsigned {probe}_size(void);']
            cases.append(f'{{{edges}, {name}_init, reference_{name}_init, {name}_signal, reference_{name}_signal, '
                         f'{name}_loop_slow, reference_{name}_loop_slow, {name}_runtime, reference_{name}_runtime, {name}_runtime_size}},')
        source = '''#include "ckp.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static volatile uint32_t clock_us;
volatile time_us_t *g_time_timebase = &clock_us;
time_us_t g_time_mask = 0xffffffffu;
static unsigned critical;
uint32_t EnterCritical(void) { return critical++; }
void ExitCritical(uint32_t prim) { assert(critical == prim+1); critical--; }
'''+ '\n'.join(declarations)+'''
struct test_case {
 unsigned edges;
 error_t (*init)(ckp_ctx_t *, ckp_instance_t, void **), (*ref_init)(ckp_ctx_t *, ckp_instance_t, void **);
 void (*signal)(ckp_ctx_t *, ecu_gpio_input_level_t, void *), (*ref_signal)(ckp_ctx_t *, ecu_gpio_input_level_t, void *);
 void (*slow)(ckp_ctx_t *, void *), (*ref_slow)(ckp_ctx_t *, void *);
 const void *(*runtime)(void), *(*ref_runtime)(void);
 unsigned (*runtime_size)(void);
} cases[] = {
'''+ '\n'.join(cases)+'''
};
int main(void) {
 uint32_t rng=123; unsigned valid_edges=0;
 for(unsigned c=0;c<2;c++) for(unsigned scenario=0;scenario<8;scenario++) {
  struct test_case *t=&cases[c]; ckp_ctx_t a={0}, b={0}; void *u, *v;
  g_time_mask = TIME_TIMEBASE_MASK;
  clock_us=g_time_mask-1000;
  a.config.desync_on_error=b.config.desync_on_error=scenario&2;
  assert(t->init(&a,0,&u)==E_OK && t->ref_init(&b,0,&v)==E_OK);
  for(unsigned i=0;i<16000;i++) {
   rng=rng*1664525u+1013904223u;
   unsigned delta=50+(i/1000)%20;
   if(i%t->edges<2) delta*=3;
   if(scenario>=4) delta+=rng%9;
   unsigned level=i&1;
   if(scenario>=6 && i%557==0) level^=1;
   clock_us=(clock_us+delta)&g_time_mask;
   t->signal(&a,level ? ECU_IN_LEVEL_HIGH : ECU_IN_LEVEL_LOW,u);
   t->ref_signal(&b,level ? ECU_IN_LEVEL_HIGH : ECU_IN_LEVEL_LOW,v);
   if(i%4093==4092) {
    clock_us=(clock_us+100000)&g_time_mask;
    t->slow(&a,u); t->ref_slow(&b,v);
   }
   assert(memcmp(&a,&b,sizeof(a))==0);
   assert(memcmp(t->runtime(),t->ref_runtime(),t->runtime_size())==0);
   assert(!critical);
   valid_edges+=a.data.validity==CKP_DATA_VALID;
  }
 }
 assert(valid_edges>100000);
 printf("CKP decoders: 256000 edges, identical data/diagnostics/private state (%u valid).\\n",valid_edges);
}
'''
        file = tmp/'test.c'; file.write_text(source)
        exe = tmp/'test'
        subprocess.run(['gcc', *flags, optimization, str(file), *objects, '-Wl,--gc-sections', '-lm', '-o', str(exe)], check=True)
        subprocess.run([str(exe)], check=True)
