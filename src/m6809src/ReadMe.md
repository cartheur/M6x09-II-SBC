## M6x09 Salvage & Boneyard

* Details can be found [here](http://land-boards.com/blwiki/index.php?title=RetroComputers).
* Compatible repo is [here](https://github.com/cartheur/SalvagedRetro/tree/master/6809)

### Background

The system can plug into a computer's USB port where it shows up as a serial port. It is powered by USB (taking about 230 mA of current) and you communicate with it using a terminal emulator.

The provided BASIC is a port of Microsoft Extended BASIC for the 6809-based Radio Shack Color Computer. A disassembly of it was published in two books. In Grant Searle's port, all code and commands that were not applicable (e.g. graphics, sound, cassette tape i/o) were removed. It is just under 10KB in size. Pretty typical of Microsoft BASIC of the era, it has a few quirks, e.g. available memory i available from the variable MEM rather than a function FRE(). Extended Color Computer BASIC has some additional commands that are useful like RENUMber, additional math functions, PRINT USING, TRON/TROFF and even EDIT to support editing program lines. Apparently this was the last version of Microsoft BASIC that Bill Gates personally worked on. You can find online copies of books on the Colour Computer and its version of BASIC.

I took the BASIC firmware and got it to build under Linux using the as09 cross-assembler with no warnings. I found and fixed a small typo that might have affected the PRINT USING command.

I breadboarded the CPU, clock, and reset circuit on a solderless breadboard. Then I got the 6809 to free run by forcing all the data lines all low. I decided not to breadboard the entire circuit as it would be quite tedious to do so, wouldn't fit on my small breadboard, and the design looked stable and had been verified by others, so I opted to move directly to a PCB layout.

I made a PCB layout from EasyEDA (using the autorouter).

Looking for machine language monitors, I found source code for the ASSIST09 program that Motorola offers for their 6809-based development ports. I got it to cross-assemble under Linux, and modified it to work with the 6850 ACIA instead of what the Motorola hardware used. With a little debugging, I got it running. It works well, and provides most feature you want for development and debug, including memory display and change, register display and change, running and breakpoints, and generating and loading Motorola S record files. here is a sample session:

```
ASSIST09
>D 1000 20

      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  
1000 45 41 4C 49 53 54 49 43 2E 22 20 3A 20 90 00 10  EALISTIC." : ...
1010 56 2F 26 87 20 22 54 48 45 20 42 49 52 54 48 44  V/&. "THE BIRTHD
>R
PC-F842 A-00 B-00 X-20FE Y-F002 U-60C2 S-6051 CC-F4 DP-00 
PC-
>M 1000
45-55
>P 1000 100F
S13100055414C49535449432E22203A2090001014
S9030000FC
>
```

I am able to cross-assemble code on a Linux computer and then load the S record file onto the board via the serial port. This is much more efficient than erasing and burning EPROMs.

Note that for uploading you need to add delays due to no hardware handshaking. I use the `ascii-xfr` program on Linux to do this, as well as the `minicom` terminal emulator.

I wrote a couple of example programs that run with the ASSIST09 monitor, using it's `SWI` functions for I/O.

Next, I combined BASIC and ASSIST09 into one EPPROM. It comes up in ASSIST09 but you can get to BASIC using the `G D000` command. This offers the ability to play with BASIC or machine language without swapping EPROMs.

```
ASSIST09
>G D000
6809 EXTENDED BASIC
(C) 1982 BY MICROSOFT

OK
10 FOR I = 1 TO 10
20 PRINT I,
30 NEXT I
RUN
 1               2               3               4               5
 6               7               8               9               10
OK
```
I have built up a second board and am just waiting to receive a second 68B09 chip to complete it. I have a few programming projects in mind to work on, and some code (like a small C compiler) that I want to look at.

### References

* [AS9 assembler](http://home.hccnet.nl/a.w.m.van.der.horst/m6809.html)
* Salvaged [here](/code/as9.md)