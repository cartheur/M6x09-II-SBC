AS9 := ../assembler/as9
PROGRAMMER_IMAGE := assist09-27c128.bin

.PHONY: all programmer-image smoke clean

all: assist09.bin

assist09.bin: assist09.asm $(AS9)
	$(AS9) assist09.asm -l c s bin s19 cre now

programmer-image: $(PROGRAMMER_IMAGE)

$(PROGRAMMER_IMAGE): assist09.bin
	dd if=/dev/zero bs=14336 count=1 2>/dev/null | tr '\000' '\377' > $@
	cat $< >> $@
	test "$$(wc -c < $@)" -eq 16384

smoke: assist09-smoke.s19

assist09-smoke.s19: assist09-smoke.asm $(AS9)
	$(AS9) assist09-smoke.asm -l c s bin s19 cre now

clean:
	rm -f assist09.bin assist09.lst assist09.s19 assist09.sym assist09.crf
	rm -f $(PROGRAMMER_IMAGE)
	rm -f assist09-smoke.bin assist09-smoke.lst assist09-smoke.s19 assist09-smoke.sym assist09-smoke.crf
