# ACIA Register-Select Root Cause

Date: 2026-09-28

Status: source repair implemented, programmed, and full-device-readback verified; board boot and serial acceptance pending.

## Issue

The previously programmed ASSIST09 EPROM was read back successfully and matched its intended 16 KiB image byte for byte, but the Alpha board produced no intelligible serial monitor output. A Gamma-board cross-check was also silent. The ROM contents were therefore reproducible, but not functionally correct for the hardware ACIA mapping.

## Hardware Evidence

The debugging session established the prerequisites before changing source:

- FTDI-powered supply was corrected to about 4.56 V.
- `/RESET` was measured at H2 pin 7. It rises through the board's RC network and settles near 4.5 V after approximately 100–150 ms.
- CPU `E` clock at H2 pin 9 measured 1.8431 MHz.
- Lower address lines A0–A11 showed continuing bus activity after reset release.
- U1 pin 4 (`TXCLK`) measured 1.8427 MHz and U1 pin 3 (`RXCLK`) measured 1.8445 MHz.
- U1 pin 6 (`TXDATA`) showed short digital transmit bursts instead of remaining idle.
- U1 pin 11 (`RS`) was active during CPU accesses.

The clock and TX evidence ruled out a dead CPU, absent ACIA clocks, or a purely terminal-side failure. The short TX bursts were inconsistent with the expected 115200-baud character timing.

## Root Cause

The MC6850/68C50 register-select definition is:

| U1 `RS` level | Register pair |
| --- | --- |
| 0 | Control (write) / Status (read) |
| 1 | Transmit Data (write) / Receive Data (read) |

The board schematic connects CPU A0 directly to U1 pin 11 (`RS`). Consequently:

| CPU address | A0 / `RS` | ACIA function |
| --- | --- | --- |
| `$BE00` | 0 | Control/Status |
| `$BE01` | 1 | Data |

The previous monitor source had those offsets reversed. It wrote the intended master-reset byte `$03` and configuration byte `$51` to `$BE01`, which is the transmit-data register, not the control register. The ACIA consequently remained in its default divide-by-1 state and transmitted those initialization bytes as short raw data bursts. The same reversal made transmit polling read the data register rather than status, and made normal output writes target control rather than transmit data.

This explains all of the otherwise conflicting observations: a valid readback, a live CPU and ACIA clock, visible TX transitions, and no usable `115200,n,8,1` console output.

## Error History And Provenance

This was not a single-source error.

1. Commit `09b68ca` (`Arrange`, 2026-08-18) brought the archived ASSIST09 source into its then-current repository location. Its `CIDTA` and `CODTAO` paths already treated offset 1 as status/control and offset 0 as data, the inverse of the 6850 pinout. Its initialization stores were still at offset 0.
2. Commit `da931d913b926a7f5ef3978d7367b10e5c7b68cf` (`Fix ASSIST09 ACIA initialization and polling`, 2026-09-28) changed the initialization stores from offset 0 to offset 1 while attempting to correct ACIA behavior. Per the project owner, this commit was made by an agent operating under the `cartheur` account. It extended the inherited register-map error to the `$03` reset and `$51` configuration writes, making the ACIA configuration failure complete.
3. Commit `0b656cd` (`Document ACIA register-select diagnosis`, 2026-09-28) records the hardware evidence that surfaced the combined error: valid ROM readback, correct reset and clocks, live ACIA TX, correct ACIA clocks, and active `RS`.

Git metadata alone identifies the account (`cartheur`) but does not distinguish a human from an agent. The agent attribution for `da931d9` is recorded here on the project owner's confirmation.

This history is also a podcast lesson: an agent can make an honest, plausible mistake, especially where a simulator and a hardware data sheet disagree. The developer remains responsible for checking the actual pinout, reviewing changes, and accepting the diagnosis only when source, instruments, and hardware evidence agree.

## Implemented Repair

The register offsets in `src/assist-09/assist09.asm` are now corrected in all three ACIA paths:

| Routine | Previous behavior | Correct behavior |
| --- | --- | --- |
| `CIDTA` | Status at offset 1; received data at offset 0 | Status at offset 0; received data at offset 1 |
| `COON` | `$03` and `$51` written at offset 1 | `$03` and `$51` written at offset 0 |
| `CODTAO` | Poll offset 1; transmit at offset 0 | Poll offset 0; transmit at offset 1 |

The emulator's 6850 model, regression test, and explanatory README were corrected to use the same hardware map.

## New Candidate Image

| Artifact | SHA-256 |
| --- | --- |
| New raw 2 KiB monitor | `3904c6277ab60bc03528475e4522bc5c8da37a09458d56687634842894f9c12c` |
| New 16 KiB programmer image | `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843` |
| Previous programmed EPROM readback | `1d7fdbe412c8e57084b99b981a24c8fbdbc013227a6da04b1810c67054aa2d72` |

The new programmer image differs from the old verified readback in six bytes, as expected for this source repair.

## Host Verification

The repaired candidate passes:

```bash
make -C emulator test
scripts/verify-assist09-image.sh
(cd roms && sha256sum -c assist09-27c128.bin.sha256)
```

## Programming And Readback Result

At 19:03 on 2026-09-28, the repaired candidate was programmed and a complete 16,384-byte device readback was saved as `roms/readback.bin`. Its SHA-256 is `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843`, identical byte for byte to `roms/assist09-27c128.bin`. This verifies the burn; it does not yet verify execution on the board.

## Next Hardware Step

Install the verified EPROM if it is not already installed, then reset the board and test for the ASSIST09 banner and `>` prompt at `115200,n,8,1`, no handshake. A correct TX waveform should have a bit time of approximately 8.68 us.
