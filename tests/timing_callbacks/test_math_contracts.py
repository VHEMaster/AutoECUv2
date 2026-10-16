#!/usr/bin/env python3
"""Compare actual math implementations, including the pre-Ofast NaN contract."""
from pathlib import Path
import re
import subprocess
import tempfile
import sys

root=Path(__file__).resolve().parents[2]
with tempfile.TemporaryDirectory(prefix='math-contracts-') as directory:
    tmp=Path(directory)
    host=(root/'CM7/ECU/Common/Inc/compiler.h').read_text()
    for signature in ['uint32_t EnterCritical(void)', 'void ExitCritical(uint32_t prim)']:
        host=re.sub(r'STATIC_INLINE '+re.escape(signature)+r'\n\{.*?\n\}', signature+';', host, flags=re.S)
    host=re.sub(r'#define ITCM_FUNC .*','#define ITCM_FUNC',host)
    host=host.replace('#if __CORTEX_M == (7)', '#if 0')
    (tmp/'host.h').write_text(host)
    incs=[p for p in root.rglob('*') if p.is_dir() and 'CM4' not in p.parts and
          (p.name in ('Inc','Include','Math','ThreadSafe') or str(p).endswith('Inc/Legacy'))]
    flags=['-std=gnu11','-w','-DCORE_CM7','-DUSE_HAL_DRIVER','-DSTM32H745xx','-DSTM32_THREAD_SAFE_STRATEGY=2',
           '-include',str(tmp/'host.h'),'-ffunction-sections','-fdata-sections']+['-I'+str(p) for p in incs]
    originals={name:subprocess.check_output(['git','-C',str(root),'show','5ea8e8c:CM7/Math/'+name],text=True)
               for name in ['interpolation.c','interpolation.h','math_misc.c','math_misc.h']}
    for name in ['interpolation.h','math_misc.h']:
        (tmp/('reference_'+name)).write_text(originals[name])
    symbols=set()
    for name in ['interpolation.c','math_misc.c']:
        symbols.update(re.findall(r'^(?:[a-zA-Z_]\w*[ \t*]+)+([a-zA-Z_]\w*)\([^;{}]*\)\n\{', originals[name], re.M))
    failed = False
    for optimization in ['-Og','-Ofast']:
        objects=[]
        for name in ['interpolation.c','math_misc.c']:
            for prefix in ['', 'reference_']:
                src=root/'CM7/Math'/name
                extra=[]
                if not prefix and name == 'interpolation.c' and '--verify-nan-fix' in sys.argv:
                    patched = src.read_text()
                    guard = 'if(value != value || size == 0)'
                    assert patched.count(guard) == 2
                    patched = patched.replace(guard, 'uint32_t value_bits;\n  memcpy(&value_bits, &value, sizeof(value_bits));\n  if((value_bits & 0x7fffffffu) > 0x7f800000u || size == 0)')
                    src = tmp/'current_interpolation.c'
                    src.write_text(patched)
                if prefix:
                    src=tmp/(prefix+name)
                    src.write_text(originals[name].replace('"interpolation.h"','"reference_interpolation.h"').replace('"math_misc.h"','"reference_math_misc.h"'))
                    extra=['-D'+s+'=reference_'+s for s in symbols]
                obj=tmp/(prefix+name+'.o')
                subprocess.run(['gcc',*flags,optimization,*extra,'-c',str(src),'-o',str(obj)],check=True)
                objects.append(str(obj))
        source='''#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "interpolation.h"
#include "math_misc.h"
sMathInterpolateInput reference_math_interpolate_input(float,const float *,uint32_t);
float reference_math_calc_air_density_mgcc(float,float);
float reference_math_calc_ntc_thermistor(float,float,float,float);
static volatile uint32_t nan_bits=0x7fc00000u;
__attribute__((optimize("O2,no-fast-math")))
int main(void) {
 float nan_value; uint32_t bits=nan_bits; memcpy(&nan_value,&bits,4);
 const float axis[]={0,1}, output[]={11,22};
 sMathInterpolateInput old=reference_math_interpolate_input(nan_value,axis,2);
 sMathInterpolateInput current=math_interpolate_input(nan_value,axis,2);
 sMathInterpolateInput zero={0};
 printf("NaN input: old zero-result=%d current zero-result=%d, direct=0x",memcmp(&old,&zero,sizeof old)==0,memcmp(&current,&zero,sizeof current)==0);
 float direct=math_interpolate_1d_direct(nan_value,axis,output,2); memcpy(&bits,&direct,4); printf("%08x\\n",bits);
 double max_ntc=0, max_density=0;
 for(unsigned i=0;i<100000;i++) {
  float t=-80+(i%23000)*0.01f, p=0.1f+(i%3300)*0.001f;
  float a=reference_math_calc_air_density_mgcc(t,p), b=math_calc_air_density_mgcc(t,p);
  double e=fabs((double)a-b); if(e>max_density)max_density=e;
  assert(isfinite(b) && e<=0.00001*fmax(1.0,fabs(a)));
  float resistance=1+(i%10000)*100.03f, beta=2500+(i%2000)*0.73f;
  a=reference_math_calc_ntc_thermistor(25,10000,beta,resistance);
  b=math_calc_ntc_thermistor(25,10000,beta,resistance);
  e=fabs((double)a-b);if(e>max_ntc)max_ntc=e;
  assert(isfinite(b) && e<=0.001);
 }
 printf("Math physical range: 100000 density + 100000 NTC comparisons; max differences %.9g mg/cc and %.9g C.\\n",max_density,max_ntc);
 return memcmp(&current,&zero,sizeof current)!=0 || bits!=0;
}
'''
        (tmp/'test.c').write_text(source)
        exe=tmp/'test'
        subprocess.run(['gcc',*flags,optimization,str(tmp/'test.c'),*objects,'-Wl,--gc-sections','-lm','-o',str(exe)],check=True)
        print(optimization,flush=True)
        result = subprocess.run([str(exe)],check=False)
        failed |= result.returncode != 0
    if failed:
        raise SystemExit(1)
