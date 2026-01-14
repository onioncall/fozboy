#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t f;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
	bool ime;
	bool halted;
} Cpu;

void cpu_init(Cpu *cpu);

#endif
