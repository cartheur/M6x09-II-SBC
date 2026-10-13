# ASSIST09 Hardware Acceptance

Date: 2026-10-13

The earlier corrected `roms/assist09-27c128.bin` image, SHA-256 `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843`, passed the serial/load/RAM smoke path only while a temporary CTS-to-ground jumper was installed. The final no-jumper acceptance is the RTS/CTS loopback candidate documented in [NEXT-RTS-CTS-LOOPBACK-TEST.md](NEXT-RTS-CTS-LOOPBACK-TEST.md).

## Preconditions

- The programmed EPROM completed a full-device readback matching that image.
- Terminal settings were `115200,n,8,1`, with no handshake.
- The MC6850-compatible ACIA CTS input is U1 pin 24. U1 pin 23 is DCD.
- EasyEDA PCB evidence shows U1 pin 24 CTS is routed to U1 pin 5 RTS; see [CTS-RTS-PCB.jpg](../images/CTS-RTS-PCB.jpg). The schematic image did not make that PCB connection evident.
- A temporary jumper connected U1 pin 24 CTS to U1 pin 1 VSS/GND with power removed.

## Acceptance Result

After reset, the terminal displayed `ASSIST09` and the `>` prompt. The smoke S-record at `src/assist-09/assist09-smoke.s19` was loaded with `L`, then run with `G 1000`.

```text
ASSIST09 RAM SMOKE TEST PASSED
ASSIST09
```

The post-test banner is ASSIST09 re-entry after the smoke program invokes the monitor.

## Terminal Fix

The Tcl terminal previously waited for a CR after every transmitted line. That cannot work for ASSIST09 S-record loading because the loader suppresses echo and CR output. `terminal/m6x09-terminal.tcl` now recognizes S-record files and sends every record with its existing per-character delay, without waiting for a CR between records.

## Superseding Correction

The temporary jumper was removed. The monitor's `$51` ACIA control byte drives RTS high; because RTS and CTS are connected on the PCB, it raises CTS and inhibits `TDRE`. The replacement `$11` control byte drives RTS low with transmit interrupt disabled while retaining the required serial framing and divide-by-16 rate. Program and readback-verify that candidate without a CTS jumper, then repeat this smoke test.
