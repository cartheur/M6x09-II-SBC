AS9 := ../assembler/as9

.PHONY: all assist09 forth09 clean

all: assist09

assist09: ../assist-09/assist09.bin

forth09: forth09.bin

../assist-09/assist09.bin: ../assist-09/assist09.asm $(AS9)
	cd ../assist-09 && ../assembler/as9 assist09.asm -l c s bin s19 cre now

forth09.bin: forth09.asm $(AS9)
	$(AS9) forth09.asm -l c s bin s19 cre

clean:
	rm -f ../assist-09/assist09.bin ../assist-09/assist09.lst ../assist-09/assist09.s19
	rm -f forth09.bin forth09.lst forth09.s19
