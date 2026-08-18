# External Reference Bundle

This folder holds external reference material gathered to support Linux bring-up and serial programming of the M6x09-II SBC.

## Saved Locally

- [../DFR0065_schematics.pdf](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/datasheets/DFR0065_schematics.pdf)
  - DFRobot DFR0065 FTDI breakout schematic
  - Source: `https://dfimg.dfrobot.com/enshop/image/data/DFR0065/DFR0065_schematics.pdf`

- [DFR0065_product_page.html](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/datasheets/external/DFR0065_product_page.html)
  - DFRobot product page for the FT232RL USB-to-TTL breakout
  - Useful for feature notes like DTR exposure, 3.3V/5V selection, and the standard FTDI cable pinout language
  - Source: `https://www.dfrobot.com/product-147.html`

- [MC6850_pinout_reference.html](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/datasheets/external/MC6850_pinout_reference.html)
  - Human-readable MC6850/68B50 pinout summary used to resolve `U1_2` and `U1_6`
  - Source: `https://www.citylan.it/index.php/6850`

## Not Saved Cleanly

These sources were identified, but direct command-line fetches were blocked by anti-bot challenge pages:

- FT232R product and datasheet pages on `ftdichip.com`
- IA6850/MC6850 datasheet pages on `alldatasheet.com`

Use these URLs manually in a browser if the full vendor PDF is needed:

- FT232R product page: `https://ftdichip.com/products/ft232rl/`
- FTDI USB IC document index: `https://ftdichip.com/document/usb-ic-data-sheets/`
- IA6850 datasheet landing page: `https://www.alldatasheet.com/datasheet-pdf/pdf/66013/INNOVASIC/IA6850.html`

## Why These Matter

Together with the local board files:

- [design/Layout_M6809-II-SBC.png](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/design/Layout_M6809-II-SBC.png)
- [design/PCB_6x09-II.json](/home/cartheur/ame/aiventure/aiventure-github/cartheur/M6x09-II-SBC/design/PCB_6x09-II.json)

these references support the working serial mapping:

- `P1` pin 1 -> `GND`
- `P1` pin 4 -> `U1_2` -> ACIA `Rx Data`
- `P1` pin 5 -> `U1_6` -> ACIA `Tx Data`
- `P1` pin 3 -> optional power feed through `H1`
