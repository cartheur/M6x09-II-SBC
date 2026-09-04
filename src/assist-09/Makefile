AS9 := ../assembler/as9

.PHONY: all smoke clean

all: assist09.bin

assist09.bin: assist09.asm $(AS9)
	$(AS9) assist09.asm -l c s bin s19 cre now

smoke: assist09-smoke.s19

assist09-smoke.s19: assist09-smoke.asm $(AS9)
	$(AS9) assist09-smoke.asm -l c s bin s19 cre now

clean:
	rm -f assist09.bin assist09.lst assist09.s19 assist09.sym assist09.crf
	rm -f assist09-smoke.bin assist09-smoke.lst assist09-smoke.s19 assist09-smoke.sym assist09-smoke.crf
