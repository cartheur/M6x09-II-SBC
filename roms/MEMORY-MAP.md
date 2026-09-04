# M6x09-II-SBC Memory Map

Use this map when placing ROM code or creating full-device programmer images.

| CPU address range | Size | Assignment |
| --- | ---: | --- |
| `$0000-$7FFF` | 32 KiB | RAM |
| `$8000-$9FFF` | 8 KiB | Unused |
| `$A000-$BFFF` | 8 KiB | Serial interface region |
| `$C000-$FFFF` | 16 KiB | ROM (`27C128`) |

## Current ASSIST09 Placement

| CPU address range | Device offset | Contents |
| --- | ---: | --- |
| `$C000-$F7FF` | `$0000-$37FF` | Available for future ROM code; currently `0xFF` padding |
| `$F800-$FFFF` | `$3800-$3FFF` | ASSIST09 monitor and hardware vectors |

The current ASSIST09 image is 2 KiB and is assembled for `$F800`. The `programmer-image` target creates a complete 16 KiB image for the `27C128`:

```bash
make -C src/assist-09 programmer-image
```

The resulting [assist09-27c128.bin](assist09-27c128.bin) places ASSIST09 at device offset `$3800` so it appears at CPU address `$F800`.

## Rules For Future ROMs

1. Keep the hardware vectors at `$FFF0-$FFFF` unless the ROM design deliberately replaces ASSIST09's reset path.
2. Reserve `$F800-$FFFF` for ASSIST09 when using it as the recovery monitor.
3. Place additional ROM code in `$C000-$F7FF` and document its CPU address range and device offset.
4. Build a full 16 KiB programmer image for the `27C128`; do not program a raw partial binary without placing it at the corresponding device offset.
5. Do not place ROM code in `$0000-$7FFF`, `$8000-$9FFF`, or `$A000-$BFFF`.

The ASSIST09 source uses the serial ACIA at `$BE00`, within the serial-interface region.
