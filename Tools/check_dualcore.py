#!/usr/bin/env python3
"""Check linked ownership and the shared address contract, not just source paths."""
import argparse, pathlib, subprocess
p=argparse.ArgumentParser()
p.add_argument('--configuration', choices=['Debug','Release'], default='Debug')
p.add_argument('--nm', default='arm-none-eabi-nm')
a=p.parse_args()
root=pathlib.Path(__file__).resolve().parents[1]
def symbols(core):
    elf=root/'build'/a.configuration/core/(core+'.elf')
    text=subprocess.check_output([a.nm,'--defined-only',str(elf)],text=True)
    return {fields[-1]:(int(fields[0],16),fields[-2]) for line in text.splitlines() if len(fields:=line.split())==3}
m4,m7=symbols('CM4'),symbols('CM7')
for name in ['g_ipc','__ipc_start__','__ipc_end__']:
    assert m4[name][0] == m7[name][0], (name,m4[name],m7[name])
assert m4['g_ipc'][0] == 0x30030000
assert m4['__ipc_end__'][0] <= 0x30048000
assert m4['_estack'][0] == 0x30030000
for name in ['can_init','isotp_loop','obd2_loop','router_loop_comm','signals_loop_comm','hfdcan1','huart1','huart3','HAL_FDCAN_IRQHandler','HAL_UART_IRQHandler']:
    assert name in m4, ('missing from CM4',name)
    assert name not in m7, ('communication implementation remains on CM7',name)
for name in ['FDCAN1_IT0_IRQHandler','FDCAN1_IT1_IRQHandler','FDCAN_CAL_IRQHandler','USART1_IRQHandler','USART3_IRQHandler'] + ['DMA1_Stream'+str(i)+'_IRQHandler' for i in [0,1,4,5]]:
    assert m4[name][1] == 'T', ('CM4 handler missing',name)
    assert m7[name][1] == 'W', ('CM7 still owns handler',name,m7[name])
print(a.configuration+': linked ownership and identical IPC placement passed; shared bytes='+str(m4['__ipc_end__'][0]-m4['__ipc_start__'][0]))
