#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
build_dir=$(mktemp -d)
trap 'rm -rf "$build_dir"' EXIT
includes="-ITests/IpcIntegration/Include -ICommon/Inc -IECU/Ipc/Inc -IECU/Config/Ipc/Inc -IECU/MiddleLayer/Inc -IMulticore/Ipc/Inc"
for module in IpcFifo IpcBuffer IpcTransfer; do
  includes="$includes -IMulticore/Ipc/$module/Inc"
done
symbols="ecu_ipc_init ecu_ipc_start ecu_ipc_loop_comm ecu_ipc_buffer_send ecu_ipc_buffer_receive ecu_ipc_transfer_send ecu_ipc_transfer_receive ecu_config_ipc_buffer ecu_config_ipc_transfer middlelayer_ipc_init middlelayer_ipc_start middlelayer_ipc_loop_comm ecu_ipc_buffer_fsm ecu_ipc_transfer_fsm"
for core in 4 7; do
  defs="-DCORE_CM$core"
  for name in $symbols; do
    defs="$defs -D${name}=cm${core}_${name}"
  done
  for path in ECU/Ipc/Src/ecu_ipc.c ECU/Ipc/Src/ecu_ipc_fsm.c ECU/Config/Ipc/Src/config_ipc.c ECU/MiddleLayer/Src/middlelayer_ipc.c; do
    gcc -std=gnu11 -O2 -g -Wall -Wextra -Werror -Wmissing-prototypes "$@" $includes $defs -c "$path" -o "$build_dir/cm${core}_${path##*/}.o"
  done
done
objcopy --weaken-symbol=ecu_config_ipc_shared "$build_dir/cm4_config_ipc.c.o"
gcc -std=gnu11 -O2 -g -Wall -Wextra -Werror -pthread "$@" $includes \
  -Wl,--defsym,__ipc_shared_bus_start__=ecu_config_ipc_shared "$build_dir"/*.o \
  Multicore/Ipc/IpcFifo/Src/*.c Multicore/Ipc/IpcBuffer/Src/*.c Multicore/Ipc/IpcTransfer/Src/*.c \
  Tests/IpcIntegration/integration_test.c -o "$build_dir/integration_test"
timeout 40 "$build_dir/integration_test"
