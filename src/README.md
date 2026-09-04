## M6809 Monitor And Language Sources

This folder contains the checked-in 6809 source tree used as the software starting point for the M6x09-II SBC.

## Contents

- [assist09.asm](assist-09/assist09.asm)
  - Motorola ASSIST09 monitor, adapted for the 6850 ACIA on this board
- [forth09.asm](forth-09/forth09.asm)
  - 6809 fig-Forth source
- [ASSIST09 Makefile](assist-09/Makefile)
  - the validated ROM baseline and RAM smoke-test build targets
- [Forth Makefile](forth-09/Makefile)
  - a separate, provisional Forth build

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

Those values are defined directly in [assist09.asm](assist-09/assist09.asm).

## Build

Build the assembler first:

```bash
make -C src/assembler as9
```

Then build and verify ASSIST09:

```bash
make -C src/assist-09
scripts/verify-assist09-image.sh
```

Build the terminal smoke-test S-record with:

```bash
make -C src/assist-09 smoke
```

Expected ASSIST09 artifacts:

- `assist09.bin`
- `assist09.lst`
- `assist09.s19`

Forth is not part of the ROM baseline: it currently assumes a different `$E000-$FFFF` ROM and `$C000-$DFFF` RAM map. Build it only for separate investigation:

```bash
make -C src/forth-09
```

## Development Workflow

The intended workflow for this repo is:

1. Keep ASSIST09 in ROM.
2. Bring the board up over the FTDI-style serial header.
3. Assemble code on Linux.
4. Load S-records into RAM over serial.
5. Burn EPROM with the Batronix Barlino II 32P only when the image is stable.

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

- [terminal/README.md](../terminal/README.md)
- [roms/README.md](../roms/README.md)

## References

- AS9 assembler notes: [as9.md](as9.md)
- AS9 source: [src/assembler](assembler/)
- Earlier project background: http://land-boards.com/blwiki/index.php?title=RetroComputers
- Related salvage repo: https://github.com/cartheur/SalvagedRetro/tree/master/6809
