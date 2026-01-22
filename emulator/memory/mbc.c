
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

intercept_flags_t rom_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // Nothing to do
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mbc2_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // TODO
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mmm01_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // TODO
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mbc3_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // TODO
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mbc5_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // TODO
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mbc6_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // TODO
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mbc7_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // TODO
  intercept_flags_t ret = {0};
  return ret;
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

  switch (rom->cart_type) {
  case ROM:
    mbc->intercept = &rom_intercept;
  case MBC1:
    mbc->regs->bank1 = 0b00001;
    mbc->intercept = &mbc1_intercept;
    break;
  case MBC2:
    mbc->intercept = &mbc2_intercept;
    break;
  case MBC3:
    mbc->intercept = &mbc3_intercept;
    break;
  case MBC5:
    mbc->intercept = &mbc5_intercept;
    break;
  case MBC6:
    mbc->intercept = &mbc6_intercept;
    break;
  case MBC7:
    mbc->intercept = &mbc7_intercept;
    break;
  case MMM01:
    mbc->intercept = &mmm01_intercept;
    break;
  default:
    // TODO return error?
    // Not going to support pocket_cam, bandai_tama5, huc3, huc1, at least for now
    break;
  }


  return mbc;
}

