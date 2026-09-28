# M6x09-II-SBC Emulator

This directory contains the first C-based host model for the M6x09-II-SBC. Its purpose is to establish firmware behaviour independently of the physical board; it does not replace electrical bring-up.

## Scope

The first model should provide:

- an MC6809 CPU core behind a small adapter interface (not implemented yet);
- 32 KiB RAM at `$0000-$7FFF`;
- an unmapped region at `$8000-$9FFF`;
- a minimal 6850-compatible ACIA at `$BE00`;
- a 16 KiB ROM at `$C000-$FFFF` loaded from `roms/assist09-27c128.bin`;
- traceable reads of `$FFFE-$FFFF` and reset-vector loading into the CPU program counter.

The current implementation provides the memory map, ROM loader, ACIA state model, reset-vector trace, and a deliberately small 6809 instruction subset. That subset executes ASSIST09's vector-initialization routine through its return to `$F83D`, plus its ACIA setup attempt and transmit-polling routine. The 6850 model follows the schematic's direct `A0 -> RS` connection: `$BE00` is data and `$BE01` is control/status. Unsupported instructions stop explicitly. It is not yet a complete 6809 implementation.

The emulator must make ACIA status flags controllable by tests. This is necessary to reproduce and correct the current ASSIST09 polling faults: receive must test `RDRF` (bit 0), while transmit must test `TDRE` (bit 1).

It will not validate EPROM programming or orientation, power rails, reset circuitry, clocks, FTDI voltage levels, or PCB wiring. Those remain board-debugging work.

## Proposed Layout

```text
emulator/
├── Makefile
├── README.md
├── src/                 # host-side C implementation
├── tests/               # C tests and 6809 assembly fixtures
└── build/               # ignored executables, traces, and generated fixtures
```

Generated files must stay in `build/`; only source, tests, documentation, and deliberately preserved reference traces belong in git.

## Build Contract

The emulator is a native host program and should build with a C compiler:

```bash
make -C emulator
```

Firmware is assembled separately, then loaded by the emulator:

```bash
make -C src/assist-09 programmer-image
make -C emulator run ROM=../roms/assist09-27c128.bin
```

Run the native regression tests with:

```bash
make -C emulator test
```

The repository's checked-in `src/assembler/as9` executable is a legacy 32-bit binary and does not run on the current host. Before relying on generated assembly fixtures, make its build reproducible with a GNU89-compatible compiler mode.

## First Milestones

1. Load the preserved 16 KiB ASSIST09 programmer image and assert its size. **Done.**
2. Reset the CPU and assert reads from `$FFFE-$FFFF` resolve to `$F837`. **Done.**
3. Extend the 6809 core from the reset-vector and ACIA-routine subsets into the SWI monitor path.
4. Extend the minimal ACIA register model and capture monitor output.
5. Write regression tests for `RDRF` and `TDRE` behaviour before changing ASSIST09.
6. Assemble and execute the RAM smoke-test fixture at `$1000`.
7. Preserve a concise reset/console trace to compare against the physical board session.

## Podcast Value

The emulator provides a repeatable evidence trail: it can show the reset vector, firmware decision points, ACIA register state, and expected output byte by byte. The scope and terminal captures from the actual SBC can then confirm where physical behaviour diverges from that model.
