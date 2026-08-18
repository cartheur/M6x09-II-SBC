# M6x09 Linux Terminal Bring-Up

This folder contains the Linux-side terminal workflow for the M6x09-II SBC. The intent is to keep a stable monitor in ROM and do normal development over serial by loading code into RAM.

## What This Uses

- [m6x09-terminal.tcl](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/terminal/m6x09-terminal.tcl) as the host terminal
- `P1` as the board's 6-pin FTDI-style serial header in [design/Layout_M6809-II-SBC.png](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/design/Layout_M6809-II-SBC.png)
- `H1` as the optional USB-power jumper between `P1_3` and board `VCC`
- ASSIST09 in ROM for prompt, inspection, S-record load, and RAM execution

## Recommended Host Setup

Install Tcl/Tk if needed:

```bash
sudo apt-get install tk
```

Launch the terminal:

```bash
wish terminal/m6x09-terminal.tcl -device /dev/ttyUSB0 -baud 19200
```

Recommended settings in the terminal window:

- Device: `/dev/ttyUSB0` or `/dev/serial/by-id/...`
- Speed: `19200`
- ParityAndBits: `n,8,1`
- Hand Shake: `none`
- Pause(ms): `2`

The script is tuned for prompt-driven, line-paced uploads. It waits for a carriage return before sending the next line, which matches the repo note that uploads need pacing because there is no hardware handshaking.

## P1 Wiring

`P1` is a 1x6 FTDI-style header. From the PCB netlist in [design/PCB_6x09-II.json](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/design/PCB_6x09-II.json):

- `P1` pin 1 = `GND`
- `P1` pin 3 = `P1_3`
- `P1` pin 4 = `U1_2`
- `P1` pin 5 = `U1_6`

The ACIA package `U1` is a 68B50/6850-compatible part whose serial pins are:

- pin 2 = `Rx Data`
- pin 6 = `Tx Data`

That makes the practical hookup:

- FTDI `GND` -> `P1` pin 1 `GND`
- FTDI `TXO` -> `P1` pin 4 `U1_2` -> ACIA `Rx Data`
- FTDI `RXI` -> `P1` pin 5 `U1_6` -> ACIA `Tx Data`
- FTDI `VCC` -> `P1` pin 3 only if you intentionally want the FTDI board to power the SBC
- FTDI `DTR` not required for console use

## Power Notes

`P1_3` is not just a logic pin. It is tied to `H1`, the `USB POWER JUMPER`, which bridges that node to board `VCC`.

Use one power strategy at a time:

- If powering the board from the FTDI breakout, fit `H1` and connect FTDI `VCC` to `P1` pin 3.
- If powering the board some other way, leave FTDI `VCC` disconnected unless you have verified the power path.

Do not back-power the board from two sources at once.

## First Bring-Up

1. Connect `GND`, `TXO`, and `RXI`.
2. Apply power.
3. Start the terminal.
4. Press Enter a few times or use `M6x09 -> Send CR`.
5. Confirm that you get an `ASSIST09` banner and `>` prompt.

Useful first commands:

```text
R
D 1000 20
G D000
```

Expected behavior:

- `R` shows registers
- `D` displays memory
- `G D000` jumps from ASSIST09 into BASIC, as described in [code/ReadMe.md](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/code/ReadMe.md)

## Programming Strategy

Use the ROM as infrastructure, not as your inner-loop target.

The recommended loop is:

1. Keep ASSIST09 in EPROM as the bootstrap and debugger.
2. Assemble code on Linux.
3. Send Motorola S-records over serial into RAM.
4. Run and debug from RAM.
5. Burn a new EPROM image only when a RAM build is stable enough to preserve.

This fits the repo's existing software notes in [code/ReadMe.md](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/code/ReadMe.md), including:

- ASSIST09 on a 6850 ACIA
- `LOAD` and `PUNCH` support
- BASIC reachable with `G D000`

## Sending Files

Use `File -> Send...` in the Tcl terminal to send text or S-record files line by line. The script's send path is intentionally conservative:

- characters are sent one at a time
- a CR is appended at end of line
- the next line waits for a CR response

If uploads stall:

- keep `Hand Shake` on `none`
- increase `Pause(ms)` from `2` to `5` or `10`
- restart the serial port from the menu
- tap `Send CR` to recover the prompt

## Reference Files In This Repo

- Board overview: [README.md](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/README.md)
- Software workflow notes: [code/ReadMe.md](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/code/ReadMe.md)
- Layout image: [design/Layout_M6809-II-SBC.png](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/design/Layout_M6809-II-SBC.png)
- PCB net evidence: [design/PCB_6x09-II.json](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/design/PCB_6x09-II.json)

## External Reference Set

These are the key outside references used to complete the Linux-side picture:

- DFRobot DFR0065 FTDI breakout product page: https://www.dfrobot.com/product-147.html
- DFRobot DFR0065 schematic PDF: https://dfimg.dfrobot.com/enshop/image/data/DFR0065/DFR0065_schematics.pdf
- FT232R product page and datasheet index: https://ftdichip.com/products/ft232rl/
- 6850-compatible pinout reference: https://www.alldatasheet.com/html-pdf/66013/INNOVASIC/IA6850/405/1/IA6850.html

## What To Freeze In ROM

The safest ROM contents for this board are:

- reset vectors
- ASSIST09 monitor
- optional BASIC image
- optional tiny bootstrap helpers

Do not use the EPROM as the place for every experiment. Use it as the stable recovery environment that lets serial development stay fast.
