#include "cpu6809.h"

#include <assert.h>
#include <stdio.h>

static void test_ram_and_unmapped_access(void)
{
    M6x09Machine machine;

    m6x09_machine_init(&machine);
    m6x09_write(&machine, 0x1000, 0xa5);
    assert(m6x09_read(&machine, 0x1000) == 0xa5);
    assert(m6x09_read(&machine, 0x8000) == 0xff);
}

static void test_acia_flags_and_data(void)
{
    M6x09Machine machine;

    m6x09_machine_init(&machine);
    assert(m6x09_read(&machine, M6X09_ACIA_ADDRESS) == M6X09_ACIA_TDRE);

    m6x09_acia_receive(&machine, 'A');
    assert((m6x09_read(&machine, M6X09_ACIA_ADDRESS) & M6X09_ACIA_RDRF) != 0);
    assert(m6x09_read(&machine, M6X09_ACIA_ADDRESS + 1) == 'A');
    assert((m6x09_read(&machine, M6X09_ACIA_ADDRESS) & M6X09_ACIA_RDRF) == 0);

    m6x09_write(&machine, M6X09_ACIA_ADDRESS + 1, 'B');
    assert(machine.acia.transmit_data == 'B');
    assert(machine.acia.transmit_count == 1);
}

static void test_assist09_reset_vector(const char *rom_path)
{
    M6x09Machine machine;

    m6x09_machine_init(&machine);
    assert(m6x09_load_rom(&machine, rom_path));
    assert(m6x09_read(&machine, M6X09_RESET_VECTOR) == 0xf8);
    assert(m6x09_read(&machine, M6X09_RESET_VECTOR + 1) == 0x37);

    m6x09_reset(&machine);
    assert(machine.program_counter == 0xf837);
}

static void test_assist09_vector_initialization(const char *rom_path)
{
    M6x09Machine machine;
    M6x09Cpu6809 cpu;
    unsigned int steps = 0;

    m6x09_machine_init(&machine);
    assert(m6x09_load_rom(&machine, rom_path));
    m6x09_cpu6809_reset(&cpu, &machine);
    while (cpu.pc != 0xf83d && steps++ < 256) {
        M6x09CpuResult result = m6x09_cpu6809_step(&cpu, &machine);

        if (result != M6X09_CPU_OK) {
            fprintf(stderr, "unsupported opcode $%02X at $%04X\n",
                    cpu.unsupported_opcode, (uint16_t)(cpu.pc - 1));
        }
        assert(result == M6X09_CPU_OK);
    }

    if (steps >= 256) {
        fprintf(stderr, "reset path did not return after %u steps; PC=$%04X B=$%02X S=$%04X\n",
                steps, cpu.pc, cpu.b, cpu.s);
    }
    assert(steps < 256);
    assert(cpu.pc == 0xf83d);
    assert(cpu.dp == 0x5f);
    assert(m6x09_read(&machine, 0x5f9d) == 0x5f);
    assert(m6x09_read(&machine, 0x5fc2) == 0x5f);
    assert(m6x09_read(&machine, 0x5fc3) == 0xc2);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    test_ram_and_unmapped_access();
    test_acia_flags_and_data();
    test_assist09_reset_vector(argv[1]);
    test_assist09_vector_initialization(argv[1]);
    puts("m6x09 machine tests passed");
    return 0;
}
