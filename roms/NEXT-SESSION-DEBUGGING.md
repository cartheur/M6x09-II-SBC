# Next Session: Debugging The New Board

Status: serial host path proven; SBC response pending.

## Evidence Already Collected

- The Tcl terminal opened `/dev/ttyUSB0` at `19200`, `n,8,1`, with no handshake.
- The FTDI adapter passed a TX-to-RX loopback test.
- The SBC power LED is lit.
- No characters or ASSIST09 prompt were received from the board.

The host terminal and FTDI adapter are therefore not the current primary suspects. Resume with board-side checks.

## Debugging Order

1. Confirm the FTDI wiring from signal labels, not connector position:
   - FTDI `TXO` -> board / ACIA `RX`
   - FTDI `RXI` -> board / ACIA `TX`
   - FTDI `GND` -> board `GND`
2. Confirm that FTDI logic levels are compatible with the board's 6850 ACIA.
3. Read the EPROM with Batronix and compare it against [assist09-27c128.bin](assist09-27c128.bin). Confirm exact device selection, successful verification, and correct chip orientation.
4. Measure the supply voltage at the CPU, EPROM, and ACIA. The power LED alone does not prove that every IC is powered correctly.
5. Check CPU reset and clock with a scope or logic probe. Reset must release, and the 7.3728 MHz clock must be present.
6. At reset, verify that the CPU fetches `$FFFE-$FFFF`. The programmed image supplies reset vector `$F837`; execution should continue there.
7. If the CPU is executing, probe ACIA TX. It should idle high and show transitions on reset or after carriage returns are sent from the terminal.

## Acceptance Condition

The next session reaches the ROM workflow only when the terminal receives an ASSIST09 `>` prompt. Then continue with the RAM smoke test in [README.md](README.md).
