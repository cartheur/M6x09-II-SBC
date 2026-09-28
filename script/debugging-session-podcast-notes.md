# M6x09-II-SBC Debugging Session Notes

Use this document as the live technical log and narrative outline for the future podcast episode. Record an observation before interpreting it, retain terminal and scope captures, and distinguish confirmed facts from hypotheses.

## Starting State

- The FTDI adapter and host terminal passed a TX-to-RX loopback test at `19200,n,8,1`, no handshake.
- The board power LED lights, but the SBC has not produced an ASSIST09 banner or `>` prompt.
- `roms/assist09-27c128.bin` is a valid 16 KiB image. Its SHA-256 file verifies, and its final 2 KiB contains the ASSIST09 monitor at `$F800-$FFFF` with reset vector `$F837`.
- The checked-in assembler executable is a 32-bit 2004 binary which fails on this host with `Bad system call`. Rebuilding its source as GNU89 produced a working 64-bit assembler and an exact byte-for-byte match to the ASSIST09 portion of the preserved ROM image.

## Confirmed Software Faults

The ASSIST09 ACIA polling code is not consistent with the checked-in MC6850 data sheet:

- `CIDTA` at `src/assist-09/assist09.asm` reads ACIA status and shifts it twice. Its carry test therefore observes status bit 1 (`TDRE`, transmit-data-register empty), while the comment says it is testing receive data.
- `CODTAO` tests status bit 0 (`RDRF`, receive-data-register full) while its comment says it is waiting for transmit readiness. The source itself marks this `FIXME`.

These errors can prevent reliable console input and output even when the board is electrically healthy. Correct and test the polling masks before treating the serial link as proven end to end.

## High-Value Serial Hypothesis

The schematic connects the ACIA `RxC` and `TxC` pins to the 6809 `E` clock. With the documented 7.3728 MHz oscillator, `E` is expected to be 1.8432 MHz. ASSIST09 initializes the ACIA with `$51`; its low control bits select the MC6850 divide-by-16 mode. If those schematic connections and clock assumptions are correct, the resulting serial rate is **115200 baud**, not the repository's historical 19200 baud setting.

Test `115200,n,8,1`, no handshake, before changing board wiring. Confirm the clock frequencies with a scope before declaring this conclusion final.

## Session Order

1. Photograph chip markings, orientation, FTDI wiring, jumper H1 state, and the EPROM programmer's selected device and verification result.
2. Save a terminal recording and test the board at 115200 with no handshake. Retain a separate capture for the existing 19200 setting.
3. With power removed, continuity-check FTDI `TXO -> U1 pin 2`, FTDI `RXI -> U1 pin 6`, and common ground. Confirm no unintended dual power source.
4. Measure VCC at the CPU, ROM, RAM, and ACIA. Then observe reset release, the 7.3728 MHz oscillator, CPU `E`, and ACIA `RxC`/`TxC`.
5. Confirm that reset fetches `$FFFE-$FFFF`, resolves to `$F837`, and that the CPU subsequently reads ROM instructions.
6. Probe ACIA TX: it should idle high and transition during monitor output. Correlate this capture with the terminal log.
7. After a stable ASSIST09 `>` prompt, load `src/assist-09/assist09-smoke.s19`, run `G 1000`, and preserve the complete `ASSIST09 RAM SMOKE TEST PASSED` output.

## Podcast Evidence Checklist

For each test, record the date, board revision, CPU/ACIA/EPROM markings, power source, EPROM image SHA-256, terminal configuration, instrument settings, raw observation, and conclusion. Keep photos and captures immutable; place interpretation and follow-up actions in the session log.

## Narrative Arc

The episode can trace the difference between reproducible bytes and a working machine: a verified ROM image, a legacy toolchain that must be made reproducible again, a serial configuration inferred from the actual clock tree, and a small status-bit error whose consequences only become visible at the boundary between software and hardware.
