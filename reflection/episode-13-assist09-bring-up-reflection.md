# Episode 13 Reflection: Feel the ROM Burn

## What We Set Out To Do

This episode began with a modest goal: make ASSIST09 a dependable monitor for the M6x09-II SBC. The practical outcome was larger. We made an archived assembler build reproducibly on a modern host, corrected the monitor's ACIA contract, produced and read back several EPROM images, proved the board's physical execution path, and made the Tcl terminal behave like a reliable development tool rather than another variable in the diagnosis.

The final result is intentionally ordinary-looking:

```text
ASSIST09 RAM SMOKE TEST PASSED
ASSIST09
>
```

Those few lines now stand on a substantial chain of evidence. With no CTS jumper installed, the board boots at `115200,n,8,1`, receives an S-record, writes and executes RAM, returns to the monitor, and presents a prompt. The final full-device EPROM readback matches the programmer image byte for byte with SHA-256 `7e564502d120123d82dc5fc04b62f397e79c95b15ff34fe09c73e75ebbce85a0`.

## The First Lesson: A Build Is Hardware Work When It Produces ROMs

The project inherited an assembler executable from 2004 that would not run on the current host. Rather than replace it with an unrelated toolchain, we built a native `as9-host` from the accompanying original source. That let the project create the same monitor artifacts again, locally and inspectably.

Reproducibility immediately found a trap. A sensible source cleanup had removed an obsolete instruction, leaving the raw monitor one byte short. The monitor still assembled, but the fixed-vector layout meant the 16 KiB programmer image was no longer exactly the required size. The repair kept the layout byte as a `NOP`, restoring a 2,048-byte monitor, an image ending at `$FFFF`, and reset vector `$F837`.

That is not a cosmetic build detail. An EPROM programmer accepts bytes; it does not know which gaps in an address map were unintended. Image-size checks, address-range checks, reset-vector checks, and checksums are as much a part of hardware discipline as a continuity meter.

## The ACIA Was Not One Error

The board maps the MC6850-compatible ACIA at `$BE00-$BE01`. CPU address line A0 connects directly to the ACIA `RS` pin, so the hardware contract is:

| CPU address | ACIA function |
| --- | --- |
| `$BE00` | Control / Status |
| `$BE01` | Transmit / Receive Data |

The inherited monitor did not consistently honour that contract. Its receive path tested the transmit-empty flag; its transmit path tested the receive-full flag; and its initial reset/configuration writes had been sent to the data register rather than the control register. The earlier source repair corrected all three paths, while an emulator regression made the register offsets and status-bit behaviour executable rather than merely descriptive.

This was a useful warning about plausible fixes. A model can faithfully reproduce an incorrect assumption if the model and source share it. The decisive evidence was the actual data sheet, the actual A0-to-`RS` connection, and a scope probe on the actual chip.

## Instruments Narrowed the Question

The first powered boot was silent. Instead of changing source again, we worked outward from basic physical facts:

- The FTDI power path was corrected and measured at the board.
- Reset was observed as a slow RC release, not the apparent digital chatter produced by an unsuitable short capture.
- The CPU E clock measured about 1.843 MHz, matching the 7.3728 MHz oscillator divided by four.
- The lower address bus was active after reset.
- ACIA receive and transmit clocks were present at the expected rate.
- The reset vector and ROM layout had already been independently verified in the rebuilt image.

Each observation removed a failure class. The board had power, left reset, clocked, fetched instructions, and accessed the ACIA. That made a blank terminal a much smaller problem than it first appeared.

The scope also taught a humbler lesson. A mismatched or missing probe ground can generate a persuasive but false comparison. We rechecked channels on the same pin with a shared ground before using a split capture to compare `RS` and `TXDATA`. The result—active register select with an idle transmit output—was valid only because the measurement arrangement had been validated first.

## CTS Was High, but It Was Not Floating

The next apparent explanation was CTS. The 6850 inhibits `TDRE` when CTS is high, exactly matching an active ACIA that never emits the monitor banner. We initially made two mistakes worth retaining in the story.

First, the pin was misidentified: CTS is U1 pin 24; pin 23 is DCD. The data sheet corrected that quickly. Second, the low-resolution schematic view made CTS appear unconnected. A temporary jumper from CTS to ground immediately proved the rest of the system: ASSIST09 booted, loaded the RAM smoke test, and ran it. That was excellent diagnostic evidence, but it was not a permanent wiring prescription.

