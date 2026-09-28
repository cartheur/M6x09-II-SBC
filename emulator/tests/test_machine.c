#include "machine.h"

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

int main(int argc, char **argv)
{
    assert(argc == 2);
    test_ram_and_unmapped_access();
    test_acia_flags_and_data();
    test_assist09_reset_vector(argv[1]);
    puts("m6x09 machine tests passed");
    return 0;
}
