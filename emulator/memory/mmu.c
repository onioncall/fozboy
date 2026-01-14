// Memory Mapping

#include <stdint.h>
#include <stdlib.h>
#include "mmu.h"

// If buf not provided, will be allocated
block_t* new_block(uint16_t start, uint16_t end, uint8_t* buf) {
  uint16_t len = end - start + 1;

  if (!buf) {
    buf = calloc(len, sizeof(uint8_t));
    if (!buf) {
      return NULL;
    }
  }

  block_t* block = malloc(sizeof(block_t));
  if (!block) {
    free(buf);
    return NULL;
  }

  *block = (block_t){
    .len = len,
    .start = start,
    .end = end,
    .buf = buf
  };

  return block;
}

void block_destroy(block_t* block) {
  if (!block) return;
  free(block->buf);
  free(block);
}

void mmu_destroy(mmu_t* mmu) {
  block_destroy(mmu->blocks[MMU_ROM_FIXED]);
  block_destroy(mmu->blocks[MMU_ROM_SWITCH]);
  block_destroy(mmu->blocks[MMU_VRAM]);
  block_destroy(mmu->blocks[MMU_EXT_RAM]);
  block_destroy(mmu->blocks[MMU_WRAM]);
  block_destroy(mmu->blocks[MMU_WRAM_SWITCH]);
  block_destroy(mmu->blocks[MMU_ECHO_RAM]);
  block_destroy(mmu->blocks[MMU_OAM]);
  block_destroy(mmu->blocks[MMU_UNUSABLE]);
  block_destroy(mmu->blocks[MMU_IO_REGS]);
  block_destroy(mmu->blocks[MMU_HRAM]);
  block_destroy(mmu->blocks[MMU_INT_ENABLE]);

  free(mmu);
}

mmu_t* mmu_create() {
  mmu_t* mmu = calloc(1, sizeof(mmu_t));
  if (!mmu) return NULL;

  mmu->blocks[MMU_ROM_FIXED] = new_block(0x0000, 0x3FFF, NULL);
  if (!mmu->blocks[MMU_ROM_FIXED]) goto cleanup;

  mmu->blocks[MMU_ROM_SWITCH] = new_block(0x4000, 0x7FFF, NULL);
  if (!mmu->blocks[MMU_ROM_SWITCH]) goto cleanup;

  mmu->blocks[MMU_VRAM] = new_block(0x8000, 0x9FFF, NULL);
  if (!mmu->blocks[MMU_VRAM]) goto cleanup;

  mmu->blocks[MMU_EXT_RAM] = new_block(0xA000, 0xBFFF, NULL);
  if (!mmu->blocks[MMU_EXT_RAM]) goto cleanup;

  mmu->blocks[MMU_WRAM] = new_block(0xC000, 0xCFFF, NULL);
  if (!mmu->blocks[MMU_WRAM]) goto cleanup;

  mmu->blocks[MMU_WRAM_SWITCH] = new_block(0xD000, 0xDFFF, NULL);
  if (!mmu->blocks[MMU_WRAM_SWITCH]) goto cleanup;

  mmu->blocks[MMU_ECHO_RAM] = new_block(0xE000, 0xFDFF, mmu->blocks[MMU_WRAM]->buf);
  if (!mmu->blocks[MMU_ECHO_RAM]) goto cleanup;

  mmu->blocks[MMU_OAM] = new_block(0xFE00, 0xFE9F, NULL);
  if (!mmu->blocks[MMU_OAM]) goto cleanup;

  mmu->blocks[MMU_UNUSABLE] = new_block(0xFEA0, 0xFEFF, NULL);
  if (!mmu->blocks[MMU_UNUSABLE]) goto cleanup;

  mmu->blocks[MMU_IO_REGS] = new_block(0xFF00, 0xFF7F, NULL);
  if (!mmu->blocks[MMU_IO_REGS]) goto cleanup;

  mmu->blocks[MMU_HRAM] = new_block(0xFF80, 0xFFFE, NULL);
  if (!mmu->blocks[MMU_HRAM]) goto cleanup;

  mmu->blocks[MMU_INT_ENABLE] = new_block(0xFFFF, 0xFFFF, NULL);
  if (!mmu->blocks[MMU_INT_ENABLE]) goto cleanup;

  return mmu;

cleanup:
  mmu_destroy(mmu);
  return NULL;
}


uint8_t mmu_read(mmu_t* mmu, uint16_t address) {
  for (int i = 0; i < MMU_BLOCK_COUNT; i++) {
    block_t* block = mmu->blocks[i];

    if (address >= block->start && address <= block->end) {
      uint16_t rel_addr = address - block->start;

      return block->buf[rel_addr];
    }
  }
  return 0xFF; // unmapped
}

void mmu_write(mmu_t* mmu, uint16_t address, uint8_t data) {
  for (int i = 0; i < MMU_BLOCK_COUNT; i++) {
    block_t* block = mmu->blocks[i];

    if (address >= block->start && address <= block->end) {
      uint16_t rel_addr = address - block->start;
      block->buf[rel_addr] = data;
    }
  }
}
