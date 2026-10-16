#!/usr/bin/env python3
"""Compare timing callbacks against an immutable pre-optimization revision.

No target peripherals are accessed. Peripheral calls and time are deterministic.
Run from any directory; pass --baseline to compare another git revision.
"""
import argparse
import pathlib
import re
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
FILES = [f'CM7/ECU/Timings/{folder}/Src/timing_{folder.lower()}.c'
         for folder in ('Base', 'Common', 'Ignition', 'Injection', 'CyBalance', 'KnockWindow')]
FILES += ['CM7/Math/interpolation.c', 'CM7/Math/math_misc.c', 'CM7/ECU/Drivers/Time/Src/time.c']


def run(args):
    subprocess.run(args, check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', default='0ef0826')
    parser.add_argument('--optimization', choices=['Og', 'Ofast'], default='Og')
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='timing-callbacks-') as tmp:
        tmp = pathlib.Path(tmp)
        # Retain the actual public types and macros; substitute only ARM instructions.
        host = (ROOT / 'CM7/ECU/Common/Inc/compiler.h').read_text()
        host = re.sub(r'STATIC_INLINE uint32_t EnterCritical\(void\)\n\{.*?\n\}', 'uint32_t EnterCritical(void);', host, flags=re.S)
        host = re.sub(r'STATIC_INLINE void ExitCritical\(uint32_t prim\)\n\{.*?\n\}', 'void ExitCritical(uint32_t prim);', host, flags=re.S)
        host = re.sub(r'#define BREAKPOINT\(x\).*', '#define BREAKPOINT(x) ((void)0)', host)
        host = re.sub(r'#define ITCM_FUNC .*', '#define ITCM_FUNC', host)
        host = host.replace('#if __CORTEX_M == (7)', '#if 0')
        host += '\n#include <math.h>\n#define MATH_FAST_H_\n#define fast_sqrt(x) sqrtf(x)\n'
        (tmp / 'host.h').write_text(host)
        incs = [p for p in ROOT.rglob('*') if p.is_dir() and 'CM4' not in p.parts
                and (p.name in ('Inc', 'Include', 'Math', 'ThreadSafe') or str(p).endswith('Inc/Legacy'))]
        flags = ['-std=gnu11', '-'+args.optimization, '-g', '-w', '-DCORE_CM7', '-DUSE_HAL_DRIVER',
                 '-DSTM32H745xx', '-DSTM32_THREAD_SAFE_STRATEGY=2', '-ffunction-sections',
                 '-fdata-sections', '-include', str(tmp / 'host.h')] + ['-I' + str(p) for p in incs]
        if 'math_interpolate_1d_direct(' in (ROOT/'CM7/Math/interpolation.h').read_text():
            flags.append('-DHAVE_DIRECT_LOOKUP')
        if 'timing_calculate_cylinders_positions(' in (ROOT/'CM7/ECU/Timings/Common/Inc/timing_common.h').read_text():
            flags.append('-DHAVE_BATCH_POSITIONS')
        # Use committed engine-default timing tables in the integration scenarios.
        defaults = ''
        for name in ('ignition', 'injection', 'cybalance', 'knockwindow'):
            text = (ROOT / f'CM7/ECU/Config/Timings/Src/config_{name}.c').read_text()
            defaults += re.search(r'static const '+name+r'_config_t .*?\n};', text, re.S)[0] + '\n'
        (tmp / 'defaults.h').write_text(defaults)
        # Old public callback signatures are required for pre-snapshot references.
        reference_headers = tmp/'reference_headers'
        reference_headers.mkdir()
        for source in FILES:
            if '/Timings/' not in source:
                continue
            header = source.replace('/Src/', '/Inc/').replace('.c', '.h')
            (reference_headers/pathlib.Path(header).name).write_text(subprocess.check_output(
                ['git', '-C', str(ROOT), 'show', args.baseline + ':' + header], text=True))
        (reference_headers/'interpolation.h').write_text(subprocess.check_output(
            ['git', '-C', str(ROOT), 'show', args.baseline + ':CM7/Math/interpolation.h'], text=True))
        originals = {f: subprocess.check_output(['git', '-C', str(ROOT), 'show', args.baseline + ':' + f], text=True)
                     for f in FILES if not f.endswith('/time.c')}
        # The baseline search must retain its original external declaration/body.
        (tmp / 'reference_math_misc.h').write_text(subprocess.check_output(
            ['git', '-C', str(ROOT), 'show', args.baseline + ':CM7/Math/math_misc.h'], text=True))
        originals = {f: text.replace('"math_misc.h"', '"reference_math_misc.h"')
                     for f, text in originals.items()}
        if 'math_interpolate_1d_direct(' not in originals['CM7/Math/interpolation.c']:
            originals['CM7/Math/interpolation.c'] += '\nfloat math_interpolate_1d_direct(float value, const float *input, const float *output, uint32_t size)\n{\n  return math_interpolate_1d(math_interpolate_input(value, input, size), output);\n}\n'
        symbols = set()
        for text in originals.values():
            # Keep return-type tokens on one line; attributes above a definition
            # must not be mistaken for callable symbols (e.g. __attribute__).
            symbols.update(re.findall(r'^(?:[a-zA-Z_]\w*[ \t*]+)+([a-zA-Z_]\w*)\([^;{}]*\)\n\{', text, re.M))
        symbols.discard('timing_pulse_schedule')  # mocked, not linked to real GPIO
        objects = []
        for prefix in ('current', 'reference'):
            for f in FILES:
                if prefix == 'reference' and f.endswith('/time.c'):
                    continue
                src = ROOT / f
                extra = []
                if prefix == 'reference':
                    src = tmp / (prefix + '_' + pathlib.Path(f).name)
                    src.write_text(originals[f])
                    extra = ['-D'+s+'=reference_'+s for s in sorted(symbols)]
                if f.endswith('/timing_common.c'):
                    extra += ['-Dtiming_pulse_schedule='+prefix+'_unused_pulse_schedule']
                obj = tmp / (prefix + '_' + pathlib.Path(f).stem + '.o')
                reference_flags = ['-I'+str(reference_headers)] if prefix == 'reference' else []
                run(['gcc', *reference_flags, *flags, *extra, '-c', str(src), '-o', str(obj)])
                objects.append(str(obj))
        obj = tmp / 'test.o'
        test_source = (ROOT/'tests/timing_callbacks/test.c').read_text()
        if 'time_init_timebase(volatile time_us_t *timebase);' in (ROOT/'CM7/ECU/Drivers/Time/Inc/time.h').read_text():
            test_source = test_source.replace('time_init_timebase(&clock_us, UINT_MAX)', 'time_init_timebase(&clock_us)')
        (tmp/'test.c').write_text(test_source)
        run(['gcc', *flags, '-I'+str(tmp), '-c', str(tmp/'test.c'), '-o', str(obj)])
        exe = tmp / 'test'
        run(['gcc', '-Wl,--gc-sections', *objects, str(obj), '-lm', '-o', str(exe)])
        run([str(exe)])


if __name__ == '__main__':
    try:
        main()
    except subprocess.CalledProcessError as error:
        sys.exit(f'Test command failed (exit {error.returncode}): {error.cmd[0]}')
