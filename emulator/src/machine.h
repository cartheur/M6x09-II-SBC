#ifndef M6X09_MACHINE_H
#define M6X09_MACHINE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum {
    M6X09_RAM_SIZE = 0x8000,
    M6X09_ROM_SIZE = 0x4000,
    M6X09_ACIA_ADDRESS = 0xBE00,
    M6X09_ROM_ADDRESS = 0xC000,
    M6X09_RESET_VECTOR = 0xFFFE,
    M6X09_ACIA_RDRF = 0x01,
    M6X09_ACIA_TDRE = 0x02
};

typedef struct {
    uint8_t status;
    uint8_t control;
    uint8_t receive_data;
    uint8_t transmit_data;
    size_t transmit_count;
} M6x09Acia;

typedef struct {
    uint8_t ram[M6X09_RAM_SIZE];
    uint8_t rom[M6X09_ROM_SIZE];
    M6x09Acia acia;
    uint16_t program_counter;
    bool rom_loaded;
} M6x09Machine;

void m6x09_machine_init(M6x09Machine *machine);
bool m6x09_load_rom(M6x09Machine *machine, const char *path);
uint8_t m6x09_read(M6x09Machine *machine, uint16_t address);
void m6x09_write(M6x09Machine *machine, uint16_t address, uint8_t value);
void m6x09_reset(M6x09Machine *machine);
void m6x09_acia_receive(M6x09Machine *machine, uint8_t value);

#endif
