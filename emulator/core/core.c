
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
    // Probably move this to its own method once it gets bigger 
    switch (meta->op) {
    case OP_NOP:
      break;
    case OP_LD:
      // Get value of second arg
      uint16_t val;
      switch (meta->arg2_type) {
      case ARG_R8:
        val = meta->arg2_value;
        break;
      case ARG_N8:
        val = opdata[0];
        break;
      case ARG_N16:
        // Read little endian opdata
        val = (opdata[1] << 8) | opdata[0];
        break;
      case ARG_R16_DREF:
        
        // Probably extract this to a method
        uint16_t addr;
        switch (meta->arg2_value) {
        case R16_AF:
          // This case isn't going to ever be hit by a LD 
          // but leaving in case I copy and paste this to a generic method
          addr = (core->cpu->a << 8) | core->cpu->f;
          break;
        case R16_BC:
          addr = (core->cpu->b << 8) | core->cpu->c;
          break;
        case R16_DE:
          addr = (core->cpu->d << 8) | core->cpu->e;
          break;
        case R16_HL:
          addr = (core->cpu->h <<8) | core->cpu->l;
          break;
        case R16_SP:
          addr = core->cpu->sp;

          if (opcode == 0xF8) {
            // Literally one opcode has 3 operands in Opcodes.json 
            // and doesn't fit the schema
            // I am not modifying the generation script and schema for this
            addr += (int8_t)opdata[0];
          }
          break;
        }
        val = mmu_read(core->mmu, addr);
        break;
      case ARG_A16:
        // basically what an ARG_N16_DREF would be
        val = mmu_read(core->mmu, (opdata[1] << 8) | opdata[0]);
        break;
      default: 
        // TODO Do an error here
      }
      
      // TODO Now finish the load instruction
      // Might just be able to call the existing cpu methods from here

      break;
    }

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
