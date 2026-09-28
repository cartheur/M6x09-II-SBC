# ASSIST09 Baseline Output Record

Date: 2026-09-04

Status: the ACIA register-map-corrected candidate has passed full-device readback; board boot and serial acceptance pending.

This record began as the first output of the M6x09-II-SBC ROM workflow. The original baseline exposed an ACIA register-selection and status-polling defect in the emulator, so the current preserved image is an ACIA-corrected candidate rather than the original untested baseline.

The original raw 2 KiB monitor SHA-256 was `15d015d50df6a71fae61c459c2d009f251f08c9bb8c7a3dbd0bf1524cac1d394`. It remains historical evidence only; do not program it for the next board session.

## Inputs

- Monitor [source](../src/assist-09/assist09.asm)
- AS9 [assembler](../src/assembler)
- Image [verifier](../scripts/verify-assist09-image.sh)
- Terminal [smoke test](../src/assist-09/assist09-smoke.asm)

## Host Verification Result

The clean-room AS9 rebuild completed successfully and matched the final 2 KiB of the checked-in `roms/assist09-27c128.bin` programmer image byte for byte after the ACIA register-map repair.

| Check | Result |
| --- | --- |
| Binary size | 2,048 bytes |
| ROM address range | `$F800-$FFFF` |
| Final S-record address | `$FFF0` |
| Reset vector | `$F837` |
| Register-map-corrected 2 KiB monitor SHA-256 | `3904c6277ab60bc03528475e4522bc5c8da37a09458d56687634842894f9c12c` |
| Register-map-corrected 16 KiB programmer image SHA-256 | `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843` |

Run the same check from the repository root with:

```bash
scripts/verify-assist09-image.sh
```

## Hardware Acceptance Gate

The corrected candidate was programmed and passed the complete programmer-readback check below, but board boot and serial acceptance remain pending.

## Earlier Programmer Readback Record

Date: 2026-09-28

The earlier 16 KiB candidate was programmed using the Windows host. A full-device readback was saved as [readback.bin](readback.bin) and compared byte for byte with the then-current programmer image on Linux. That image has since been superseded by the ACIA register-map repair below.

| Check | Result |
| --- | --- |
| Readback size | 16,384 bytes |
| Comparison with then-current programmer image | Identical (byte for byte) |
| Readback SHA-256 | `1d7fdbe412c8e57084b99b981a24c8fbdbc013227a6da04b1810c67054aa2d72` |
| Current register-map-corrected image checksum | `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843` (does not match; expected) |

`readback.txt` was renamed to `readback.bin`; it is raw binary data, not a text log. That filename now holds the later corrected-candidate readback recorded below; the values in this section are retained as historical evidence of the earlier burn.

## Corrected-Candidate Programmer Readback

Date/time: 2026-09-28 19:03

The corrected 16 KiB image was programmed and read back in full as [readback.bin](readback.bin). It matches the current `assist09-27c128.bin` byte for byte.

| Check | Result |
| --- | --- |
| Readback size | 16,384 bytes |
| Readback SHA-256 | `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843` |
| Comparison with current programmer image | Identical (byte for byte) |

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

## ACIA Register-Select Diagnosis

Date: 2026-09-28

The ACIA transmit investigation reached a source-level conclusion. AD2 captures establish that the physical prerequisites are present:

- U1 pin 4 (`TXCLK`) measures 1.8427 MHz.
- U1 pin 3 (`RXCLK`) measures 1.8445 MHz.
- U1 pin 11 (`RS`) is active during CPU bus accesses.
- U1 pin 6 (`TXDATA`) idles high near 4.7 V and produces short digital transmit bursts.

The bursts are far shorter than an 115200-baud character. The local [MC6850 data sheet](../build/datasheets/MC6850.pdf) defines `RS=0` as Control/Status and `RS=1` as Transmit/Receive Data. The board schematic connects CPU A0 directly to U1 pin 11 (`RS`), so `$BE00` selects Control/Status and `$BE01` selects Data.

The previous monitor source used those offsets in reverse: it wrote the intended ACIA reset (`$03`) and configuration (`$51`) to offset 1, and read status/wrote transmit data at offset 0. The observed short TX bursts are consistent with `$03` and `$51` being emitted as data while the ACIA remained in its default divide-by-1 mode.

The next source change swaps those register offsets in `CIDTA`, `COON`, and `CODTAO`:

| Operation | Current offset | Required offset |
| --- | --- | --- |
| Read status | 1 | 0 |
| Read received data | 0 | 1 |
| Write control (`$03`, `$51`) | 1 | 0 |
| Read transmit status | 1 | 0 |
| Write transmit data | 0 | 1 |

The source repair has now been applied, including the matching emulator model and regression tests. It preserves the 2,048-byte monitor size. The new 16 KiB image has SHA-256 `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843`; it passes `make -C emulator test`, `scripts/verify-assist09-image.sh`, and its checksum check. It differed in six bytes from the earlier readback, as expected, and the corrected candidate's full readback now matches it exactly.

Complete and record these remaining observations on the target board:

1. With power removed, install the EPROM, confirming its orientation and exact device type.
2. Start the terminal at `115200`, `n,8,1`, with no handshake; reset the M6x09-II-SBC and record the ASSIST09 banner and `>` prompt.
3. Build the RAM test with `make -C src/assist-09 smoke`.
4. At the monitor prompt, enter `L`, send `src/assist-09/assist09-smoke.s19`, then enter `G 1000`.
5. Record `ASSIST09 RAM SMOKE TEST PASSED` followed by the monitor prompt.

Only after those observations are recorded should a copied ROM image, checksum file, and completed milestone note be committed under `roms/`.

## Post-Repair ACIA State

The corrected EPROM has now been programmed and verified by complete readback, but the terminal still receives no banner or prompt. A misleading initial scope comparison was resolved by a shared-ground, same-pin check: both AD2 channels on U1 pin 11 (`RS`) show the same active waveform ([same-acia.jpg](../images/same-acia.jpg)). Power-off continuity also confirms H2 pin 2 (`A0`) to U1 pin 11, with no continuity from U1 pin 11 to VCC on U1 pin 12.

The subsequent split capture ([split-acia.jpg](../images/split-acia.jpg)) places C1 on U1 pin 11 (`RS`) and C2 on U1 pin 6 (`TXDATA`). After reset, `RS` is active but `TXDATA` remains idle high. The next session must measure U1 pin 23 (`CTS`) relative to ground: high CTS inhibits the ACIA `TDRE` bit and would make the monitor's transmit polling loop wait indefinitely. Do not change source until that measurement is recorded.

## Episode 10 Shadow

This technical record is paired with the Episode 10 companion note in `The Last Cyberneticist`. The pairing makes the episode's claim concrete: reproducibility is demonstrated by a build, an image check, a serial RAM test, and a preserved hardware result.
