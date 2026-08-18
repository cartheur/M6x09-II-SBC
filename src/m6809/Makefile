AS9 := ../as9/as9

TARGETS := assist09 forth9
ARTIFACT_SUFFIXES := bin lst s19

.PHONY: all assist09 forth9 clean

all: assist09

assist09: assist09.bin assist09.lst assist09.s19

forth9: forth9.bin forth9.lst forth9.s19

assist09.bin assist09.lst assist09.s19: assist09.asm $(AS9)
	$(AS9) assist09.asm -l c s bin s19 cre now

forth9.bin forth9.lst forth9.s19: forth9.asm $(AS9)
	$(AS9) forth9.asm -l c s bin s19 cre

clean:
	rm -f assist09.bin assist09.lst assist09.s19
	rm -f forth9.bin forth9.lst forth9.s19
