#ifndef MBC_H
#define MBC_H

#include <stdint.h>
#include "rom.h"

// These flags will be set by the mbc intercept functions
// They are effectively commands for the caller
typedef struct {
  bool mbc; // Tells the caller that this is an mbc action, so skip writing
  bool set_switch_bank;
  uint16_t switch_bank;
  bool set_fixed_bank;
  uint16_t fixed_bank;
  bool set_ram_gate;
  bool ram_gate_enabled;
  bool set_timer;
  bool timer_enabled;
} intercept_flags_t;

// These are the hardware registers for the mbc, 
// unlike flags, these will persist across virtual clock cycles.
typedef struct {
  uint8_t ramg;
  uint8_t bank1;
  uint8_t bank2;
  uint8_t mode;
} mbc_regs_t;

typedef struct mbc_t {
  mbc_regs_t* regs;
  rom_t* rom;
  intercept_flags_t(*intercept)(struct mbc_t* self, uint16_t addr, uint8_t data);
} mbc_t;

mbc_t* mbc_create(rom_t* rom);
void mbc_destroy(mbc_t* mbc);

#endif
