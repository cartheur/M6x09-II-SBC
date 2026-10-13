# ASSIST09 Hardware Acceptance

Date: 2026-10-13

The corrected `roms/assist09-27c128.bin` image, SHA-256 `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843`, passed physical-board acceptance.

## Preconditions

- The programmed EPROM completed a full-device readback matching that image.
- Terminal settings were `115200,n,8,1`, with no handshake.
- The MC6850-compatible ACIA CTS input is U1 pin 24. U1 pin 23 is DCD.
- The schematic leaves U1 pin 24 CTS floating; the measured high level inhibited `TDRE` and therefore monitor transmission.
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

## Open Hardware Item

Retain the temporary CTS-to-ground jumper until a permanent defined-low CTS solution is installed and this acceptance test is repeated. P1 pin 2, labelled CTS, is an unconnected stub in the current schematic; it does not currently reach U1 pin 24.
