# ASSIST09 Hardware Acceptance: CTS And ACIA Transmit Enable

Date: 2026-10-13

Status: superseded. The temporary CTS-to-ground jumper produced a successful smoke test, but subsequent EasyEDA PCB evidence shows CTS is intentionally tied to RTS. Do not add a CTS pull-down or retain the jumper. The next candidate changes the ACIA control byte to drive RTS low.

## Established Evidence

- `roms/readback.bin` is byte-for-byte identical to the corrected `roms/assist09-27c128.bin`: SHA-256 `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843`.
- Tcl opens `/dev/ttyUSB0` at `115200,n,8,1`, no handshake. With a temporary CTS-to-ground jumper it received the `ASSIST09` banner and `>` prompt.
- The CPU, reset release, E clock, ACIA Rx/Tx clocks, and address bus have already been observed active.
- Power-off continuity: H2 pin 2 (`A0`) to U1 pin 11 (`RS`) is present. U1 pin 11 has no continuity to U1 pin 12 (`VCC`).
- [same-acia.jpg](../images/same-acia.jpg): both AD2 scope channels connected to U1 pin 11 using a shared ground show the same active `RS` waveform. This validates the scope channel and ground arrangement.
- [split-acia.jpg](../images/split-acia.jpg): C1 on U1 pin 11 (`RS`) is active after reset; C2 on U1 pin 6 (`TXDATA`) remains idle high.

## Resolved Cause

The corrected monitor polls ACIA status bit 1 (`TDRE`) before writing transmit data. On the MC6850/68C50, a high `CTS` input inhibits `TDRE`. The data sheet identifies U1 pin 24—not pin 23—as `CTS`; pin 23 is `DCD`. The EasyEDA PCB capture [CTS-RTS-PCB.jpg](../images/CTS-RTS-PCB.jpg) identifies pad 24 as net `U1_5`, tying CTS to the ACIA RTS output.

This is a hypothesis, not a source-change authorization.

With board power removed, a temporary jumper was installed from U1 pin 24 (`CTS`) to U1 pin 1 (`VSS`/GND). On the next boot, ASSIST09 produced its prompt. The RAM smoke S-record was then loaded and run successfully:

```text
ASSIST09 RAM SMOKE TEST PASSED
ASSIST09
```

This proved the ROM, serial, load, RAM, and execution path but not the final RTS/CTS configuration. The jumper was removed after the EasyEDA evidence showed it would override the RTS output.

## Firmware Follow-up

The prior `$51` control byte has transmitter-control bits `CR6=1, CR5=0`, which drive RTS high. The local CTS↔RTS link therefore inhibits `TDRE`. The new candidate uses `$11`, preserving 8N1 and the divide-by-16 rate while selecting `CR6=0, CR5=0`: RTS low and transmitter interrupt disabled.

Program and full-readback verify the new candidate, with no CTS jumper installed. Then repeat boot and the RAM smoke test.

Preserve the CTS capture and retain the temporary jumper until the permanent remedy has been installed and repeat-tested.
