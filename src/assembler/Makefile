

SRCH=   \
as.h    \
table9.h \
#

SRCC=   \
as9.c   \
as.c    \
do9.c   \
eval.c  \
ffwd.c  \
output.c        \
pseudo.c        \
symtab.c        \
util.c          \
#

CC ?= gcc
CFLAGS ?= -std=gnu89 -w -g
AS9 := ./as9-host

.PHONY: as9 as9-host

# This is peculiar, as9 only includes other files.
# The main program sits in as.c
as9: as9-host

as9-host: $(SRCC) $(SRCH)
	$(CC) $(CFLAGS) as9.c -o $@

as9.tgz : $(SRCC) $(SRCH) as9 changes.doc Makefile; tar cfz $@ $^

as9.zip : $(SRCC) $(SRCH) as9n.c as9 as9.exe as9_changes.txt Makefile; zip $@ $^

m6809src.zip : assist09.asm forth09.asm ; zip $@ $^

testrom :  as9
	$(AS9) rom.asm -l c s bin s19 cre
	diff -b -B rom.lst results/rom.lst
	diff -b -B rom.bin results/rom.bin
	diff -b -B rom.s19 results/rom.s19

testassist09 :  as9
	$(AS9) assist09.asm -l c s bin s19 cre now
	diff -b -B assist09.lst results/assist09.lst
	diff -b -B assist09.bin results/assist09.bin
	diff -b -B assist09.s19 results/assist09.s19

testforth09 :  as9
	$(AS9) forth09.asm -l c s bin s19 cre
	diff -b -B forth09.lst results/forth09.lst
	diff -b -B forth09.bin results/forth09.bin
	diff -b -B forth09.s19 results/forth09.s19

test : testrom testassist09 testforth09
