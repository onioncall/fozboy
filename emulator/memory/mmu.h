#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include "rom.h"
#include "mbc.h"

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
  mbc_t* mbc;
  
  // External RAM state
  bool ram_enabled;
  uint16_t current_ram_bank;
  
  // RTC state (for MBC3)
  bool timer_enabled;
  uint8_t rtc_s;           // seconds (0-59)
  uint8_t rtc_m;           // minutes (0-59)
  uint8_t rtc_h;           // hours (0-23)
  uint16_t rtc_dl;         // day counter lower 8 bits
  uint8_t rtc_dh;          // day counter upper bit + flags
  
  // Latched RTC values
  uint8_t rtc_s_latched;
  uint8_t rtc_m_latched;
  uint8_t rtc_h_latched;
  uint16_t rtc_dl_latched;
  uint8_t rtc_dh_latched;
} mmu_t;

// Cleans up all memory blocks allocated for mmu
void mmu_destroy(mmu_t* mmu);

// Creates new mmu with all blocks allocated
mmu_t* mmu_create(rom_t* rom);

// Read from virtualized gb memory bank
// Addresses will be the same as on the original GB hardware
uint8_t mmu_read(mmu_t* mmu, uint16_t address);

// Write to virtualized gb memory bank
// Addresses will be the same as on the original GB hardware
void mmu_write(mmu_t* mmu, uint16_t address, uint8_t data);

// Write the fixed length first block of rom to memory
void write_rom_fixed(mmu_t* mmu, rom_t* rom_full);

// Switch to another switchable rom bank in memory
// bank: 2-512
// Returns -1 if bank is invalid or if bank would exceed the size of rom_full
int switch_rom(mmu_t* mmu, rom_t* rom_full, uint16_t bank, uint8_t fixed_rom);

#endif
