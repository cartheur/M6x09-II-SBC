#include "cpu6809.h"

#include <string.h>

enum {
    M6X09_CC_ZERO = 0x04
};

static uint8_t fetch_byte(M6x09Cpu6809 *cpu, M6x09Machine *machine)
{
    return m6x09_read(machine, cpu->pc++);
}

static uint16_t fetch_word(M6x09Cpu6809 *cpu, M6x09Machine *machine)
{
    uint16_t high = fetch_byte(cpu, machine);

    return (uint16_t)(high << 8) | fetch_byte(cpu, machine);
}

static uint16_t read_word(M6x09Machine *machine, uint16_t address)
{
    uint16_t high = m6x09_read(machine, address);

    return (uint16_t)(high << 8) | m6x09_read(machine, address + 1);
}

static void write_word(M6x09Machine *machine, uint16_t address, uint16_t value)
{
    m6x09_write(machine, address, (uint8_t)(value >> 8));
    m6x09_write(machine, address + 1, (uint8_t)value);
}

static uint16_t get_d(const M6x09Cpu6809 *cpu)
{
    return (uint16_t)((uint16_t)cpu->a << 8) | cpu->b;
}

static void set_d(M6x09Cpu6809 *cpu, uint16_t value)
{
    cpu->a = (uint8_t)(value >> 8);
    cpu->b = (uint8_t)value;
}

static void set_zero(M6x09Cpu6809 *cpu, uint16_t value)
{
    if (value == 0) {
        cpu->cc |= M6X09_CC_ZERO;
    } else {
        cpu->cc &= (uint8_t)~M6X09_CC_ZERO;
    }
}

static uint16_t *index_register(M6x09Cpu6809 *cpu, uint8_t postbyte)
{
    switch ((postbyte >> 5) & 0x03) {
    case 0:
        return &cpu->x;
    case 1:
        return &cpu->y;
    case 2:
        return &cpu->u;
    default:
        return &cpu->s;
    }
}

static uint16_t indexed_address(M6x09Cpu6809 *cpu, M6x09Machine *machine)
{
    uint8_t postbyte = fetch_byte(cpu, machine);
    uint16_t *base;

    if ((postbyte & 0x80) == 0) {
        int8_t offset = (int8_t)(postbyte & 0x1f);

        if ((offset & 0x10) != 0) {
            offset = (int8_t)(offset | (int8_t)0xe0);
        }
        return (uint16_t)(*index_register(cpu, postbyte) + offset);
    }

    base = index_register(cpu, postbyte);
    switch (postbyte & 0x1f) {
    case 0x00: {
        uint16_t address = *base;
        *base += 1;
        return address;
    }
    case 0x01: {
        uint16_t address = *base;
        *base += 2;
        return address;
    }
    case 0x04:
        return *base;
    case 0x0c: {
        int8_t offset = (int8_t)fetch_byte(cpu, machine);

        return (uint16_t)(cpu->pc + offset);
    }
    case 0x0d: {
        int16_t offset = (int16_t)fetch_word(cpu, machine);

        return (uint16_t)(cpu->pc + offset);
    }
    default:
        return 0;
    }
}

static void push_byte(M6x09Cpu6809 *cpu, M6x09Machine *machine, uint8_t value)
{
    m6x09_write(machine, --cpu->s, value);
}

static void push_word(M6x09Cpu6809 *cpu, M6x09Machine *machine, uint16_t value)
{
    push_byte(cpu, machine, (uint8_t)value);
    push_byte(cpu, machine, (uint8_t)(value >> 8));
}

static uint8_t pull_byte(M6x09Cpu6809 *cpu, M6x09Machine *machine)
{
    return m6x09_read(machine, cpu->s++);
}

static uint16_t pull_word(M6x09Cpu6809 *cpu, M6x09Machine *machine)
{
    uint16_t high = pull_byte(cpu, machine);

    return (uint16_t)(high << 8) | pull_byte(cpu, machine);
}

static uint16_t transfer_word(const M6x09Cpu6809 *cpu, uint8_t code)
{
    switch (code) {
    case 0x0:
        return get_d(cpu);
    case 0x1:
        return cpu->x;
    case 0x2:
        return cpu->y;
    case 0x3:
        return cpu->u;
    case 0x4:
        return cpu->s;
    case 0x5:
        return cpu->pc;
    case 0x8:
        return cpu->a;
    case 0x9:
        return cpu->b;
    case 0xa:
        return cpu->cc;
    case 0xb:
        return cpu->dp;
    default:
        return 0;
    }
}

