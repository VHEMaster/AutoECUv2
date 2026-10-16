#!/usr/bin/env python3
"""Inspect a linked CM7 ELF using GNU readelf; no Python packages required."""
import argparse
from pathlib import Path
import re
import subprocess
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('elf', type=Path)
parser.add_argument('--readelf', default='arm-none-eabi-readelf')
parser.add_argument('--capacity', type=int, default=65536)
parser.add_argument('--top', type=int, default=15)
args = parser.parse_args()
sections = subprocess.check_output([args.readelf, '-SW', str(args.elf)], text=True)
section = re.search(r'\[\s*(\d+)\]\s+\.itcm_func\s+PROGBITS\s+[0-9a-fA-F]+\s+[0-9a-fA-F]+\s+([0-9a-fA-F]+)', sections)
if not section:
    sys.exit('ELF has no .itcm_func section')
index, size = section[1], int(section[2], 16)
symbols = subprocess.check_output([args.readelf, '-sW', str(args.elf)], text=True)
live = []
for line in symbols.splitlines():
    fields = line.split()
    if len(fields) < 8 or fields[3] != 'FUNC':
        continue
    name, location, length = fields[7], fields[6], int(fields[2])
    if name in ('flash_fsm', 'qspi_fsm_init') and location == index:
        sys.exit(name+' unexpectedly remains in ITCM')
    if location == index:
        live.append((length, name))
print(f'ITCM: {size} / {args.capacity} bytes; free: {args.capacity-size}')
for length, name in sorted(live, reverse=True)[:args.top]:
    print(f'{length:6d}  {name}')
if size > args.capacity:
    sys.exit('ITCM exceeds physical capacity')
