# TunerStudio transport scaffold

The CM4 owns the TCP listener, direct RS-422 transport, and stream handler. CM7 configures and resets
the component through the existing communication RPC mechanism. Configuration
V1 has enabled (default true), lwip_enabled (default true), rs422_enabled
(default false), port (default TCP 2000), and rs422_instance (default
ECU_COMM_RS422_1). EFI Analytics'
ECU Definition File specification lists `defaultIpPort = 2000` as an example;
the port is configurable, not mandatory.

The listener is bound to the PPP netif, follows link state, and runs in
`lwipdrv_loop_main` alongside DoIP. One concurrent client is supported.
A change to configuration or a reset closes the existing connection and
restarts the listener on the next poll. Configuration/reset RPC executes in
communication IRQ and retries with E_AGAIN while the main-loop transport
holds the component lock. Flash allocation: block 11, sector 8.

The receive callback only retains a pbuf (or refuses further data with
ERR_MEM). A transport-independent arbiter with separate RX/TX channels selects input
round-robin and returns replies to the originating transport. The component
main loop calls `tunerstudio_protocol_loop()` in
[`tunerstudio_protocol.c`](Src/tunerstudio_protocol.c), which owns the
protocol processing scaffold and currently echoes a bounded number of
chunks verbatim, including NULs. No TunerStudio frame parsing, signature, command
dispatch or calibration access is implemented yet. TCP chunk boundaries are
not protocol frame boundaries. Future framing must maintain independent
state per stream; the arbiter does not merge partial frames.

Direct RS-422 uses the selected instance's existing UART settings and DMA
buffers without PPP framing. Both transports can be enabled independently.
When direct RS-422 is enabled, that physical port is reserved for TunerStudio;
lwIP neither consumes nor transmits bytes on it. An active PPP session on the
same port is closed without carrier, and queued TX is drained before changing
ownership. PPP over USB remains available for TCP and DoIP. No automatic
protocol detection or multiplexing on the same serial port is implemented.
Stop traffic and reconnect when changing ownership; already received bytes
in the UART DMA ring are not flushed. The direct transport is polled from tunerstudio_loop_main, even if LwIPDrv
is disabled. LwIPDrv only coordinates ownership of the shared physical port.
LwIPDrv V1 independently enables RS-422 and USB (both true by default).
For direct serial plus TCP over USB, set LwIPDrv rs422_enabled=false and
usb_enabled=true. Restore defaults or explicitly set these flags after
upgrading older development firmware using the same V1 layout.

## Smoke test

With a system PPP network interface (or a local TCP proxy to the ECU), run
the following using Python 3; substitute the actual ECU address and port:

```python
import socket

payload = bytes(range(256)) * 16
with socket.create_connection(("ECU_IP", 2000), timeout=5) as sock:
    sock.sendall(payload)
    received = bytearray()
    while len(received) < len(payload):
        chunk = sock.recv(len(payload) - len(received))
        if not chunk:
            raise RuntimeError("Premature EOF")
        received.extend(chunk)
    assert received == payload
    sock.shutdown(socket.SHUT_WR)
    assert sock.recv(1) == b""
print("TunerStudio TCP echo OK")
```

Also test reconnect, changing the port, disabling the component, link loss,
and simultaneous DoIP activity. The TunerStudio application itself will not
complete identification until the protocol is implemented.

For direct RS-422, enable the selected RS-422 driver, set rs422_enabled and
select its instance in the TunerStudio configuration. With the ECU's serial
settings (115200 8N1 by default), use Python 3 with pyserial:

```python
import serial

payload = bytes(range(256)) * 4
with serial.Serial("COMx", 115200, timeout=5) as port:
    port.write(payload)
    assert port.read(len(payload)) == payload
print("TunerStudio RS-422 echo OK")
```

Test RS-422 only with LwIPDrv disabled, TCP only over PPP/RS-422 and PPP/USB,
and both transports concurrently with PPP/USB. Check reconfiguration while
PPP is active on RS-422. The serial path inherits the RS-422 driver's RX ring
overflow limitations.
