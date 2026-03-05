
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

  return NULL;
}
