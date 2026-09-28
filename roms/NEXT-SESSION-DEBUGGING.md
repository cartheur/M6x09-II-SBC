# Next Session: Debugging The New Board

Status: corrected ASSIST09 candidate is host-verified; SBC response pending.

## Evidence Already Collected

- The Tcl terminal previously opened `/dev/ttyUSB0` at `19200`, `n,8,1`, with no handshake.
- The FTDI adapter passed a TX-to-RX loopback test.
- The SBC power LED is lit.
- No characters or ASSIST09 prompt were received from the board using the pre-correction ROM.
- The emulator reproduced three serial defects in that ROM: setup bytes were written to ACIA data rather than control, receive polling tested `TDRE`, and transmit polling tested `RDRF`. The corrected candidate is `roms/assist09-27c128.bin` with SHA-256 `1d7fdbe412c8e57084b99b981a24c8fbdbc013227a6da04b1810c67054aa2d72`.

The host terminal and FTDI adapter are therefore not the current primary suspects. Program the corrected candidate before resuming board-side checks.

## Debugging Order

1. Read and save the currently installed EPROM, then program and verify the corrected 16 KiB image with Batronix. Confirm exact device selection and chip orientation.
2. Start the terminal at `115200`, `n,8,1`, with no handshake; capture a reset attempt. The corrected `$51` ACIA control value and the 7.3728 MHz/E-clock design imply this expected rate.
3. Confirm the FTDI wiring from signal labels, not connector position:
   - FTDI `TXO` -> board / ACIA `RX`
   - FTDI `RXI` -> board / ACIA `TX`
   - FTDI `GND` -> board `GND`
4. Confirm that FTDI logic levels are compatible with the board's 6850 ACIA.
5. Measure the supply voltage at the CPU, EPROM, and ACIA. The power LED alone does not prove that every IC is powered correctly.
6. Check CPU reset and clock with a scope or logic probe. Reset must release, and the 7.3728 MHz clock must be present.
7. At reset, verify that the CPU fetches `$FFFE-$FFFF`. The programmed image supplies reset vector `$F837`; execution should continue there.
8. If the CPU is executing, probe ACIA TX. It should idle high and show transitions on reset or after carriage returns are sent from the terminal.

## Acceptance Condition

The next session reaches the ROM workflow only when the terminal receives an ASSIST09 `>` prompt. Then continue with the RAM smoke test in [README.md](README.md).
