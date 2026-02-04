// Memory Mapping

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"
#include "rom.h"
#include "mbc.h"

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
  mbc_destroy(mmu->mbc);

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

mmu_t* mmu_create(rom_t* rom) {
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

  mmu->mbc = mbc_create(rom);
  if (!mmu->mbc) goto cleanup;

  mmu->ram_enabled = false;
  mmu->current_ram_bank = 0;
  
  mmu->timer_enabled = false;
  mmu->rtc_s = 0;
  mmu->rtc_m = 0;
  mmu->rtc_h = 0;
  mmu->rtc_dl = 0;
  mmu->rtc_dh = 0;
  mmu->rtc_s_latched = 0;
  mmu->rtc_m_latched = 0;
  mmu->rtc_h_latched = 0;
  mmu->rtc_dl_latched = 0;
  mmu->rtc_dh_latched = 0;

  return mmu;

cleanup:
  mmu_destroy(mmu);
  return NULL;
}


uint8_t mmu_read(mmu_t* mmu, uint16_t address) {
  
  // Special handling for external RAM region
  if (address >= 0xA000 && address <= 0xBFFF) {
    if (!mmu->ram_enabled) {
      return 0xFF;
    }
    
    // Check if an RTC register is selected (MBC3 only)
    if (mmu->mbc->regs->rtc_register >= 0x08 && mmu->mbc->regs->rtc_register <= 0x0C) {
      // Return latched RTC register value
      switch (mmu->mbc->regs->rtc_register) {
        case 0x08: return mmu->rtc_s_latched;
        case 0x09: return mmu->rtc_m_latched;
        case 0x0A: return mmu->rtc_h_latched;
        case 0x0B: return mmu->rtc_dl_latched;
        case 0x0C: return mmu->rtc_dh_latched;
      }
    }
  }
  
  // identify block to read from and read
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
  
  // Special handling for external RAM region
  if (address >= 0xA000 && address <= 0xBFFF) {
    if (!mmu->ram_enabled && !mmu->timer_enabled) {
      return;
    }
    
    // Check if an RTC register is selected (MBC3 only)
    if (mmu->mbc->regs->rtc_register >= 0x08 && mmu->mbc->regs->rtc_register <= 0x0C) {
      // Write to RTC register
      switch (mmu->mbc->regs->rtc_register) {
        case 0x08: mmu->rtc_s = data; return;
        case 0x09: mmu->rtc_m = data; return;
        case 0x0A: mmu->rtc_h = data; return;
        case 0x0B: mmu->rtc_dl = data; return;
        case 0x0C: mmu->rtc_dh = data; return;
      }
    }
  }
  
  // identify block to write to and write
  for (int i = 0; i < MMU_BLOCK_COUNT; i++) {
    block_t* block = mmu->blocks[i];

    if (address >= block->start && address <= block->end) {
      uint16_t rel_addr = address - block->start;
      block->buf[rel_addr] = data;
    }
  }
}

void write_rom_fixed(mmu_t* mmu, rom_t* rom_full) {
  block_t* block = mmu->blocks[MMU_ROM_FIXED];

  uint16_t cpy_len = block->len;
  if (rom_full->size < cpy_len) {
    cpy_len = rom_full->size;
  }

  memcpy(block->buf, rom_full->data, cpy_len);
}

int switch_rom(mmu_t* mmu, rom_t* rom_full, uint16_t bank, uint8_t fixed_rom) {
  mmu_region_t block_key = fixed_rom ? MMU_ROM_FIXED : MMU_ROM_SWITCH;
  block_t* block = mmu->blocks[MMU_ROM_SWITCH];

  if (bank < 2 || bank > 512) { return -1; }

  long address = bank * block->len;
  if (address >= rom_full->size) { return -1; }

  memcpy(block->buf, &rom_full->data[address], block->len);
  return 0;
}

int switch_ram(mmu_t* mmu, uint16_t bank) {
  if (!mmu->mbc->rom->is_ram) { return -1; }
  
  mmu->current_ram_bank = bank;
  
  // TODO - swap between actual RAM banks here
  // - these ram banks will be used in .sav files
  // - variable number of banks for different cart types
  return 0;
}

int mbc_intercept(mmu_t* mmu, rom_t* rom_full, mbc_t* mbc, uint16_t addr, uint8_t data) {
  intercept_flags_t flags = mbc->intercept(mbc, addr, data);

  if (flags.set_switch_bank) {
    switch_rom(mmu, rom_full, flags.switch_bank, 0);
  }
  if (flags.set_fixed_bank) {
    switch_rom(mmu, rom_full, flags.fixed_bank, 1);
  }
  if (flags.set_ram_gate) {
    mmu->ram_enabled = flags.ram_gate_enabled;
  }
  if (flags.set_ram_bank) {
    switch_ram(mmu, flags.ram_bank);
  }
  if (flags.set_timer) {
    mmu->timer_enabled = flags.timer_enabled;
  }
  if (flags.latch_rtc) {
    // Freeze the clock in place
    // this is done in case the clock tics between reads of s, m, h, etc
    mmu->rtc_s_latched = mmu->rtc_s;
    mmu->rtc_m_latched = mmu->rtc_m;
    mmu->rtc_h_latched = mmu->rtc_h;
    mmu->rtc_dl_latched = mmu->rtc_dl;
    mmu->rtc_dh_latched = mmu->rtc_dh;
  }

  return flags.mbc;
}
