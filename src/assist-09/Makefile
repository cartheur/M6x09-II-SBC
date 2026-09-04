AS9 := ../assembler/as9
PROGRAMMER_IMAGE := ../../roms/assist09-27c128.bin
PROGRAMMER_CHECKSUM := $(PROGRAMMER_IMAGE).sha256

.PHONY: all programmer-image smoke clean

all: assist09.bin

assist09.bin: assist09.asm $(AS9)
	$(AS9) assist09.asm -l c s bin s19 cre now

programmer-image: $(PROGRAMMER_IMAGE) $(PROGRAMMER_CHECKSUM)

$(PROGRAMMER_IMAGE): assist09.bin
	dd if=/dev/zero bs=14336 count=1 2>/dev/null | tr '\000' '\377' > $@
	cat $< >> $@
	test "$$(wc -c < $@)" -eq 16384

$(PROGRAMMER_CHECKSUM): $(PROGRAMMER_IMAGE)
	cd ../../roms && sha256sum assist09-27c128.bin > assist09-27c128.bin.sha256

smoke: assist09-smoke.s19

assist09-smoke.s19: assist09-smoke.asm $(AS9)
	$(AS9) assist09-smoke.asm -l c s bin s19 cre now

clean:
	rm -f assist09.bin assist09.lst assist09.s19 assist09.sym assist09.crf
	rm -f assist09-smoke.bin assist09-smoke.lst assist09-smoke.s19 assist09-smoke.sym assist09-smoke.crf
