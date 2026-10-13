# ASSIST09 Hardware Acceptance: CTS And ACIA Transmit Enable

Date: 2026-10-13

Status: passed. The corrected EPROM booted, an S-record loaded into RAM, and the RAM smoke test passed after grounding the previously floating CTS input. The temporary CTS jumper remains installed pending a permanent hardware change.

## Established Evidence

- `roms/readback.bin` is byte-for-byte identical to the corrected `roms/assist09-27c128.bin`: SHA-256 `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843`.
- Tcl opens `/dev/ttyUSB0` at `115200,n,8,1`, no handshake. With CTS grounded it receives the `ASSIST09` banner and `>` prompt.
- The CPU, reset release, E clock, ACIA Rx/Tx clocks, and address bus have already been observed active.
- Power-off continuity: H2 pin 2 (`A0`) to U1 pin 11 (`RS`) is present. U1 pin 11 has no continuity to U1 pin 12 (`VCC`).
- [same-acia.jpg](../images/same-acia.jpg): both AD2 scope channels connected to U1 pin 11 using a shared ground show the same active `RS` waveform. This validates the scope channel and ground arrangement.
- [split-acia.jpg](../images/split-acia.jpg): C1 on U1 pin 11 (`RS`) is active after reset; C2 on U1 pin 6 (`TXDATA`) remains idle high.

## Resolved Cause

The corrected monitor polls ACIA status bit 1 (`TDRE`) before writing transmit data. On the MC6850/68C50, a high `CTS` input inhibits `TDRE`. The data sheet identifies U1 pin 24—not pin 23—as `CTS`; pin 23 is `DCD`. The schematic leaves U1 pin 24 unconnected, and the scope measured it high near the board supply.

This is a hypothesis, not a source-change authorization.

With board power removed, a temporary jumper was installed from U1 pin 24 (`CTS`) to U1 pin 1 (`VSS`/GND). On the next boot, ASSIST09 produced its prompt. The RAM smoke S-record was then loaded and run successfully:

```text
ASSIST09 RAM SMOKE TEST PASSED
ASSIST09
```

This completes the ROM, serial, load, RAM, and execution acceptance gate.

## Permanent Hardware Follow-up

Do not leave CTS floating. Add a defined low level for U1 pin 24, either by retaining a direct CTS-to-ground link for this no-flow-control design or by installing an appropriate pull-down / bringing CTS to a populated flow-control connector. The current P1 pin 2 `CTS` stub is also unconnected; it cannot substitute for a U1 pin 24-to-ground link without a board modification.

Preserve the CTS capture and retain the temporary jumper until the permanent remedy has been installed and repeat-tested.
