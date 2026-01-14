#include "gbc.h"
#include "cpu/cpu.h"
#include <stdio.h>
#include <stdlib.h>

int run(void) {
	Cpu cpu;
	cpu_init(&cpu);

	uint8_t bit = (1 << 4);
	for (int i = 7; i >= 0; i--) {
		printf("%d", (bit >> i) & 1);
	}
	printf("\n");

	return 0;
}