static void set_transfer(M6x09Cpu6809 *cpu, uint8_t code, uint16_t value)
{
    switch (code) {
    case 0x0:
        set_d(cpu, value);
        break;
    case 0x1:
        cpu->x = value;
        break;
    case 0x2:
        cpu->y = value;
        break;
    case 0x3:
        cpu->u = value;
        break;
    case 0x4:
        cpu->s = value;
        break;
    case 0x5:
        cpu->pc = value;
        break;
    case 0x8:
        cpu->a = (uint8_t)value;
        break;
    case 0x9:
        cpu->b = (uint8_t)value;
        break;
    case 0xa:
        cpu->cc = (uint8_t)value;
        break;
    case 0xb:
        cpu->dp = (uint8_t)value;
        break;
    default:
        break;
    }
}

void m6x09_cpu6809_reset(M6x09Cpu6809 *cpu, M6x09Machine *machine)
{
    memset(cpu, 0, sizeof(*cpu));
    m6x09_reset(machine);
    cpu->pc = machine->program_counter;
}

M6x09CpuResult m6x09_cpu6809_step(M6x09Cpu6809 *cpu, M6x09Machine *machine)
{
    uint8_t opcode = fetch_byte(cpu, machine);
    uint16_t address;

    switch (opcode) {
    case 0x1f: {
        uint8_t postbyte = fetch_byte(cpu, machine);

        set_transfer(cpu, postbyte & 0x0f, transfer_word(cpu, postbyte >> 4));
        break;
    }
    case 0x20: {
        int8_t displacement = (int8_t)fetch_byte(cpu, machine);

        cpu->pc = (uint16_t)(cpu->pc + displacement);
        break;
    }
    case 0x26: {
        int8_t displacement = (int8_t)fetch_byte(cpu, machine);

        if ((cpu->cc & M6X09_CC_ZERO) == 0) {
            cpu->pc = (uint16_t)(cpu->pc + displacement);
        }
        break;
    }
    case 0x30:
        cpu->x = indexed_address(cpu, machine);
        break;
    case 0x31:
        cpu->y = indexed_address(cpu, machine);
        break;
    case 0x32:
        cpu->s = indexed_address(cpu, machine);
        break;
    case 0x33:
        cpu->u = indexed_address(cpu, machine);
        break;
    case 0x34: {
        uint8_t mask = fetch_byte(cpu, machine);

        if ((mask & 0x04) != 0) {
            push_byte(cpu, machine, cpu->b);
        }
        break;
    }
    case 0x35: {
        uint8_t mask = fetch_byte(cpu, machine);

        if ((mask & 0x04) != 0) {
            cpu->b = pull_byte(cpu, machine);
        }
        if ((mask & 0x80) != 0) {
            cpu->pc = pull_word(cpu, machine);
        }
        break;
    }
    case 0x4f:
        cpu->a = 0;
        set_zero(cpu, cpu->a);
        break;
    case 0x5a:
        cpu->b--;
        set_zero(cpu, cpu->b);
        break;
    case 0x6a:
        address = indexed_address(cpu, machine);
        m6x09_write(machine, address, (uint8_t)(m6x09_read(machine, address) - 1));
        set_zero(cpu, m6x09_read(machine, address));
        break;
    case 0x8d: {
        int8_t displacement = (int8_t)fetch_byte(cpu, machine);

        push_word(cpu, machine, cpu->pc);
        cpu->pc = (uint16_t)(cpu->pc + displacement);
        break;
    }
    case 0x8e:
        cpu->x = fetch_word(cpu, machine);
        set_zero(cpu, cpu->x);
        break;
    case 0x97:
        address = (uint16_t)((uint16_t)cpu->dp << 8) | fetch_byte(cpu, machine);
        m6x09_write(machine, address, cpu->a);
        set_zero(cpu, cpu->a);
        break;
    case 0xa6:
        cpu->a = m6x09_read(machine, indexed_address(cpu, machine));
        set_zero(cpu, cpu->a);
        break;
    case 0xa7:
        m6x09_write(machine, indexed_address(cpu, machine), cpu->a);
        set_zero(cpu, cpu->a);
        break;
    case 0xac:
        set_zero(cpu, (uint16_t)(cpu->x - read_word(machine, indexed_address(cpu, machine))));
        break;
    case 0xad:
        address = indexed_address(cpu, machine);
        push_word(cpu, machine, cpu->pc);
        cpu->pc = address;
        break;
    case 0xc6:
        cpu->b = fetch_byte(cpu, machine);
        set_zero(cpu, cpu->b);
        break;
    case 0xe3:
        set_d(cpu, (uint16_t)(get_d(cpu) + read_word(machine, indexed_address(cpu, machine))));
        set_zero(cpu, get_d(cpu));
        break;
    case 0xed:
        write_word(machine, indexed_address(cpu, machine), get_d(cpu));
        break;
    case 0xef:
        write_word(machine, indexed_address(cpu, machine), cpu->u);
        break;
    default:
        cpu->unsupported_opcode = opcode;
        return M6X09_CPU_UNSUPPORTED;
    }

    return M6X09_CPU_OK;
}
