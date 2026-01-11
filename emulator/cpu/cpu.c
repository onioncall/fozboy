#include <stdint.h>

// CPU state (registers, flags)

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

typedef struct {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t f; // only to be used for flags
	uint8_t h;
	uint8_t l;
	uint16_t sp; // Stack Pointer
	uint16_t pc; // Program Counter
} Cpu;

void cpu_init(Cpu *cpu) {
	cpu->a = 0;
	cpu->b = 0;
	cpu->c = 0;
	cpu->d = 0;
	cpu->e = 0;
	cpu->f = 0;
	cpu->l = 0;
	cpu->sp = 0;
	cpu->pc = 0;
}
