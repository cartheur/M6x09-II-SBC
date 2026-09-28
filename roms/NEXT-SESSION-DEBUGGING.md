# Next Session: CTS And ACIA Transmit Enable

Date: 2026-09-28

Status: stop after the post-repair `RS`/`TXDATA` split capture. The repaired EPROM is programmed and fully read back; terminal acceptance is still pending. Do not change source before completing this session.

## Established Evidence

- `roms/readback.bin` is byte-for-byte identical to the corrected `roms/assist09-27c128.bin`: SHA-256 `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843`.
- Tcl opens `/dev/ttyUSB0` at `115200,n,8,1`, no handshake, but receives neither the ASSIST09 banner nor `>`.
- The CPU, reset release, E clock, ACIA Rx/Tx clocks, and address bus have already been observed active.
- Power-off continuity: H2 pin 2 (`A0`) to U1 pin 11 (`RS`) is present. U1 pin 11 has no continuity to U1 pin 12 (`VCC`).
- [same-acia.jpg](../images/same-acia.jpg): both AD2 scope channels connected to U1 pin 11 using a shared ground show the same active `RS` waveform. This validates the scope channel and ground arrangement.
- [split-acia.jpg](../images/split-acia.jpg): C1 on U1 pin 11 (`RS`) is active after reset; C2 on U1 pin 6 (`TXDATA`) remains idle high.

## Working Hypothesis

The corrected monitor polls ACIA status bit 1 (`TDRE`) before writing transmit data. On the MC6850/68C50, a high `CTS` input inhibits `TDRE`. If U1 pin 23 (`CTS`) is high, the observed active register-select traffic and idle TX output are expected.

This is a hypothesis, not a source-change authorization.

## Exact First Measurement

Make this connection with board power removed, then power the board through its normal FTDI connection:

| AD2 wire | Board connection |
| --- | --- |
| Scope C1+ | U1 pin 23 (`CTS`) |
| Scope C1- | board/FTDI GND |
| Scope C2+ (optional) | U1 pin 5 (`RTS`) |
| Scope C2- | same board/FTDI GND node |

Only one board GND pin is needed: join C1- and C2- at a shared ground node before connecting that node to board GND.

In WaveForms Scope, enable DC coupling and use 2 V/div. Select a 1 ms/div timebase and Repeated acquisition. Trigger Auto is sufficient because this is a DC-level measurement.

Record the measured CTS level after reset:

- **Near 0 V:** CTS is clear; continue by checking ACIA bus read/write and chip-select timing before considering source behavior.
- **Near 4.5 V:** CTS is blocking TDRE and is the immediate cause of the idle TX line. Trace the CTS connection to the FTDI/control wiring and decide the hardware remedy before touching firmware.

## Session Boundary

Preserve the CTS capture and its measured voltage. Do not erase, reprogram, or replace the verified EPROM, and do not make a new source commit during this measurement session. The next decision follows the observed CTS level.
