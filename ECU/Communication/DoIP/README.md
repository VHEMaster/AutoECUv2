# DoIP

The component follows the ISOTP lifecycle and upstream/downstream naming. It is
independent of lwIP. `doip_loop()` and transport APIs execute in the main loop;
Router mailbox APIs may also execute in the communication interrupt. A try-lock
returns E_AGAIN instead of blocking an interrupted main-loop operation.

Supported wire profiles: ISO 13400-2 versions 0x02 and 0x03 over plain TCP/UDP.
This is a local diagnostic node, not an Ethernet gateway or a TLS implementation.
The configured default version is 0x02. The generic header is 8 bytes; diagnostic
messages add 16-bit source and target addresses before the unchanged UDS/OBD data.
All multi-byte wire fields use network byte order.

Two active TCP testers and one reserve connection are statically allocated.
Routing activation type 0x00 is supported. Duplicate tester addresses and socket
capacity are resolved by alive checks. Unsupported activation types are denied.
Initial inactivity, general inactivity, partial reception and alive checks have
separate timers. Elapsed time accumulates across the platform timebase wrap.

Each connection has a bounded receive buffer, one upstream diagnostic mailbox,
and a downstream frame queue. Payloads up to 1024 bytes are supported. A full
upstream mailbox receives diagnostic NACK 0x05. Full transmission queues retain
the current message for retry. Router replies carry the original connection index,
generation and tester address; replies belonging to disconnected testers fail with
E_ABORTED. No transaction ID is added to the standardized wire format.

UDP handles vehicle identification (unfiltered, EID, VIN), entity status and power
mode. Vehicle announcement construction is exposed separately to the transport.
Configure VIN/EID/GID for the actual ECU; the defaults are development identities.

Protocol references:
- ISO 13400-2 (transport and network layer services).
- AUTOSAR CP R24-11, DiagnosticOverIP, sections 7.2 through 7.5:
  https://www.autosar.org/fileadmin/standards/R24-11/CP/AUTOSAR_CP_SWS_DiagnosticOverIP.pdf

Public APIs are declared in Inc/doip.h. Length parameters are capacities on input
and byte counts on success. Stream reception additionally returns the consumed
byte count; the caller retains and retries the unconsumed suffix. Peeking TX data
does not remove it: consume exactly the bytes accepted by the transport.

## AutoECUv2 integration

Default endpoint: 10.77.0.2:13400 over the existing PPPoS connection. Logical
ECU address: 0x1000; tester range: 0x0E00..0x0FFF; functional address: 0xE400.
Router uses UDS_3 and OBD2_3, serializing requests from the TCP testers through
these protocol instances. CAN/KWP routes cannot share those instances. The Router
configuration is version 2; version 1 configurations are translated preserving
existing routes. A legacy configuration already using instance 3 leaves the new
DoIP route disabled rather than taking ownership away from its old transport.

LWIPDRV owns the raw TCP/UDP adapter and calls it from its main loop. All raw API
calls stay in this context. Receive pbuf chains are retained until consumed;
ERR_MEM leaves ownership with lwIP. TCP writes use COPY and only successful bytes
are removed from the DoIP queue. Listener allocation/bind failures are retried.
PPP reset/reconfiguration waits for the old PPP state machine to reach DEAD
before listening again. Link loss closes diagnostic sockets and invalidates reply
routes. SO_REUSE permits listener restart while previous sockets are in TIME_WAIT.

CM4's Router and DoIP contexts reside in `.commbss` in SRAM4 at 0x38000000.
This is a NOLOAD section: router_init() and doip_init() explicitly clear their
entire contexts before use. Shared IPC SRAM layout is unchanged. Clean-rebuild
lwip-static_CM4 with CM4 because LWIP_UDP and SO_REUSE are enabled for this adapter.

The wire profile excludes TLS, OEM routing activation and Ethernet activation-line
handling. PPP peer unicast is used for announcements; discovery broadcast behavior
on a system PPP adapter depends on the host routing configuration. Direct access
to the known ECU IP does not require discovery.
