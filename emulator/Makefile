CC ?= cc
CPPFLAGS := -Isrc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -g

BUILD := build
EMU := $(BUILD)/m6x09-emu
TEST := $(BUILD)/test-machine
ROM ?= ../roms/assist09-27c128.bin

.PHONY: all run test clean

all: $(EMU)

$(BUILD):
	mkdir -p $@

$(EMU): src/main.c src/machine.c src/machine.h src/cpu6809.c src/cpu6809.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) src/main.c src/machine.c src/cpu6809.c -o $@

$(TEST): tests/test_machine.c src/machine.c src/machine.h src/cpu6809.c src/cpu6809.h | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_machine.c src/machine.c src/cpu6809.c -o $@

run: $(EMU)
	$(EMU) --rom $(ROM)

test: $(TEST)
	$(TEST) ../roms/assist09-27c128.bin

clean:
	rm -rf $(BUILD)
