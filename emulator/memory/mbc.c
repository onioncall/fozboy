
#include <stdlib.h>
#include "mbc.h"
#include "rom.h"

intercept_flags_t mbc1_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  intercept_flags_t flags = {0};
  if (addr > 0x7FFF) { return flags; }
  flags.mbc = true;

  mbc_regs_t* regs = self->regs;

  if (addr < 0x2000) {
    flags.set_ram_gate = true;
    flags.ram_gate_enabled = (data & 0xF) == 0b1010;
  } 
  else if (addr < 0x4000) {
    // BANK1 register - 5 bit
    regs->bank1 = data & 0x1F;
    // MBC1 does not allow 0 in bank1 register and will treat 0 as 1
    regs->bank1 = regs->bank1 == 0 ? 1 : regs->bank1;

    flags.set_switch_bank = true;
    flags.switch_bank = (regs->bank2 << 5) | regs->bank1;
  } 
  else if (addr < 0x6000) {
    // BANK2 register - 2 bit
    regs->bank2 = data & 0x3;

    flags.set_switch_bank = true;
    flags.switch_bank = (regs->bank2 << 5) | regs->bank1;

    // fixed rom is switchable on bank2 register in mode 1
    if (self->regs->mode == 1) {
      flags.set_fixed_bank = true;
      flags.fixed_bank = regs->bank2 << 5;
    }
  } 
  else {
    self->regs->mode = data;

    // fixed rom is switchable on bank2 register in mode 1
    if (data == 1) {
      flags.set_fixed_bank = true;
      flags.fixed_bank = regs->bank2 << 5;
    }
  }

  return flags;
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

