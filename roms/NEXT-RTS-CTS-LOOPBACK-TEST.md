# Next Test: RTS/CTS Loopback Candidate

Date: 2026-10-13

## Root Cause

The EasyEDA PCB capture [CTS-RTS-PCB.jpg](../images/CTS-RTS-PCB.jpg) shows U1 pin 24 (`CTS`) on net `U1_5`, which is U1 pin 5 (`RTS`). This is intentional local hardware flow control, not a floating input.

The prior ACIA control byte `$51` sets `CR6=1, CR5=0`. Per the MC6850 data sheet this drives RTS high. The local connection therefore makes CTS high, inhibits `TDRE`, and blocks the monitor's transmit polling loop.

## Candidate

`src/assist-09/assist09.asm` now writes `$11` in `COON`:

- bits 0–4 remain unchanged: divide-by-16 and 8N1 framing;
- `CR6=0, CR5=0` drives RTS low with the transmit interrupt disabled.

The 16 KiB programmer image is `roms/assist09-27c128.bin`, SHA-256:

```text
f2415a89500c6539153a626db2b70be33d331fe30ab247a874c679d829a51721
```

Host verification passed:

```text
make -C emulator test
scripts/verify-assist09-image.sh
```

## Board Procedure

1. Keep the board powered off and leave the temporary CTS-to-ground jumper removed.
2. Program this full 16 KiB candidate, then perform a complete programmer readback and compare it to the stated SHA-256.
3. Install the EPROM with power removed, then boot normally with no CTS jumper.
4. Confirm `ASSIST09` and `>` at `115200,n,8,1`, no handshake.
5. Load `src/assist-09/assist09-smoke.s19` with `L`, then run `G 1000`.
6. Record `ASSIST09 RAM SMOKE TEST PASSED` as the final no-jumper acceptance result.
