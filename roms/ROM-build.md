# ROM Build And Burn

This README covers the ROM-image side of the workflow: build a stable monitor image on Linux, test in RAM first, then burn EPROM with the GTEK 7228.

The recommended development rhythm is:

1. Build or update ASSIST09 on Linux.
2. Test new code over serial in RAM first.
3. Burn EPROM with the GTEK 7228 only after the image is stable.

## Source Layout

- ASSIST09 source: [src/m6809/assist09.asm](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/m6809/assist09.asm)
- Source notes: [src/m6809/README.md](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/m6809/README.md)
- AS9 assembler source: [src/as9](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/as9)
- AS9 build file: [src/as9/Makefile](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/as9/Makefile)
- Programmer references: [programming/610P.pdf](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/programming/610P.pdf), [programming/ManuSP6100_en.pdf](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/programming/ManuSP6100_en.pdf), [programming/xsp610p-devicelist.pdf](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/programming/xsp610p-devicelist.pdf)

## Build ASSIST09 On Linux

Build the assembler:

```bash
cd src/as9
make as9
```

Assemble ASSIST09:

```bash
cd src/m6809
make assist09
```

That should produce at least:

- `assist09.lst`
- `assist09.bin`
- `assist09.s19`

The source itself confirms the intended ROM base and ACIA address:

- `ROMBEG EQU $F800`
- `ACIA EQU $BE00`

Those values are in [src/m6809/assist09.asm](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/m6809/assist09.asm).

## Use RAM As The Fast Loop

Do not reburn EPROM for every test.

Use ASSIST09 as the stable ROM monitor, then:

1. Assemble your program on Linux.
2. Load the S-record through the serial terminal into RAM.
3. Run and debug from RAM.
4. Promote only stable milestones into EPROM.

This keeps the GTEK 7228 in the outer loop where it belongs.

## Burn EPROM With The GTEK 7228

Use the GTEK only after you already trust the image in RAM. A safe sequence is:

1. Build `assist09.bin` or your combined ROM image.
2. Read the existing EPROM first and save a backup.
3. Check the exact EPROM device type in the GTEK software.
4. Blank-check the replacement EPROM.
5. Program the image.
6. Verify after program.
7. Label the part with image name and date.

If the final target is still the `27C128` shown in the board BOM, make sure the GTEK device selection matches the exact chip family you are actually inserting.

## Suggested Artifact Discipline

For each ROM milestone, keep:

- the source file set used to build it
- the generated `.bin`
- the generated `.s19`
- the programmer project or settings if applicable
- a short note describing what changed

If you later add a combined ASSIST09 + BASIC ROM image, store that as a separate named milestone instead of replacing the plain ASSIST09 build history.

## Practical Strategy

Use ASSIST09 as:

- the reset-time recovery environment
- the serial upload target
- the debugger and memory inspector
- the stepping stone to BASIC with `G D000` if that combined ROM is in use

Use the GTEK 7228 as:

- the preservation tool
- the milestone-publishing tool
- not the inner-loop development tool
