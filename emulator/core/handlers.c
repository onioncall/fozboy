#include "handlers.h"

ERR_LD handle_load(core_t *core, instruction_meta_t *meta, uint8_t opcode, uint8_t opdata[]) {
  // Get value of second arg
  uint16_t addr1;
  uint16_t addr2;
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
    switch (meta->arg2_value) {
    case R16_AF:
      // This case isn't going to ever be hit by a LD 
      // but leaving in case I copy and paste this to a generic method
      addr2 = (core->cpu->a << 8) | core->cpu->f;
      break;
    case R16_BC:
      addr2 = (core->cpu->b << 8) | core->cpu->c;
      break;
    case R16_DE:
      addr2 = (core->cpu->d << 8) | core->cpu->e;
      break;
    case R16_HL:
      addr2 = (core->cpu->h <<8) | core->cpu->l;
      break;
    case R16_SP:
      addr2 = core->cpu->sp;

      if (opcode == 0xF8) {
        // Literally one opcode has 3 operands in Opcodes.json 
        // and doesn't fit the schema
        // I am not modifying the generation script and schema for this
        addr2 += (int8_t)opdata[0];
      }
      break;
    default:
      return ERR_LD_ARG2_VAL;
    }
    val = mmu_read(core->mmu, addr2);
    break;
  case ARG_A16:
    // basically what an ARG_N16_DREF would be
    val = mmu_read(core->mmu, (opdata[1] << 8) | opdata[0]);
    break;
  default: 
    return ERR_LD_ARG2_TYPE;
  }

  // Do the load
  switch (meta->arg1_type) {
  case ARG_R8:
    switch (meta->arg1_value) {
    case R8_A:
      core->cpu->a = val;
      break;
    case R8_B:
      core->cpu->b = val;
      break;
    case R8_C:
      core->cpu->c = val;
      break;
    case R8_D:
      core->cpu->d = val;
      break;
    case R8_E:
      core->cpu->e = val;
      break;
    case R8_H:
      core->cpu->h = val;
      break;
    case R8_L:
      core->cpu->l = val;
      break;

    default:
      return ERR_LD_ARG1_VAL;
    }
    break;
  case ARG_R16:
    switch (meta->arg1_value) {
    case R16_AF:
      core->cpu->a = val >> 8;
      core->cpu->f = val;
      break;
    case R16_BC:
      core->cpu->b = val >> 8;
      core->cpu->c = val;
      break;
    case R16_DE:
      core->cpu->d = val >> 8;
      core->cpu->e = val;
      break;
    case R16_HL:
      core->cpu->h = val >> 8;
      core->cpu->l = val;
      break;
    case R16_SP:
      core->cpu->sp = val;
      break;

    default: 
      return ERR_LD_ARG1_VAL;
    }
    break;

  case ARG_R16_DREF:
    switch (meta->arg1_value) {
    case R16_BC:
      addr1 = (core->cpu->b << 8) | core->cpu->c;
      break;
    case R16_DE:
      addr1 = (core->cpu->d << 8) | core->cpu->e;
      break;
    case R16_HL:
      addr1 = (core->cpu->h << 8) | core->cpu->l;
      break;

    default:
      return ERR_LD_ARG1_VAL;
    }
    mmu_write(core->mmu, addr1, val);
    break;

  case ARG_A8:
    addr1 = opdata[0] | 0xFF00;
    mmu_write(core->mmu, addr1, val);
    break;

  case ARG_A16:
    addr1 = opdata[0] | (opdata[1] << 8);
    mmu_write(core->mmu, addr1, val);
    break;

  default:
    return ERR_LD_ARG1_TYPE;
  }


  // TODO LDH?
  
  // LDI and LDD always inc or dec the HL register
  if (meta->op == OP_LDI) {
      uint16_t hl_reg = (core->cpu->h <<8) | core->cpu->l;
      ++hl_reg;
      core->cpu->h = hl_reg >> 8;
      core->cpu->l = hl_reg;
  }
  if (meta->op == OP_LDD) {
      uint16_t hl_reg = (core->cpu->h <<8) | core->cpu->l;
      --hl_reg;
      core->cpu->h = hl_reg >> 8;
      core->cpu->l = hl_reg;
  }
  


  //TODO do any loads set flags? the current instruction funcs don't set them

}
