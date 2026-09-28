#include "cpu6809.h"

#include <stdio.h>
#include <string.h>

static void usage(const char *program)
{
    fprintf(stderr, "Usage: %s --rom PATH\n", program);
}

int main(int argc, char **argv)
{
    M6x09Machine machine;
    M6x09Cpu6809 cpu;
    const char *rom_path = NULL;

    if (argc == 3 && strcmp(argv[1], "--rom") == 0) {
        rom_path = argv[2];
    } else {
        usage(argv[0]);
        return 2;
    }

    m6x09_machine_init(&machine);
    if (!m6x09_load_rom(&machine, rom_path)) {
        fprintf(stderr, "Unable to load a 16 KiB ROM image: %s\n", rom_path);
        return 1;
    }

    m6x09_reset(&machine);
    m6x09_cpu6809_reset(&cpu, &machine);
    printf("ROM loaded: %s (16 KiB)\n", rom_path);
    printf("reset reads: $FFFE=$%02X $FFFF=$%02X\n",
           m6x09_read(&machine, M6X09_RESET_VECTOR),
           m6x09_read(&machine, M6X09_RESET_VECTOR + 1));
    printf("reset PC: $%04X\n", machine.program_counter);
    printf("ACIA status: $%02X (TDRE=%u RDRF=%u)\n", machine.acia.status,
           (machine.acia.status & M6X09_ACIA_TDRE) != 0,
           (machine.acia.status & M6X09_ACIA_RDRF) != 0);
    printf("first opcode: $%02X at $%04X\n", m6x09_read(&machine, cpu.pc), cpu.pc);
    printf("CPU reset-path instruction subset is available; monitor SWI is not implemented yet.\n");
    return 0;
}
