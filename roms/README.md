# ROM Workflow For Batronix Barlino II 32P

This folder is for ROM images that are worth preserving in git and for documenting the build, test, and burn workflow used with the Batronix Barlino II 32P.

The recommended development rhythm is:

1. Build and verify the ASSIST09 ROM baseline on Linux.
2. Prove the serial loader with the ASSIST09 RAM smoke test.
3. Burn EPROM with the Batronix Barlino II 32P only after both gates pass.
4. Preserve the verified milestone ROM and test record in this folder.

## Source Layout

- ASSIST09 [source](../src/assist-09/assist09.asm)
- Source [notes](../src/README.md)
- AS9 assembler [source](../src/assembler)
- AS9 [build file](../src/assembler/Makefile)

## Build ASSIST09 On Linux

From the repository root, build the assembler and ROM image:

```bash
make -C src/assembler as9
make -C src/assist-09
```

Verify a clean-room rebuild before programming hardware:

```bash
scripts/verify-assist09-image.sh
```

That should produce at least:

- `assist09.lst`
- `assist09.bin`
- `assist09.s19`

The source itself confirms the intended ROM base and ACIA address:

- `ROMBEG EQU $F800`
- `ACIA EQU $BE00`

Those values are in the ASSIST09 [source](../src/assist-09/assist09.asm).

The verifier requires all of the following:

- a 2,048-byte image
- S-record data beginning at `$F800` and ending at `$FFFF`
- reset vector `$F837`
- a byte-for-byte match to the checked-in `assist09.bin` baseline

fig-Forth is intentionally outside this workflow until its `$E000-$FFFF` ROM and `$C000-$DFFF` RAM assumptions have been adapted to this board.

## Prove The Terminal Workflow

Build the RAM smoke-test S-record:

```bash
make -C src/assist-09 smoke
```

With a known-good ASSIST09 EPROM installed and the terminal connected, use this acceptance sequence:

1. Start the terminal as described in [terminal/README.md](../terminal/README.md), reset the board, and record the `ASSIST09` banner and `>` prompt.
2. At the prompt, enter `L` and press Enter.
3. Use `File -> Send...` to transmit `src/assist-09/assist09-smoke.s19`.
4. Wait for the prompt to return without an error, then enter `G 1000`.
5. Record the exact output: `ASSIST09 RAM SMOKE TEST PASSED`, followed by the monitor prompt.

Do not label a ROM image stable until this sequence has been recorded for the specific board, EPROM, serial adapter, and terminal settings used.

## Use RAM As The Fast Loop

Do not reburn EPROM for every test.

Use ASSIST09 as the stable ROM monitor, then:

1. Assemble your program on Linux.
2. Load the S-record through the serial terminal into RAM.
3. Run and debug from RAM.
4. Promote only stable milestones into EPROM.

This keeps the programmer in the outer loop where it belongs.

## Burn EPROM With The Batronix Barlino II 32P

Use the programmer only after you already trust the image in RAM. A safe sequence is:

1. Build `assist09.bin` or your combined ROM image.
2. Read the existing EPROM first and save a backup.
3. Select the exact EPROM device type in the Batronix software.
4. Blank-check the replacement EPROM.
5. Program the image.
6. Verify after programming.
7. Label the part with image name and date.

If the target is still a `27C128` or similar device, make sure the Batronix device selection matches the exact chip family you are actually inserting.

## What Belongs Here

Commit a ROM image here only when it is a real milestone, for example:

- the first known-good ASSIST09 boot image
- a verified ASSIST09 + BASIC combined image
- an image actually programmed into EPROM with the Batronix Barlino II 32P
- an image tied to a documented hardware or software milestone

## What Does Not Belong Here

Do not commit routine build outputs from `src/assist-09` here.

Normal development artifacts should stay ignored:

- `*.bin`
- `*.lst`
- `*.s19`
- `*.sym`
- `*.crf`

Those files are disposable unless they represent a milestone image.

## Which Build Output Is For Burning

For the burn step, the file that belongs here is the final ROM image binary produced by the build, for example:

- `src/assist-09/assist09.bin`

That `.bin` file is the EPROM payload to preserve, checksum, and program.

The other build outputs have different roles:

- `*.s19`: useful for serial or RAM loading through ASSIST09
- `*.lst`: listing file for inspection and debug
- `*.sym`: symbol output from the build
- `*.crf`: cross-reference output from the build

Only the final ROM image `.bin` should normally be copied into `roms/` for the burn path.

## What To Commit With A Milestone ROM

For each preserved ROM, include:

- the ROM image itself
- a checksum file such as `.sha256`
- a short Markdown note describing what it is
- the build date
- the source inputs used to create it
- whether it was tested in RAM before burn
- whether it was programmed into EPROM with the Batronix Barlino II 32P

## Suggested Naming

Use names that carry both purpose and date, for example:

- `assist09-2026-09-04.bin`
- `assist09-basic-combined-2026-09-04.bin`
- `assist09-2026-09-04.bin.sha256`
- `assist09-2026-09-04.md`

## Practical Strategy

Use ASSIST09 as:

- the reset-time recovery environment
- the serial upload target
- the debugger and memory inspector
- the stepping stone to BASIC with `G D000` if that combined ROM is in use

Use the Batronix Barlino II 32P as:

- the preservation tool
- the milestone-publishing tool
- not the inner-loop development tool

## Recommended Workflow

1. Build in `src/assist-09`.
2. Test over serial in RAM, typically using the generated `.s19` file.
3. Program EPROM with the Batronix Barlino II 32P only after the image is trusted.
4. Copy the final preserved `.bin` ROM image into `roms/`.
5. Add the checksum and short milestone note.

This keeps the main source tree clean while still preserving important ROM history.
