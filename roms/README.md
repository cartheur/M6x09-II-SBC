# ROM Workflow For Batronix Barlino II 32P

This folder is for ROM images that are worth preserving in git and for documenting the build, test, and burn workflow used with the Batronix Barlino II 32P.

The recommended development rhythm is:

1. Build or update the ROM image on Linux.
2. Test new code in RAM first, typically via serial using the generated S-record.
3. Burn EPROM with the Batronix Barlino II 32P only after the image is stable.
4. Preserve only milestone ROMs in this folder.

## Source Layout

- ASSIST09 [source](../src/assist-09/assist09.asm)
- Source [notes](../src/README.md)
- AS9 assembler [source](../src/assembler)
- AS9 [build file](../src/assembler/Makefile)

## Build ASSIST09 On Linux

From the repository root, build the assembler:

```bash
cd src/assembler
make as9
```

Assemble ASSIST09:

```bash
cd src/assist-09
../assembler/as9 assist09.asm -l c s bin s19 cre now
```

That should produce at least:

- `assist09.lst`
- `assist09.bin`
- `assist09.s19`

The source itself confirms the intended ROM base and ACIA address:

- `ROMBEG EQU $F800`
- `ACIA EQU $BE00`

Those values are in the ASSIST09 [source](../src/assist-09/assist09.asm).

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
