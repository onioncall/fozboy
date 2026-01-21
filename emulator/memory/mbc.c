
#include <stdlib.h>
#include <string.h>
#include "mbc.h"
#include "rom.h"

void mbc1_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  memset(self->flags, 0, sizeof(intercept_flags_t));
  self->regs->bank1 = 0b00001;

  if (addr > 0x7FFF) { return; }

  if (addr < 0x2000) {
    self->flags->set_ram_gate = true;
    self->flags->ram_gate_enabled = (data & 0xF) == 0b1010;
  } 
  else if (addr < 0x4000) {
    self->flags->set_switch_bank = true;
    uint8_t bank_rg_1 = data & 0x1F;
    bank_rg_1 = bank_rg_1 == 0 ? 1 : bank_rg_1;
    uint8_t bank_rg_2 = self->regs->bank2 & 0x3;
    self->flags->switch_bank = (bank_rg_2 << 5) | bank_rg_1;
  } 
  else if (addr < 0x6000) {
    self->flags->set_switch_bank = true;
    uint8_t bank_rg_2 = data & 0x3;
    uint8_t bank_rg_1 = self->regs->bank1 & 0x1F;
    self->flags->switch_bank = (bank_rg_2 << 5) | bank_rg_1;

    if (self->rom->mode == 1) {
      self->flags->set_fixed_bank = true;
      self->flags->fixed_bank = bank_rg_2 << 5;
    }
  } 
  else {
    self->rom->mode = data;
  }
}

void mbc_destroy(mbc_t* mbc) {
  free(mbc->regs);
  free(mbc);
}

mbc_t* mbc_create(rom_t* rom) {
  mbc_t* mbc = calloc(1, sizeof(mbc_t));
  mbc_regs_t* regs = calloc(1, sizeof(mbc_regs_t));
  mbc->regs = regs;
  mbc->rom = rom;

  if (rom->cart_type == MBC1) {
    mbc->regs->bank1 = 0b00001;
    mbc->intercept = &mbc1_intercept;
  }

  return mbc;
}

