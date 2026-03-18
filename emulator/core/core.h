#ifndef CORE_H
#define CORE_H

#include "../cartridge/cart.h"
#include "../memory/mmu.h"
#include "../cpu/cpu.h"
#include <stdlib.h>

typedef struct {
  cart_t *cart;
  mmu_t *mmu;
  Cpu *cpu;
} core_t;

typedef enum {
  ERR_CORE_OK = 0,
  ERR_CORE_INVALID_OPCODE,
} ERR_CORE;

void core_destroy(core_t *core);
core_t* core_create(char *file_name);
ERR_CORE run();

#endif
