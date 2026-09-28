# Subscriber Notes: The Board That Was Silent

Date: 2026-09-28

## The Short Version

The new ASSIST09 EPROM image was not the cause of the silence. A full 16 KiB readback from the Windows programmer matched the Linux-built image byte for byte. Yet neither the Alpha board nor the Gamma cross-board check produced a serial character.

The investigation then moved outward from the ROM: serial host, supply, reset, clock, CPU bus, and finally the ACIA transmit path. The useful lesson is not that every first measurement was right; it is that each measurement narrowed the next question.

## Storyboard

### 1. The ROM Was Real

The programmer readback, preserved as `roms/readback.bin`, is identical to `roms/assist09-27c128.bin`. The image is 16,384 bytes and has SHA-256 `1d7fdbe412c8e57084b99b981a24c8fbdbc013227a6da04b1810c67054aa2d72`. The binary file is the evidence; it merely looked corrupt when it was initially opened as text.

### 2. Silence Was Not a Terminal Rendering Problem

The FTDI device was visible as `/dev/ttyUSB0`, and the Tcl terminal opened it at `115200,n,8,1`, no handshake. A direct 45-second capture, including a transmitted carriage return and a board reset, received zero bytes. The absence of an `ASSIST09` prompt was therefore physical evidence, not a GUI display issue.

### 3. The Analyzer Found Life on the Bus

The first Logic Analyzer views turned individual lines into an address-bus question. Once A0–A11 were grouped as a hexadecimal bus, the capture showed repeated changing values rather than a frozen machine.

Suggested figures:

- [logic-analyzer.jpg](../images/logic-analyzer.jpg) — the initial raw bus activity.
- [logic-analyzer-run.jpg](../images/logic-analyzer-run.jpg) — a running capture.
- [busses.jpg](../images/busses.jpg) — the decisive `A[11:0]` bus grouping.

Technical appendix:

- [logic-export.csv](../images/logic-export.csv) — raw 100 MHz bus capture.

### 4. Reset Looked Broken Until the Measurement Was Questioned

The short digital capture made `/RESET` appear to chatter. This was a productive false lead: it showed that a 40.96 us digital window cannot describe a deliberately slow analog reset ramp. The early captures belong in the story as evidence of the method being corrected, not as proof that reset was faulty.

Suggested figures and data:

- [RESET.jpg](../images/RESET.jpg) — the first misleading digital-reset configuration.
- [reset-pressed.jpg](../images/reset-pressed.jpg) and [reset-released.jpg](../images/reset-released.jpg) — early scope attempts, explicitly labelled inconclusive.
- [ch2-trigger.jpg](../images/ch2-trigger.jpg) — the clue: the reset voltage was still only around 2 V in a short view.
- [D015-rise.csv](../images/D015-rise.csv) and [long-reset.csv](../images/long-reset.csv) — raw captures that explain the misleading threshold transitions.

### 5. The Long Scope Capture Resolved Reset

The AD2 scope, connected to H2 pin 7 (`/RESET`) with its negative input at board ground, was given a long enough timebase. It showed the expected behavior: Reset stays low while the button is held, then rises smoothly through the RC network and settles near 4.5 V after approximately 100–150 ms.

That changed the conclusion. Reset was not chattering; the logic analyzer had been watching a slow analog signal cross a digital threshold. The corresponding CPU E-clock measured 1.8431 MHz at H2 pin 9, as expected from the 7.3728 MHz oscillator. The installed HD6309 remains compatible with the 6809 assumptions used by ASSIST09.

Primary figure:

- [ch2-trigger-02.jpg](../images/ch2-trigger-02.jpg) — the reset-release resolution; this is the key subscriber image.

### 6. Where the Investigation Stands

The TX probe changed the story again. U1 pin 6 was not silent: it emitted short digital bursts. The receive and transmit clock pins measured approximately 1.843 MHz, so the ACIA clock source was correct. The bursts, however, were far too short to be 115200-baud characters.

The answer was in the relationship between source and pinout. On the 6850, `RS=0` selects Control/Status and `RS=1` selects Data. The board carries CPU A0 directly to U1 pin 11 (`RS`), so `$BE00` is Control/Status and `$BE01` is Data. The monitor source had those offsets reversed. Its two intended initialization bytes, `$03` and `$51`, were being transmitted as raw data while the ACIA remained in its default divide-by-1 mode.

Suggested figures:

- [acia-burst.jpg](../images/acia-burst.jpg) and [acia-burst-narrow.jpg](../images/acia-burst-narrow.jpg) — TX was alive, but its timing was wrong.
- [acia-crystal-01.jpg](../images/acia-crystal-01.jpg) and [acia-crystal-02.jpg](../images/acia-crystal-02.jpg) — both ACIA clocks were correct.
- [RS.jpg](../images/RS.jpg) — register-select activity connecting the bus diagnosis to the firmware mistake.

The next chapter is a deliberately small source repair: Control/Status moves to offset 0; Data moves to offset 1. The new ROM must earn its place through a rebuild, programmer readback, and a fresh hardware boot test.

## Preservation Decision

Delete **none** of the current figures or CSV captures yet. The supposedly unsuccessful captures document the path from an apparent reset failure to the correct RC-reset interpretation. They are useful primary evidence for a subscriber note and a later technical appendix.

For a compact published version, lead with `busses.jpg` and `ch2-trigger-02.jpg`; keep the other JPEGs and all CSV files as linked or downloadable appendix material. A later editorial pass can omit intermediate images from the public page without destroying the underlying record.
