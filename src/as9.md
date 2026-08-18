### ASSEMBLER as9 SALVAGED

The legendary microprocessor Motorola M6809 had an official assembler called as9 needed for classic 6809 sources. This was ported to MS-DOS by a man named E.J.Rupp. From those sources only one set survived on the Internet, as part of a distribution of a fig-Forth. The author Mike Pashea of (then) Southern Illinois University had the foresight to pack the assembler with the Forth, and even the sources, though he changed them without documenting the changes.

This is not all. The archiver used `arc` has fallen into obscurity, and the versions of `arc` that I had (5.1,4.1) couldn't unpack those sources. Somehow I managed to find a version that could. So I thought to do the community a service by making the source portable accross Linux-gcc and TurboC, adding the executable programs and repacking into the ubiquitous zip format. This package you can download `as9.zip` with its [documentation](/code/as11v2.pdf).

The main reason that this package is worth the effort is that it is the official Motorola assembler. This implements the instruction set as documented in _MC6809-MC6809E Microprocessor Programming Manual (c) Motorola 1981_. You will not find that on the Internet, I'm the proud owner of what appears to be a first generation fotocopy. It can be used to assemble some classic sources, as well as my 6809 version of `ciforth`.

The second reason is that it is meticulously documented in [as11v2.pdf](/code/as11v2.pdf) with respect to the assembler directives or pseudocodes, and the way expressions are built up. However there are some [bugs and changes](/code/bugs-changes.md).

You may be surprised, given that the assembler is hard to find. The reason is that Motorola documented her assemblers together, and this document, maintained for more than 20 years, still contains a 6809 chapter, besides processors that are more popular, like the 6811. (The m6809 still appears in the 2003 Conrad catalog.) There are other 6809 assemblers on the internet, like a gcc version, and the solid work as09_130.zip of Frank Vorstenbosch. You can find those, and more, from Alan DeKok's 6809 Page, now defunct. However an archived version can still be found [here](https://web.archive.org/web/20070112041235/http://www.striker.ottawa.on.ca/6809/).

I advise against conversions. It is a great pain regards pseudo code, and a great danger regards subtle differences. Starting out with more than thousands errors is not uncommon, e.g. the regression test of Vorstenbosch give errors in more than one fifth of the lines, if run through `as9`. Typically, conversion to and from gnu-assemblers is worse, because not even the opcodes are respected. And, by the way, the `.Intel_syntax` directive was only recently added (as per 2.13.90.xx). Regardless it gives errors on all lines that contain comment introduced by a semicolon `;`, for my `ciforth` source this is over 90% of the lines. Forth-assemblers typically have radical differences, too.

### CHANGES TO THE C-SOURCE

The assembler `as9` doesn't support blank space within expressions, and assumes that anything past a valid expression is comment, not even needing a comment character. Because nowadays this is greatly unexpected, and a pitfall, I added a warning about comment that doesn't start with a comment character. (Dark shades of the past. It was not until 2004 that the `gcc` family of assembler outputs a warning for that, after I sent in a bug report.) This was not entirely successful, see also the bugs and more technical comments in my changes document

This was the only real change. I got rid of the crashes, of course, and I made sure that MS-DOS text files can be handled without modification. I left the triple use of `*`, as a comment introducer, an operator and the current program counter.

You will find a file `as9n.c` in the source archive. This is an alternative to `as9.c` if your library does not contain the `strncasecmp()` function. Rename and use as needed.

### CLASSIC 6809 MATERIAL

The official Motorola M6809 assembler as9 is to be used to assemble the famous assist09.asm Motorola monitor program and the forth9.asm 6809 figforth implementation, that you can download `m6809src.zip`, in MS-DOS text format.
It is also used for the 6809 version of [ciforth](https://home.hccnet.nl/a.w.m.van.der.horst/ciforth.html).