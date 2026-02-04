
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
    flags.ram_gate_enabled = (data & 0xF) == 0xA;
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
  // Unsupported
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mmm01_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // Unsupported
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mbc3_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  intercept_flags_t flags = {0};
  if (addr > 0x7FFF) { return flags; }
  flags.mbc = true;

  mbc_regs_t* regs = self->regs;

  if (addr < 0x2000) {
    flags.set_ram_gate = true;
    flags.set_timer = true;
    flags.ram_gate_enabled = (data & 0xF) == 0xA;
    flags.timer_enabled = flags.ram_gate_enabled;
  } 
  else if (addr < 0x4000) {
    // single 7-bit register
    regs->bank1 = data & 0x7F;
    // MBC3 does not allow 0 in rom bank register
    regs->bank1 = regs->bank1 == 0 ? 1 : regs->bank1;

    flags.set_switch_bank = true;
    flags.switch_bank = regs->bank1;
  } 
  else if (addr < 0x6000) {
    // Ram bank number or RTC Register select 
    if (data <= 0x07) {

      regs->bank2 = data & 0x07;
      flags.set_ram_bank = true;
      flags.ram_bank = regs->bank2;

    } 
    else if (data <= 0x0C) {
      // RTC Register select 
      // 0x08 = RTC S (seconds)
      // 0x09 = RTC M (minutes) 
      // 0x0A = RTC H (hours)
      // 0x0B = RTC DL (day counter lower 8 bits)
      // 0x0C = RTC DH (day counter upper 1 bit + flags)
      regs->rtc_register = data;
    }
  } 
  else {
    // When writing $00, and then $01 to this register, 
    // the current time becomes latched into the RTC registers. 
    // The latched data will not change until it becomes latched again, 
    // by repeating the write $00->$01 procedure. 
    if (data == 0x01 && regs->latch_clock == 0x00) {
      regs->latch_clock = data;
      flags.latch_rtc = true;

    } 
    else if (data == 0x00) {
      regs->latch_clock = 0x00;

    } 
    else {
      regs->latch_clock = 0xFF; // reset latch sequence
    }
  }

  return flags;
}

intercept_flags_t mbc5_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  intercept_flags_t flags = {0};
  if (addr > 0x5FFF) { return flags; }
  flags.mbc = true;

  mbc_regs_t* regs = self->regs;

  if (addr < 0x2000) {
    regs->ramg = data;
    flags.set_ram_gate = true;
    flags.ram_gate_enabled = (data & 0xF) == 0xA;
  } 
  else if (addr < 0x3000) {
    regs->bank1 = data; // 8 bit register, no need to mask
    flags.set_switch_bank = true;
    flags.switch_bank = (regs->bank2 << 8) | regs->bank1;
  } 
  else if (addr < 0x4000) {
    regs->bank2 = data & 0x1;
    flags.set_switch_bank = true;
    flags.switch_bank = (regs->bank2 << 8) | regs->bank1;
  }
  else {
    regs->bank3 = data & 0xF;
    flags.set_ram_bank = true;
    flags.ram_bank = regs->bank3;
  }

  return flags;
}

intercept_flags_t mbc6_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // unsupported
  intercept_flags_t ret = {0};
  return ret;
}

intercept_flags_t mbc7_intercept(mbc_t* self, uint16_t addr, uint8_t data) {
  // unsupported
  intercept_flags_t ret = {0};
  return ret;
}

void mbc_destroy(mbc_t* mbc) {
  if (!mbc) return;
  free(mbc->regs);
  free(mbc);
}

mbc_t* mbc_create(rom_t* rom) {
  mbc_t* mbc = calloc(1, sizeof(mbc_t));
  mbc_regs_t* regs = calloc(1, sizeof(mbc_regs_t));
  mbc->regs = regs;
  mbc->rom = rom;

  mbc->regs->latch_clock = 0xFF; // Invalid state for latch sequence

  switch (rom->cart_type) {
  case ROM:
    mbc->intercept = &rom_intercept;
    break;
  case MBC1:
    mbc->regs->bank1 = 0b00001;
    mbc->intercept = &mbc1_intercept;
    break;
  case MBC2:
    mbc->intercept = &mbc2_intercept;
    break;
  case MBC3:
    mbc->regs->bank1 = 0b00001;
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

