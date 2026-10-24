#!/usr/bin/env python3
import argparse,os,pathlib,subprocess,tempfile
p=argparse.ArgumentParser()
p.add_argument("--optimization",choices=["O0","Og","O2","Ofast"],default="O2")
p.add_argument("--sanitize",action="store_true")
a=p.parse_args()
root=pathlib.Path(__file__).resolve().parents[1]
cc=os.environ.get('CC','gcc')
with tempfile.TemporaryDirectory(prefix='autoecu-ipc-') as temp:
    out=pathlib.Path(temp)
    flags=['-'+a.optimization,'-std=c11','-Wall','-Wextra','-Werror','-I'+str(root/'Tests/ipc_stubs'),'-I'+str(root/'Common/IPC/Inc'),'-I'+str(root/'CM7/ECU/Common/Inc'),'-I'+str(root/'CM7/ECU/Config/Common/Inc')]
    if a.sanitize: flags += ['-fsanitize=address,undefined','-fno-omit-frame-pointer']
    def run(*args):subprocess.run([str(x) for x in args],check=True,cwd=root,timeout=60)
    run(cc,*flags,'-pthread','Tests/test_ipc.c','Common/IPC/Src/ipc.c','-o',out/'rpc')
    run(out/'rpc')
    source='Common/IPC/Src/comm_ipc_parameters.c'
    run(cc,*flags,'-DCORE_CM4','-Dcomm_ipc_parameters_poll=cm4_poll','-Dcomm_ipc_parameter_write=cm4_write','-Decu_config_common_get_parameter_ptr_by_id=cm4_get_ptr','-Decu_config_common_get_parameter_value_by_id=cm4_get_value','-c',source,'-o',out/'cm4.o')
    run(cc,*flags,'-Dcomm_ipc_parameters_poll=cm7_poll','-Decu_config_common_get_parameter_ptr_by_id=engine_get_ptr','-c',source,'-o',out/'cm7.o')
    run(cc,*flags,'-Decu_config_common_get_parameter_ptr_by_id=engine_get_ptr','Tests/test_ipc_parameters.c','Common/IPC/Src/ipc.c','CM7/ECU/Config/Communication/Src/comm_ipc_dispatch.c',out/'cm4.o',out/'cm7.o','-o',out/'parameters')
    run(out/'parameters')
    signal_flags=flags+['-DCONFIG_COMMUNICATION_INC_CONFIG_COMM_H_','-include',str(root/'Tests/ipc_stubs/config_comm.h'),'-Wno-sign-compare','-DCORE_CM4','-Decu_config_common_get_parameter_ptr_by_id=cm4_get_ptr','-Dcomm_ipc_parameter_write=cm4_write']
    for directory in ['CM4/ECU/Communication/Signals/Inc','CM4/ECU/Drivers/Can/Inc','CM7/ECU/Config/Communication/Inc','CM7/ECU/Config/Versioned/Communication/Inc']:
        signal_flags += ['-I'+str(root/directory)]
    signal_sources=['Tests/test_ipc_signals.c','Common/IPC/Src/ipc.c','CM4/ECU/Communication/Signals/Src/signals.c','CM4/ECU/Communication/Signals/Src/signals_private.c','CM4/ECU/Config/Communication/Src/config_signals.c']
    run(cc,*signal_flags,*signal_sources,out/'cm4.o',out/'cm7.o','-lm','-o',out/'signals')
    run(out/'signals')
    obd_flags=signal_flags+['-Wno-unused-parameter','-Decu_config_common_get_parameter_value_by_id=cm4_get_value','-I'+str(root/'CM4/ECU/Communication/OBD2/Inc')]
    obd_sources=['Tests/test_ipc_obd2.c','Common/IPC/Src/ipc.c','CM4/ECU/Communication/OBD2/Src/obd2.c','CM4/ECU/Communication/OBD2/Src/obd2_private.c','CM4/ECU/Config/Communication/Src/config_obd2.c']
    run(cc,*obd_flags,*obd_sources,out/'cm4.o',out/'cm7.o','-lm','-o',out/'obd2')
    run(out/'obd2')
