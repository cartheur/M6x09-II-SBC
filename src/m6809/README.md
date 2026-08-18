## M6809 Monitor And Language Sources

This folder contains the checked-in 6809 source tree used as the software starting point for the M6x09-II SBC.

## Contents

- [assist09.asm](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/m6809/assist09.asm)
  - Motorola ASSIST09 monitor, adapted for the 6850 ACIA on this board
- [forth09.asm](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/m6809/forth09.asm)
  - 6809 fig-Forth source
- [Makefile](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/m6809/Makefile)
  - convenience targets for building ASSIST09 and Forth artifacts

## Why ASSIST09 Matters Here

ASSIST09 is the best ROM starting point for this machine.

It gives the board:

- a reliable serial monitor at reset
- memory display and modification
- register display and modification
- breakpoints and program control
- Motorola S-record `LOAD` and `PUNCH` support

That makes it ideal as the ROM-resident bootstrap while normal development happens by loading code into RAM over serial.

## Board-Specific Notes

This copy of ASSIST09 was adapted to use the board's 6850 ACIA. In the source:

- `ROMBEG EQU $F800`
- `ACIA EQU $BE00`

Those values are defined directly in [assist09.asm](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/m6809/assist09.asm).

## Build

Build the assembler first:

```bash
cd src/as9
make as9
```

Then build ASSIST09:

```bash
cd ../m6809
make assist09
```

Or just use the default target:

```bash
make
```

Expected ASSIST09 artifacts:

- `assist09.bin`
- `assist09.lst`
- `assist09.s19`

You can also build Forth with:

```bash
make forth09
```

## Development Workflow

The intended workflow for this repo is:

1. Keep ASSIST09 in ROM.
2. Bring the board up over the FTDI-style serial header.
3. Assemble code on Linux.
4. Load S-records into RAM over serial.
5. Burn EPROM with the GTEK 7228 only when the image is stable.

That keeps ROM as the recovery layer and RAM as the fast iteration layer.

## BASIC Context

The broader project notes describe a combined ASSIST09 + BASIC ROM arrangement where BASIC is reached with:

```text
G D000
```

That is still a useful target layout, but ASSIST09 should remain the canonical first-stage monitor because it gives the cleanest bring-up and debug path.

## Historical Notes

These sources were carried forward from earlier salvage work around a simple 6809 SBC software stack:

- Microsoft Extended BASIC, reduced for serial-only use
- ASSIST09 for monitor and debug work
- fig-Forth as another language/runtime candidate

The older background writeup was valuable historically, but the practical working guidance now lives in:

- [terminal/README.md](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/terminal/README.md)
- [programming/README.md](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/programming/README.md)

## References

- AS9 assembler notes: [src/as9.md](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/as9.md)
- AS9 source: [src/as9](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/src/as9)
- Earlier project background: http://land-boards.com/blwiki/index.php?title=RetroComputers
- Related salvage repo: https://github.com/cartheur/SalvagedRetro/tree/master/6809
