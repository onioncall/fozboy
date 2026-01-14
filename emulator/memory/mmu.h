#ifndef MMU_H
#define MMU_H

#include <stdint.h>

typedef struct {
  uint8_t* buf;
  uint16_t len;
  uint16_t start;
  uint16_t end;
} block_t;

typedef enum {
  MMU_ROM_FIXED = 0,
  MMU_ROM_SWITCH,
  MMU_VRAM,
  MMU_EXT_RAM,
  MMU_WRAM,
  MMU_WRAM_SWITCH,
  MMU_ECHO_RAM,
  MMU_OAM,
  MMU_UNUSABLE,
  MMU_IO_REGS,
  MMU_HRAM,
  MMU_INT_ENABLE,
  MMU_BLOCK_COUNT
} mmu_region_t;

typedef struct {
  block_t* blocks[MMU_BLOCK_COUNT];
} mmu_t;

// Cleans up all memory blocks allocated for mmu
void mmu_destroy(mmu_t* mmu);

// Creates new mmu with all blocks allocated
mmu_t* mmu_create();

// Read from virtualized gb memory bank
// Addresses will be the same as on the original GB hardware
uint8_t mmu_read(mmu_t* mmu, uint16_t address);

// Write to virtualized gb memory bank
// Addresses will be the same as on the original GB hardware
void mmu_write(mmu_t* mmu, uint16_t address, uint8_t data);

#endif
