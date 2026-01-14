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

#endif
