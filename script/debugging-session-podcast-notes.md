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

## Programmer And First-Boot Evidence (2026-09-28)

The corrected 16 KiB image was programmed from the Windows host. Its complete EPROM readback was preserved as `roms/readback.bin` and compared on Linux with `roms/assist09-27c128.bin`: all 16,384 bytes matched, with the expected SHA-256 `1d7fdbe412c8e57084b99b981a24c8fbdbc013227a6da04b1810c67054aa2d72`. This confirms the programmed device contents, not board execution.

The first boot remained silent. At `115200,n,8,1`, no handshake, resetting the board produced neither the `ASSIST09` banner nor a `>` prompt. A separate direct 45-second FTDI capture sent a carriage return and included a board reset; it received zero bytes. The terminal GUI is therefore not the immediate explanation. The physical acceptance work now begins with reset release, CPU clock, and reset-vector fetch at `$FFFE-$FFFF` (expected vector `$F837`), before moving to ACIA TX and serial wiring.

## Reset, Clock, And Address-Bus Milestone (2026-09-28)

The next physical observations narrowed the silence without changing firmware. The AD2 scope showed the board's reset circuit behaving as an RC release: holding Reset drove H2 pin 7 low, and releasing it produced a smooth rise that settled near 4.5 V after roughly 100–150 ms. A short digital capture had misleadingly appeared to show reset chatter; it was sampling the slow analog ramp as it crossed the analyzer threshold. The long capture made the distinction visible.

With reset released, H2 pin 9 (`E`) measured 1.8431 MHz, exactly the expected CPU E-clock rate derived from the 7.3728 MHz oscillator. The AD2 Logic Analyzer then captured continuously changing values on H2 A0–A11, grouped as a hexadecimal bus. The CPU is therefore receiving power, leaving reset, clocking, and generating address-bus activity. The HD6309 installed in the board remains compatible with the ASSIST09/6809 reset-vector and bus assumptions used here.

The diagnostic sequence was deliberately incremental: correct the FTDI supply jumper, scope reset over a long enough interval to see its RC release, verify E-clock, then observe the lower address bus. Each step removed one category of failure without claiming serial success. The next instrument point is ACIA TX—U1 pin 6 / P1 pin 5—after the reset-release delay. It should idle high and show 115200-baud traffic during the ASSIST09 banner. A continued idle line would move the investigation to the ACIA's supply, select/control signals, and TX routing rather than the terminal application.

## The ACIA Turn: When the Firmware Met the Pinout

The TX probe did not remain idle. U1 pin 6 showed real digital bursts, while U1 pins 3 and 4 showed the expected receive and transmit clocks at approximately 1.843 MHz. That result moved the question from “is the ACIA alive?” to “what configuration did it receive?” The `RS` probe supplied the final bridge between firmware and hardware.

The MC6850 defines `RS=0` for Control/Status and `RS=1` for Transmit/Receive Data. The board routes CPU A0 directly to U1 pin 11 (`RS`), making `$BE00` Control/Status and `$BE01` Data. The monitor had those two offsets reversed. Its apparent ACIA initialization wrote `$03` and `$51` to the data register; the short TX bursts were the ACIA transmitting those values at its default divide-by-1 rate rather than accepting a 115200-baud configuration.

This is the point where disciplined measurement earned a source change. The repair is intentionally small and mechanical: offset 0 for status/control and offset 1 for data in `CIDTA`, `COON`, and `CODTAO`. The emulator model and regression test were corrected to the same map. The resulting 2,048-byte monitor and 16 KiB programmer image pass the emulator, clean-room image verifier, and checksum check; the new programmer-image SHA-256 is `37ece487da6b49c7d9f24deb34d25598983782a9ff958558f39a1dff2e5cc843`. It is the next EPROM candidate, not yet a hardware result.

## A Note On Agents And Responsibility

This debugging session is also part of the episode's argument against treating an agent's output as self-authenticating. An agent, operating through the project account, made an honest but consequential ACIA mapping mistake while attempting to fix the monitor. The inherited archive code already contained part of the reversal; the later agent change extended it to initialization. Neither intent nor passing host tests was enough to make the result correct.

The developer's responsibility does not disappear when an agent writes code: review the proposed change, compare it with the authoritative data sheet and schematic, measure the hardware, and retain evidence. Here the ROM readback, reset waveform, clocks, TX waveform, and `RS` trace turned an apparently plausible software fix into a falsifiable hardware claim. That is the standard the episode should leave with subscribers: use agents as collaborators, but keep diagnosis and acceptance accountable to the developer.

## Active Working Step: First-Boot Hardware Diagnosis

This is the current hand-off from host/emulator work to physical hardware.

This step is the technical core of the titular podcast Episode 13, **“Feel the (ROM) burn.”** The episode follows the corrected image from reproducible host artifact, through independently verified programming and physical installation, to the next disciplined hardware observation.

1. Apply the ACIA register-offset source repair, then rebuild and host-verify a new 16 KiB programmer image.
2. Program and full-device-readback verify that new image before installing it.
3. Probe TX again; the frame timing should now be 115200 baud (about 8.68 us per bit) rather than the default divide-by-1 timing.
4. Once serial output appears, capture the `ASSIST09` banner and `>` prompt at `115200,n,8,1`, no handshake. Only then build and send `src/assist-09/assist09-smoke.s19`, run `G 1000`, and record `ASSIST09 RAM SMOKE TEST PASSED`.

Capture the backup filename and checksum, programmer device selection and verify result, EPROM orientation, terminal configuration, and every observed character. A silent first boot is still useful evidence: proceed to the board-side voltage, reset, clock, reset-vector, and ACIA-TX checks rather than changing multiple variables at once.

## Why The Emulator Was Worth The Investment

The board began as a silent physical object. A scope-first session could have consumed hours validating power, reset, clocks, wiring, and levels without revealing that the firmware addressed the ACIA inconsistently. The emulator made the hardware contract executable: memory placement, reset-vector fetch, `A0 -> RS`, ACIA status bits, and data/control register behavior were all stated as testable rules.

That investment also guarded the build chain. The 2,047-byte regression was caught by the programmer-image size check before a chip was programmed, while the emulator regression confirmed that the replacement `NOP` preserved execution through the reset path. The result is not merely a more convenient debugger; it is a way to prevent an incorrect artifact from becoming a misleading hardware symptom.

## Narrative Arc

The episode can trace the difference between reproducible bytes and a working machine: a legacy toolchain made reproducible again; a one-byte layout regression caught before programming; an emulator that turned a silent-board mystery into three precise ACIA defects; and a corrected image that must still earn its claim on real hardware. The through-line is that an emulator does not replace instruments—it makes their time more valuable by narrowing exactly what the physical session must prove.
