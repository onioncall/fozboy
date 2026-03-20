
#include "core.h"
#include "../cartridge/cart.h"
#include "../cartridge/ext_ram.h"
#include "../static/instructions_meta.h"
#include "../memory/mmu.h"
#include "../cpu/cpu.h"
#include <stdlib.h>

void core_destroy(core_t *core) {
  if (core == NULL) {
    return;
  }

  if (core->cart != NULL) {
    cart_destroy(core->cart);
  }
  if (core->mmu != NULL) {
    mmu_destroy(core->mmu);
  }
  if (core->cart != NULL) {
    free(core->cpu);
  }

  free(core);
}

core_t* core_create(char *file_name) {
  core_t *core = malloc(sizeof(core_t));
  if (core == NULL) {
    return NULL;
  }

  core->cart = cart_create(file_name);
  if (!core->cart) { 
    goto cleanup;
  }
  core->mmu = mmu_create(core->cart);
  if (!core->mmu) { 
    goto cleanup;
  }
  core->cpu = malloc(sizeof(Cpu));
  if (!core->cpu) { 
    goto cleanup;
  }
  cpu_init(core->cpu);

  return core;

cleanup:
  core_destroy(core);
  return NULL;
}

ERR_CORE run(core_t* core) {
  uint8_t opcode;
  uint8_t opdata[2];

  while (1) {

    // Read next opcode
    opcode = mmu_read(core->mmu, core->cpu->pc);

    // Get args based on opcode
    const instruction_meta_t *meta = &INS_UNPREFIXED[opcode];
    if (meta->op == OP_PREFIX) {
      meta = &INS_CBPREFIXED[mmu_read(core->mmu, ++core->cpu->pc)];
    }

    if (meta->bytes > 1) opdata[0] = mmu_read(core->mmu, core->cpu->pc + 1);
    if (meta->bytes > 2) opdata[1] = mmu_read(core->mmu, core->cpu->pc + 2);

    // Advance program counter to next op
    core->cpu->pc += meta->bytes;

    // Handle follow up implications from each instruction (ex mmu writes, jumps)

    // Any other per-cycle handling ticks here
      // ex. apu, ppu, rtc
      // pass the cycle count to each one of these, 
      // because this loop is per-instruction, the amount of virtual cpu cycles can vary between loops

    snapshot_ram_throttled(core->cart->ext_ram);

    // LAST - Accumulate cycle count
    // LAST - periodically calculate time delta based on instruction's cycle count and wait
    // do this every scanline (456 t-cycle) 
    // or every frame (456 * 154 scanlines = 70224 cycles = 1 frame) 
    // note - GB is 4194304 t-cycles/second
  }

  return ERR_CORE_OK;
}