The EasyEDA PCB view supplied the missing fact. U1 pin 24 (`CTS`) is routed to U1 pin 5 (`RTS`). This is local hardware flow control. The problem was firmware configuration.

The previous control byte, `$51`, selected the right framing and divide-by-16 serial rate but set the MC6850's RTS output high. Because RTS is physically looped to CTS, that one setting raised CTS and disabled the transmitter's empty status. The monitor then waited correctly, forever, for a condition its own initialization had prevented.

The final control byte is `$11`: the same divide-by-16 and 8N1 settings, with RTS low and transmit interrupts disabled. No soldered pull-down was needed. The temporary CTS jumper was removed, a new full 16 KiB image was programmed and read back, and the board passed the smoke test without any jumper.

## ROM Burning Was a Conversation, Not a Ritual

Each EPROM pass had a named candidate and a purpose. The programmer image was built on Linux, programmed through the Batronix workflow, read back in full, and compared byte-for-byte—not merely trusted because a programming dialog reported success. The resulting repository history preserves the image, checksum, correction, and reason for the next burn.

This is why a two-EPROM development rotation is a productive constraint. It encourages each burn to answer a single question. The repository becomes the long-term record; the chips remain reusable instruments rather than an expanding collection of unlabeled near-misses.

## The Terminal Is Part of the System

After the monitor worked, the host terminal exposed two separate protocol mistakes.

The first was S-record uploading. The Tcl sender waited for a carriage return after each line. ASSIST09's loader intentionally suppresses echo and carriage returns while it is loading, so the sender transmitted the first record and then waited forever for a response that the protocol deliberately does not send. The terminal now recognizes S-record files and sends them as a character-paced continuous stream.

The second was interactive input. Typing in the terminal's scrollback window sent GUI key events immediately, bypassing the configured inter-character delay. A polled ACIA can lose rapidly delivered characters, which made commands appear to require a second attempt. Direct typing is now queued and paced at 5 ms per character; the command-entry field remains the preferred route for complete monitor commands.

The takeaway is not that serial terminals are fragile curiosities. It is that the host tool is part of the serial protocol. Its pacing, echo expectations, focus rules, and line endings deserve the same explicit treatment as a board trace.

## What We Would Do Differently

We would inspect both the schematic and the routed PCB earlier, especially for signals whose schematic labels suggest an intended relationship. We would decode every bit of the ACIA control word before accepting a serial configuration that merely produces the expected baud rate. We would validate probe grounding before interpreting two-channel scope captures. And we would exercise the terminal's actual loader handshake with a three-record smoke file before declaring its upload mode ready.

Those are not reasons to distrust tools, emulators, or assistants. They are reasons to make every layer falsifiable. An emulator is valuable because it turns a hardware contract into a regression test. A scope is valuable because it catches assumptions the emulator cannot see. A complete EPROM readback is valuable because it distinguishes programmed bytes from intended bytes. A small RAM smoke test is valuable because it joins all of those claims into one observable result.

## Closing

At the end of the episode, ASSIST09 is no longer just a 2 KiB binary at the top of an EPROM. It is a verified recovery environment: a monitor with a known reset vector, a known serial configuration, a known RTS/CTS relationship, a reproducible programmer image, a full readback, a tested loader, and a terminal that respects the board's speed.

The most useful final image is not the scope trace or the ROM label. It is the prompt after the smoke test returns:

```text
ASSIST09 RAM SMOKE TEST PASSED
ASSIST09
>
```

That prompt is the invitation to begin the next phase—RAM-resident experiments and, eventually, the Forth port—without turning every new idea into another blind ROM burn.

## Evidence In This Repository

- [Final RTS/CTS acceptance record](../roms/NEXT-RTS-CTS-LOOPBACK-TEST.md)
- [Original ACIA register-select diagnosis](../roms/ACIA-REGISTER-SELECT-ROOT-CAUSE-2026-09-28.md)
- [Terminal workflow](../terminal/README.md)
- [Live debugging and episode notes](../script/debugging-session-podcast-notes.md)
- [EasyEDA CTS-to-RTS PCB evidence](../images/CTS-RTS-PCB.jpg)
