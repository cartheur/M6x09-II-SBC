AS9 := ../assembler/as9

.PHONY: all forth09 clean

all: forth09

forth09: forth09.bin

forth09.bin: forth09.asm $(AS9)
	$(AS9) forth09.asm -l c s bin s19 cre

clean:
	rm -f forth09.bin forth09.lst forth09.s19 forth09.sym forth09.crf
