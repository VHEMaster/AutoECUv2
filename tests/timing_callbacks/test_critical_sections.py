#!/usr/bin/env python3
"""Exercise actual critical-section helper bodies with a simulated PRIMASK."""
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
header = (root / 'CM7/ECU/Common/Inc/compiler.h').read_text()
bodies = []
for signature in ['uint32_t EnterCritical(void)', 'void ExitCritical(uint32_t prim)']:
    body = re.search(r'STATIC_INLINE '+re.escape(signature)+r'\n\{.*?\n\}', header, re.S)
    if body is None:
        raise SystemExit('Missing inline helper: '+signature)
    bodies.append(body[0])
source = '''#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#define STATIC_INLINE static inline __attribute__((always_inline))
static volatile uint32_t mask;
static unsigned enables;
static uint32_t __get_PRIMASK(void) { return mask; }
static void __disable_irq(void) { mask = 1; }
static void __enable_irq(void) { enables++; mask = 0; }
'''+'\n'.join(bodies)+'''
int main(void) {
  uint32_t random = 123;
  for(unsigned initial = 0; initial < 2; initial++) {
    uint32_t stack[32]; unsigned depth = 0;
    mask = initial;
    for(unsigned i = 0; i < 100000; i++) {
      random = random * 1664525u + 1013904223u;
      if(depth == 0 || (depth < 32 && (random & 4))) {
        uint32_t before = mask;
        stack[depth++] = EnterCritical();
        assert(stack[depth-1] == before && mask == 1);
      } else {
        uint32_t restore = stack[--depth]; unsigned before = enables;
        ExitCritical(restore);
        assert(mask == restore);
        assert(enables == before + (restore == 0));
      }
    }
    while(depth) ExitCritical(stack[--depth]);
    assert(mask == initial);
  }
  puts("Critical sections: 200000 nested transitions; PRIMASK preserved.");
}
'''
with tempfile.TemporaryDirectory(prefix='critical-sections-') as directory:
    source_path = Path(directory)/'test.c'
    source_path.write_text(source)
    for optimization in ['-Og', '-Ofast']:
        binary = Path(directory)/'test'
        subprocess.run(['gcc', optimization, str(source_path), '-o', str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
