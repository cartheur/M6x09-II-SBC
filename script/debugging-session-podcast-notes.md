# M6x09-II-SBC Debugging Session Notes

Use this document as the live technical log and narrative outline for the future podcast episode. Record an observation before interpreting it, retain terminal and scope captures, and distinguish confirmed facts from hypotheses.

## Starting State

- The FTDI adapter and host terminal passed a TX-to-RX loopback test at `19200,n,8,1`, no handshake.
- The board power LED lights, but the SBC has not produced an ASSIST09 banner or `>` prompt.
- The pre-correction `roms/assist09-27c128.bin` image was 16 KiB and carried ASSIST09 at `$F800-$FFFF` with reset vector `$F837`, but it had not passed board acceptance.
- The checked-in assembler executable is a 32-bit 2004 binary which fails on this host with `Bad system call`. The repository now builds an ignored native `as9-host` executable from the original source with GNU89 mode, leaving the historical binary intact.

## Confirmed Software Faults

The ASSIST09 ACIA polling code is not consistent with the checked-in MC6850 data sheet:

- `CIDTA` at `src/assist-09/assist09.asm` reads ACIA status and shifts it twice. Its carry test therefore observes status bit 1 (`TDRE`, transmit-data-register empty), while the comment says it is testing receive data.
- `CODTAO` tests status bit 0 (`RDRF`, receive-data-register full) while its comment says it is waiting for transmit readiness. The source itself marks this `FIXME`.
- The schematic connects CPU `A0` directly to the 6850 `RS` input. Therefore `$BE00` is the data register and `$BE01` is control/status. `COON` reads status at `$BE01` elsewhere in the source, but writes its `$03` reset and `$51` control bytes to `$BE00`; those bytes are transmitted as data rather than configuring the ACIA.

These errors can prevent reliable console input and output even when the board is electrically healthy. Commit `da931d9` corrects them: `COON` writes to `1,X`; `CIDTA` tests `RDRF`; and `CODTAO` tests `TDRE` and waits only while it is clear. The corrected image still requires EPROM programming and hardware acceptance before this becomes a board-level conclusion.

## High-Value Serial Hypothesis

The schematic connects the ACIA `RxC` and `TxC` pins to the 6809 `E` clock. With the documented 7.3728 MHz oscillator, `E` is expected to be 1.8432 MHz. The corrected ASSIST09 candidate initializes the ACIA with `$51`; its low control bits select the MC6850 divide-by-16 mode. If those schematic connections and clock assumptions are correct, the resulting serial rate is **115200 baud**, not the repository's historical 19200 baud setting.

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

## Emulator Evidence: Reset And ACIA Slice

The native C emulator loads the 16 KiB image, reads the reset vector as `$F8,$37`, and executes ASSIST09's vector-initialization routine through its return to `$F83D`. The CPU implementation is intentionally partial: unsupported instructions stop explicitly instead of generating a plausible but false result.

The pre-correction ROM reproduced three failures in executable form: `$03` and `$51` went to `$BE00` (data), leaving `$BE01` control untouched; receive polling observed `TDRE`; and `CODTAO` made its transmit decision from `RDRF`. The corrected-ROM regression test now verifies that `$51` reaches `$BE01`, an output byte is sent when `TDRE` is set, and the routine waits when `TDRE` is clear. This supports the source-level diagnosis, but it does not replace a physical ACIA or scope observation.

## Build-Contract Evidence

Rebuilding with `as9-host` exposed an important artifact-layout constraint. Removing the obsolete second `LSRA` instruction initially made the raw monitor 2,047 bytes. The 16 KiB image target then failed its size guard: 14,336 bytes of `0xFF` padding plus 2,047 bytes of monitor made a 16,383-byte EPROM image. The assembler does not automatically fill the gap created before its fixed-vector `ORG`, so the image could have been malformed even though the source assembled.

The correction preserves the removed instruction's byte with a `NOP`. The rebuilt monitor is again exactly 2,048 bytes; its reset vector remains `$F837`; the full programmer image is exactly 16 KiB; and its checksum verifies. This is a host-side acceptance result, not evidence that the EPROM or board works.

## Corrected Candidate Identity

- Source and image milestone: commit `da931d9` (`Fix ASSIST09 ACIA initialization and polling`)
- Raw monitor: `src/assist-09/assist09.bin`, 2,048 bytes, SHA-256 `6175249f8ea71f8bc4e3c0e92745f4b1a9f8941e078106bf9b196dff904f7592`
- Programmer image: `roms/assist09-27c128.bin`, 16,384 bytes, SHA-256 `1d7fdbe412c8e57084b99b981a24c8fbdbc013227a6da04b1810c67054aa2d72`
- Host checks passed: `scripts/verify-assist09-image.sh`, `make -C emulator test`, and `sha256sum -c assist09-27c128.bin.sha256` from `roms/`.

The only remaining acceptance is physical: back up the existing EPROM; program and verify this candidate; reset at `115200,n,8,1`, no handshake; and preserve the terminal, programmer, and scope evidence.

## Why The Emulator Was Worth The Investment

The board began as a silent physical object. A scope-first session could have consumed hours validating power, reset, clocks, wiring, and levels without revealing that the firmware addressed the ACIA inconsistently. The emulator made the hardware contract executable: memory placement, reset-vector fetch, `A0 -> RS`, ACIA status bits, and data/control register behavior were all stated as testable rules.

That investment also guarded the build chain. The 2,047-byte regression was caught by the programmer-image size check before a chip was programmed, while the emulator regression confirmed that the replacement `NOP` preserved execution through the reset path. The result is not merely a more convenient debugger; it is a way to prevent an incorrect artifact from becoming a misleading hardware symptom.

## Narrative Arc

The episode can trace the difference between reproducible bytes and a working machine: a legacy toolchain made reproducible again; a one-byte layout regression caught before programming; an emulator that turned a silent-board mystery into three precise ACIA defects; and a corrected image that must still earn its claim on real hardware. The through-line is that an emulator does not replace instruments—it makes their time more valuable by narrowing exactly what the physical session must prove.
