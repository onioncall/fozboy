
#import "core.h"
#import "../cartridge/cart.h"
#import "../memory/mmu.h"
#import "../cpu/cpu.h"
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

ERR_CORE run() {

  while (1) {

    // Read next opcode
    // Get args based on opcode
    // get result of cpu instruction from instructions.c
    // Handle follow up implications from each instruction (ex mmu writes, jumps)
      // advance rom reader pointer to next line (unless instruction is a jump I guess)
      // is rom reader pointer one of the cpu registers?

    // Any other per-cycle handling ticks here
      // ex. apu, ppu, rtc
      // pass the cycle count to each one of these, 
      // because this loop is per-instruction, the amount of virtual cpu cycles can vary between loops
      
    // LAST - calculate time delta based on instruction's cycle count and wait

  }

  return NULL;
}
