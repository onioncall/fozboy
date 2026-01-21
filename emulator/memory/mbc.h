#ifndef MBC_H
#define MBC_H

#include <stdint.h>
#include "rom.h"

typedef struct {
  bool set_switch_bank;
  uint16_t switch_bank; // 9 bit
  bool set_fixed_bank;
  uint16_t fixed_bank; // 9 bit
  bool set_ram_gate;
  bool ram_gate_enabled;
} intercept_flags_t;

typedef struct {
  uint8_t ramg; // 4 bit. 0b1010 : ram enable
  uint8_t bank1; // 5 bit. Cannot be 0b00000, init to 0b00001
  uint8_t bank2; // 2 bit. 
  uint8_t mode; // 1 bit.
} mbc_regs_t;

typedef struct mbc_t {
  mbc_regs_t* regs;
  rom_t* rom;
  void(*intercept)(struct mbc_t* self, uint16_t addr, uint8_t data);
  intercept_flags_t* flags;
} mbc_t;

mbc_t* mbc_create(rom_t* rom);
void mbc_destroy(mbc_t* mbc);

#endif
