// Memory Mapping

#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint8_t* buf;
  uint16_t len; // 1-indexed
  uint16_t start;
  uint16_t end;
} block_t;

typedef struct {
  block_t rom_fixed;
  block_t rom_switch;
  block_t v_ram;
  block_t ext_ram;
  block_t w_ram;
  block_t w_ram_switch;
  block_t echo_ram;
  block_t oam;
  block_t unusable;
  block_t io_regs;
  block_t h_ram;
  block_t int_enable;
} mmu_t;

block_t new_block(uint16_t start, uint16_t end) {
  uint16_t len = end - start + 1;
  uint8_t* buf = calloc(len, sizeof(uint8_t));

  block_t block = {
    .len = len,
    .start = start,
    .end = end,
    .buf = buf
  };

  return block;
}

void block_deinit(block_t block) {
  free(block.buf);
}

mmu_t new_mmu() {
  block_t rom_fixed = new_block(0x0000, 0x3FFF);
  block_t rom_switch = new_block(0x4000, 0x7FFF);
  block_t v_ram = new_block(0x8000, 0x9FFF);
  block_t ext_ram = new_block(0xA000, 0xBFFF);
  block_t w_ram = new_block(0xC000, 0xCFFF);
  block_t w_ram_switch = new_block(0xD000, 0xDFFF);
  block_t echo_ram = new_block(0xE000, 0xFDFF);
  block_t oam = new_block(0xFE00, 0xFE9F);
  block_t unusable = new_block(0xFEA0, 0xFEFF);
  block_t io_regs = new_block(0xFF00, 0xFF7F);
  block_t h_ram = new_block(0xFF80, 0xFFFE);
  block_t int_enable = new_block(0xFFFF, 0xFFFF);

  mmu_t mmu = {
    .rom_fixed = rom_fixed,
    .rom_switch = rom_switch,
    .v_ram = v_ram,
    .ext_ram = ext_ram,
    .w_ram = w_ram,
    .w_ram_switch = w_ram_switch,
    .oam = oam,
    .unusable = unusable,
    .io_regs = io_regs,
    .h_ram = h_ram,
    .int_enable = int_enable,
  };

  return mmu;
}

void mmu_deinit(mmu_t mmu) {
  block_deinit(mmu.rom_fixed);
  block_deinit(mmu.rom_switch);
  block_deinit(mmu.v_ram);
  block_deinit(mmu.ext_ram);
  block_deinit(mmu.w_ram);
  block_deinit(mmu.w_ram_switch);
  block_deinit(mmu.echo_ram);
  block_deinit(mmu.oam);
  block_deinit(mmu.unusable);
  block_deinit(mmu.io_regs);
  block_deinit(mmu.h_ram);
  block_deinit(mmu.int_enable);
}
