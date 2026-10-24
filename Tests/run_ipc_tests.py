#!/usr/bin/env python3
import os,pathlib,subprocess,tempfile
root=pathlib.Path(__file__).resolve().parents[1]
cc=os.environ.get('CC','gcc')
with tempfile.TemporaryDirectory(prefix='autoecu-ipc-') as temp:
    out=pathlib.Path(temp)
    flags=['-std=c11','-Wall','-Wextra','-Werror','-I'+str(root/'Tests/ipc_stubs'),'-I'+str(root/'Common/IPC/Inc'),'-I'+str(root/'CM7/ECU/Common/Inc'),'-I'+str(root/'CM7/ECU/Config/Common/Inc')]
    def run(*args):subprocess.run([str(x) for x in args],check=True,cwd=root)
    run(cc,*flags,'-pthread','Tests/test_ipc.c','Common/IPC/Src/ipc.c','-o',out/'rpc')
    run(out/'rpc')
    source='Common/IPC/Src/comm_ipc_parameters.c'
    run(cc,*flags,'-DCORE_CM4','-Dcomm_ipc_parameters_poll=cm4_poll','-Dcomm_ipc_parameter_write=cm4_write','-Decu_config_common_get_parameter_ptr_by_id=cm4_get_ptr','-Decu_config_common_get_parameter_value_by_id=cm4_get_value','-c',source,'-o',out/'cm4.o')
    run(cc,*flags,'-Dcomm_ipc_parameters_poll=cm7_poll','-Decu_config_common_get_parameter_ptr_by_id=engine_get_ptr','-c',source,'-o',out/'cm7.o')
    run(cc,*flags,'Tests/test_ipc_parameters.c','Common/IPC/Src/ipc.c',out/'cm4.o',out/'cm7.o','-o',out/'parameters')
    run(out/'parameters')
