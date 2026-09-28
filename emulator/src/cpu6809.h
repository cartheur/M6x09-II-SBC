#ifndef M6X09_CPU6809_H
#define M6X09_CPU6809_H

#include "machine.h"

#include <stdint.h>

typedef struct {
    uint8_t a;
    uint8_t b;
    uint8_t cc;
    uint8_t dp;
    uint16_t x;
    uint16_t y;
    uint16_t u;
    uint16_t s;
    uint16_t pc;
    uint8_t unsupported_opcode;
} M6x09Cpu6809;

typedef enum {
    M6X09_CPU_OK,
    M6X09_CPU_UNSUPPORTED
} M6x09CpuResult;

void m6x09_cpu6809_reset(M6x09Cpu6809 *cpu, M6x09Machine *machine);
M6x09CpuResult m6x09_cpu6809_step(M6x09Cpu6809 *cpu, M6x09Machine *machine);

#endif
