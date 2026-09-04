# ASSIST09 Baseline Output Record

Date: 2026-09-04

Status: host-verified; hardware acceptance pending.

This record is the first output of the M6x09-II-SBC ROM workflow. It establishes ASSIST09 as the reproducible monitor baseline before Forth, BASIC, or combined-ROM work is considered.

## Inputs

- Monitor [source](../src/assist-09/assist09.asm)
- AS9 [assembler](../src/assembler)
- Image [verifier](../scripts/verify-assist09-image.sh)
- Terminal [smoke test](../src/assist-09/assist09-smoke.asm)

## Host Verification Result

The clean-room AS9 rebuild completed successfully and matched the checked-in monitor image byte for byte.

| Check | Result |
| --- | --- |
| Binary size | 2,048 bytes |
| ROM address range | `$F800-$FFFF` |
| Final S-record address | `$FFF0` |
| Reset vector | `$F837` |
| SHA-256 | `15d015d50df6a71fae61c459c2d009f251f08c9bb8c7a3dbd0bf1524cac1d394` |

Run the same check from the repository root with:

```bash
scripts/verify-assist09-image.sh
```

## Hardware Acceptance Gate

The image is not yet a hardware-verified ROM milestone. Complete and record these observations on the target board:

1. Confirm the exact EPROM part and its placement for the 2 KiB `$F800-$FFFF` ASSIST09 image before programming. A 16 KiB `27C128` may require a padded image or a specific device offset, depending on board address decoding.
2. Read and save a backup of the existing EPROM, then blank-check the replacement if applicable.
3. Program the confirmed image with the Batronix Barlino II 32P and complete its verify operation.
4. Reset the M6x09-II-SBC and record the ASSIST09 banner and `>` prompt in the Tcl terminal.
5. Build the RAM test with `make -C src/assist-09 smoke`.
6. At the monitor prompt, enter `L`, send `src/assist-09/assist09-smoke.s19`, then enter `G 1000`.
7. Record `ASSIST09 RAM SMOKE TEST PASSED` followed by the monitor prompt.

Only after those observations are recorded should a copied ROM image, checksum file, and completed milestone note be committed under `roms/`.

## Episode 10 Shadow

This technical record is paired with the Episode 10 companion note in `The Last Cyberneticist`. The pairing makes the episode's claim concrete: reproducibility is demonstrated by a build, an image check, a serial RAM test, and a preserved hardware result.
