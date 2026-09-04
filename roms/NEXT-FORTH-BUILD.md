# Next Build Step: Forth Port

Status: planned after ASSIST09 hardware acceptance.

The existing fig-Forth source is not a build candidate for the M6x09-II-SBC without a board-specific port. Its original target assumes RAM at `$C000-$DFFF`, ROM at `$E000-$FFFF`, and an ACIA at `$9800`; those assumptions conflict with the board [memory map](MEMORY-MAP.md).

## Entry Gate

Start this work only after the ASSIST09 milestone has recorded all of the following:

1. A verified Batronix program operation using [assist09-27c128.bin](assist09-27c128.bin).
2. An ASSIST09 boot banner and `>` prompt on the target board.
3. A successful `ASSIST09 RAM SMOKE TEST PASSED` result through the Tcl terminal.

## Port Process

1. Define the Forth RAM allocation inside the board's `$0000-$7FFF` RAM, excluding the ASSIST09 work page around `$5F00`.
2. Change the Forth ACIA constants from `$9800-$9801` to the board ACIA at `$BE00`.
3. Create a RAM-test build in which Forth code, dictionary, TIB, and stacks all occupy confirmed writable RAM.
4. Load that RAM-test S-record through ASSIST09 and prove terminal input, terminal output, dictionary writes, stack operations, and reset recovery.
5. Relocate the proven Forth ROM code into the available `$C000-$F7FF` ROM region while retaining ASSIST09 at `$F800-$FFFF` and its hardware vectors at `$FFF0-$FFFF`.
6. Build a full 16 KiB `27C128` programmer image with Forth in the lower ROM region and ASSIST09 in the final 2 KiB.
7. Program, verify, boot-test, and record the resulting ROM as a separate Forth milestone under `roms/`.

## Constraints

- Do not overwrite or relocate ASSIST09 until the Forth ROM has a separately proven reset and recovery path.
- Do not use the current Forth output as a programmer image; it targets incompatible memory and serial addresses.
- Do not choose a final Forth RAM range until the ASSIST09 work area and the required Forth dictionary and stack sizes have been measured.
