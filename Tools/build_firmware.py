#!/usr/bin/env python3
"""Build the checked-in CubeIDE source lists with GNU Arm Embedded GCC."""
import argparse, concurrent.futures, os, pathlib, subprocess, sys, xml.etree.ElementTree as ET
ROOT = pathlib.Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument('--core', choices=['CM4','CM7'], required=True)
p.add_argument('--configuration', choices=['Debug','Release'], default='Debug')
p.add_argument('--cc', default=os.environ.get('ARM_CC','arm-none-eabi-gcc'))
p.add_argument('--output', type=pathlib.Path)
p.add_argument('-j', type=int, default=4)
a = p.parse_args()
project = ROOT/a.core
out = (a.output or ROOT/'build'/a.configuration/a.core).resolve(); out.mkdir(parents=True,exist_ok=True)
(out/(a.core+'.elf')).unlink(missing_ok=True)
xml = ET.parse(project/'.cproject')
configs = xml.findall('.//cconfiguration')
config = next(c for c in configs if c.find('.//configuration').get('name') == 'Build_'+a.configuration)
compiler = next(t for t in config.findall('.//tool') if t.get('superClass','').endswith('tool.c.compiler'))
includes = []; defines = []; optimization = '-O0'
for opt in compiler.findall('option'):
    if opt.get('superClass','').endswith('option.optimization.level'):
        mode = opt.get('value','').split('.')[-1]
        optimization = {'og':'-Og','ofast':'-Ofast','os':'-Os','o1':'-O1','o2':'-O2','o3':'-O3'}.get(mode,'-O0')
    if opt.get('valueType') == 'includePath':
        includes.extend(str((project/'Debug'/v.get('value').replace('${workspace_loc:/${ProjName}}',str(project))).resolve()) for v in opt.findall('listOptionValue'))
    if opt.get('valueType') == 'definedSymbols':
        defines.extend(v.get('value') for v in opt.findall('listOptionValue'))
flags = ['-mcpu=cortex-m'+('4' if a.core=='CM4' else '7'),'-mthumb','-mfpu=fpv4-sp-d16' if a.core=='CM4' else '-mfpu=fpv5-d16','-mfloat-abi=hard','-std=gnu11',optimization,'-g3','-ffunction-sections','-fdata-sections','-Wall','-Wno-unused-function']
flags += ['-I'+v for v in dict.fromkeys(includes)] + ['-D'+v for v in defines]
links = ET.parse(project/'.project').findall('.//linkedResources/link')
sources = set()
entries = config.findall('.//sourceEntries/entry')
for e in entries:
    path = project/e.get('name')
    if path.is_dir():
        sources.update(f for f in path.rglob('*') if f.suffix in ['.c','.s','.S'] and not any(ex and str(f.relative_to(path)).startswith(ex) for ex in e.get('excluding','').split('|')))
for link in links:
    name = link.findtext('name')
    if not any(name==e.get('name') or name.startswith(e.get('name')+'/') for e in entries): continue
    path = pathlib.Path(link.findtext('locationURI').replace('PARENT-1-PROJECT_LOC',str(ROOT)))
    if path.is_dir(): sources.update(f for f in path.rglob('*') if f.suffix in ['.c','.s','.S'])
    elif path.suffix in ['.c','.s','.S']: sources.add(path)
objects = []
def compile_one(src):
    obj = out/(str(src.relative_to(ROOT)).replace('/','__')+'.o')
    cmd = [a.cc,*flags,'-c',str(src),'-o',str(obj)]
    result = subprocess.run(cmd,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    return obj,result
fail = False
with concurrent.futures.ThreadPoolExecutor(a.j) as pool:
    for obj,result in pool.map(compile_one, sorted(sources)):
        if result.stdout: print(result.stdout,end='')
        fail |= result.returncode != 0
        objects.append(str(obj))
if fail: sys.exit(1)
elf = out/(a.core+'.elf')
cmd = [a.cc,*flags,'-T'+str(project/'STM32H745IITX_FLASH.ld'),'-Wl,--gc-sections','-Wl,-Map='+str(out/(a.core+'.map')),'--specs=nano.specs','--specs=nosys.specs','-Wl,--start-group',*objects,'-lm','-lc','-Wl,--end-group','-o',str(elf)]
if subprocess.run(cmd).returncode: sys.exit(1)
subprocess.run([str(pathlib.Path(a.cc).with_name(pathlib.Path(a.cc).name.replace('gcc','size'))),str(elf)],check=True)
print('Built',elf,'from',len(sources),'sources')
