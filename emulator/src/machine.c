#include "machine.h"

#include <stdio.h>
#include <string.h>

static bool is_acia_status(uint16_t address)
{
    return address == M6X09_ACIA_ADDRESS + 1;
}

static bool is_acia_data(uint16_t address)
{
    return address == M6X09_ACIA_ADDRESS;
}

void m6x09_machine_init(M6x09Machine *machine)
{
    memset(machine, 0, sizeof(*machine));
    machine->acia.status = M6X09_ACIA_TDRE;
}

bool m6x09_load_rom(M6x09Machine *machine, const char *path)
{
    FILE *rom_file = fopen(path, "rb");
    size_t bytes_read;
    int trailing_byte;

    if (rom_file == NULL) {
        return false;
    }

    bytes_read = fread(machine->rom, 1, M6X09_ROM_SIZE, rom_file);
    trailing_byte = fgetc(rom_file);
    fclose(rom_file);

    if (bytes_read != M6X09_ROM_SIZE || trailing_byte != EOF) {
        return false;
    }

    machine->rom_loaded = true;
    return true;
}

uint8_t m6x09_read(M6x09Machine *machine, uint16_t address)
{
    if (address < M6X09_RAM_SIZE) {
        return machine->ram[address];
    }

    if (is_acia_status(address)) {
        return machine->acia.status;
    }

    if (is_acia_data(address)) {
        uint8_t value = machine->acia.receive_data;

        machine->acia.status &= (uint8_t)~M6X09_ACIA_RDRF;
        return value;
    }

    if (address >= M6X09_ROM_ADDRESS) {
        return machine->rom[address - M6X09_ROM_ADDRESS];
    }

    return 0xff;
}

void m6x09_write(M6x09Machine *machine, uint16_t address, uint8_t value)
{
    if (address < M6X09_RAM_SIZE) {
        machine->ram[address] = value;
        return;
    }

    if (is_acia_status(address)) {
        machine->acia.control = value;
        if (value == 0x03) {
            machine->acia.status = M6X09_ACIA_TDRE;
            machine->acia.receive_data = 0;
            machine->acia.transmit_data = 0;
        }
        return;
    }

    if (is_acia_data(address)) {
        machine->acia.transmit_data = value;
        machine->acia.transmit_count++;
    }
}

void m6x09_reset(M6x09Machine *machine)
{
    uint8_t high = m6x09_read(machine, M6X09_RESET_VECTOR);
    uint8_t low = m6x09_read(machine, M6X09_RESET_VECTOR + 1);

    machine->program_counter = (uint16_t)((uint16_t)high << 8) | low;
}

void m6x09_acia_receive(M6x09Machine *machine, uint8_t value)
{
    machine->acia.receive_data = value;
    machine->acia.status |= M6X09_ACIA_RDRF;
}
