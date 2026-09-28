# ASSIST09 Baseline Output Record

Date: 2026-09-04

Status: superseded by a host-verified ACIA-corrected candidate; hardware acceptance pending.

This record began as the first output of the M6x09-II-SBC ROM workflow. The original baseline exposed an ACIA register-selection and status-polling defect in the emulator, so the current preserved image is an ACIA-corrected candidate rather than the original untested baseline.

The original raw 2 KiB monitor SHA-256 was `15d015d50df6a71fae61c459c2d009f251f08c9bb8c7a3dbd0bf1524cac1d394`. It remains historical evidence only; do not program it for the next board session.

## Inputs

- Monitor [source](../src/assist-09/assist09.asm)
- AS9 [assembler](../src/assembler)
- Image [verifier](../scripts/verify-assist09-image.sh)
- Terminal [smoke test](../src/assist-09/assist09-smoke.asm)

## Host Verification Result

The clean-room AS9 rebuild completed successfully and matched the final 2 KiB of the checked-in `roms/assist09-27c128.bin` programmer image byte for byte.

| Check | Result |
| --- | --- |
| Binary size | 2,048 bytes |
| ROM address range | `$F800-$FFFF` |
| Final S-record address | `$FFF0` |
| Reset vector | `$F837` |
| Corrected 2 KiB monitor SHA-256 | `6175249f8ea71f8bc4e3c0e92745f4b1a9f8941e078106bf9b196dff904f7592` |
| Corrected 16 KiB programmer image SHA-256 | `1d7fdbe412c8e57084b99b981a24c8fbdbc013227a6da04b1810c67054aa2d72` |

Run the same check from the repository root with:

```bash
scripts/verify-assist09-image.sh
```

## Hardware Acceptance Gate

The corrected candidate has passed the programmer readback check, but board boot and serial acceptance remain pending.

## Programmer Readback Record

Date: 2026-09-28

The 16 KiB corrected candidate was programmed using the Windows host. A full-device readback was saved as [readback.bin](readback.bin) and compared byte for byte with `assist09-27c128.bin` on Linux.

| Check | Result |
| --- | --- |
| Readback size | 16,384 bytes |
| Comparison with `assist09-27c128.bin` | Identical (byte for byte) |
| Readback SHA-256 | `1d7fdbe412c8e57084b99b981a24c8fbdbc013227a6da04b1810c67054aa2d72` |
| Expected image checksum | Matched |

`readback.txt` was renamed to `readback.bin`; it is raw binary data, not a text log.

## First Boot Attempt

Date: 2026-09-28

With the EPROM image verified by full-device readback, the board was reset with the terminal configured for `115200,n,8,1`, no handshake. No `ASSIST09` banner or `>` prompt was received. A separate direct 45-second FTDI capture, including a transmitted carriage return and a board reset during the capture, received zero bytes. This rules out the Tcl terminal display as the immediate cause. Hardware acceptance therefore remains pending.

## Reset And CPU-Timing Milestone

Date: 2026-09-28

The reset and CPU prerequisites for serial output have now been demonstrated with a Digilent Analog Discovery 2 (AD2). They do not yet establish ACIA output or a successful monitor boot.

| Check | Observation | Result |
| --- | --- | --- |
| Supply after FTDI jumper correction | About 4.56 V at the FTDI-powered board | Present, but with little 5 V margin |
| `/RESET` release | AD2 Scope on H2 pin 7 (`/RESET`) shows a low level while the button is held, then a smooth RC rise to about 4.5 V over roughly 100–150 ms | Passed |
| CPU `E` clock | 1.8431 MHz measured at H2 pin 9 (`E`) | Passed; matches the 7.3728 MHz oscillator / divide-by-four design |
| Lower address bus | AD2 Logic Analyzer capture on H2 pins A0–A11 shows repeated changing values while reset is released | CPU bus is active |

The reset release is intentionally slow because of the board's reset RC network. The short 40.96 us logic-analyzer capture initially appeared to show reset chatter; the longer scope capture ([ch2-trigger-02.jpg](../images/ch2-trigger-02.jpg)) shows that this was the analyzer sampling the slow analog threshold crossing, not a reset oscillation. Consequently, a reset-vector fetch cannot be inferred from the short capture alone.

### AD2 Diagnostic Procedure Used

1. Connect AD2 ground to board ground.
2. Scope H2 pin 7 (`/RESET`) with the AD2 differential scope input (`2+` to `/RESET`, `2-` to ground); use a long timebase to capture both pressing and releasing reset.
3. Confirm the released reset level reaches approximately 4.5 V and remains there.
4. Scope H2 pin 9 (`E`) and confirm approximately 1.8432 MHz.
5. Connect AD2 digital inputs to H2 A0–A11, group them as a hexadecimal `A[11:0]` bus, and confirm ongoing address activity after reset is released.
6. Do not treat an AD2 digital rising-edge trigger as a clean reset-release event while the RC ramp is crossing its input threshold.

Next diagnostic: probe ACIA TX at U1 pin 6 / P1 pin 5 with the AD2 Scope after the reset-release delay. It should idle high and show 115200-baud activity while ASSIST09 emits its banner. If it remains idle, investigate the ACIA supply, chip select/control signals, and the U1-to-P1 TX path before revisiting terminal software.

Complete and record these remaining observations on the target board:

1. With power removed, install the EPROM, confirming its orientation and exact device type.
2. Start the terminal at `115200`, `n,8,1`, with no handshake; reset the M6x09-II-SBC and record the ASSIST09 banner and `>` prompt.
3. Build the RAM test with `make -C src/assist-09 smoke`.
4. At the monitor prompt, enter `L`, send `src/assist-09/assist09-smoke.s19`, then enter `G 1000`.
5. Record `ASSIST09 RAM SMOKE TEST PASSED` followed by the monitor prompt.

Only after those observations are recorded should a copied ROM image, checksum file, and completed milestone note be committed under `roms/`.

## Episode 10 Shadow

This technical record is paired with the Episode 10 companion note in `The Last Cyberneticist`. The pairing makes the episode's claim concrete: reproducibility is demonstrated by a build, an image check, a serial RAM test, and a preserved hardware result.
