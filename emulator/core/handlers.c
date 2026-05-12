#include "handlers.h"

typedef uint8_t FLAG_BIT;
const FLAG_BIT FLAG_Z = 0x80;
const FLAG_BIT FLAG_N = 0x40;
const FLAG_BIT FLAG_H = 0x20;
const FLAG_BIT FLAG_C = 0x10;

int get_r16(core_t *core, cpu_registers_16bit_enum reg, uint16_t *buf) {
  switch (reg) {
  case R16_AF:
    // This case isn't going to ever be hit by a LD 
    // but leaving in case I copy and paste this to a generic method
    *buf = (core->cpu->a << 8) | core->cpu->f;
    break;
  case R16_BC:
    *buf = (core->cpu->b << 8) | core->cpu->c;
    break;
  case R16_DE:
    *buf = (core->cpu->d << 8) | core->cpu->e;
    break;
  case R16_HL:
    *buf = (core->cpu->h <<8) | core->cpu->l;
    break;
  case R16_SP:
    *buf = core->cpu->sp;
    break;

  default:
    return 1;
  }
  return 0;
}

void set_flag(core_t *core, FLAG_BIT flag) {
  core->cpu->f |= flag;
}

ERR_LD handle_load(core_t *core, instruction_meta_t *meta, uint8_t opcode, uint8_t opdata[]) {
  uint16_t addr1;
  uint16_t addr2;
  uint16_t val;
  
  // Get value of second arg
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

  case ARG_R8_DREF:
    // ARG_R8_DREF is only used for OP_LDH and only ever with R8_C
    val = mmu_read(core->mmu, 0xFF00 | core->cpu->c);
    break;

  case ARG_R16_DREF:

    if (get_r16(core, meta->arg2_value, &addr2) != 0) {
      return ERR_LD_ARG2_VAL;
    }

    val = mmu_read(core->mmu, addr2);
    break;

  case ARG_R16:

    if (get_r16(core, meta->arg2_value, &val) != 0) {
      return ERR_LD_ARG2_VAL;
    }
    if (opcode == 0xF8) {
      // Literally one opcode has 3 operands in Opcodes.json 
      // and doesn't fit the schema
      // I am not modifying the generation script and schema for this
      val += (int8_t)opdata[0];

      // It's also the only load that sets flags
      // Hate this opcode
      core->cpu->f = 0;
      if ((core->cpu->sp & 0xF) + (opdata[0] & 0xF) > 0xF) {
        set_flag(core, FLAG_H);
      }
      if ((core->cpu->sp & 0xFF) + (opdata[0] & 0xFF) > 0xFF) {
        set_flag(core, FLAG_C);
      }
    }

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

  case ARG_R8_DREF:
    // ARG_R8_DREF is only used for OP_LDH and only ever with R8_C
    mmu_write(core->mmu, 0xFF00 | core->cpu->c, val);
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
    mmu_write(core->mmu, addr1, val & 0xFF);
    mmu_write(core->mmu, addr1 + 1, val >> 8);
    break;

  default:
    return ERR_LD_ARG1_TYPE;
  }

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
  
  return ERR_LD_OK;
}

ERR_INC handle_inc(core_t *core, instruction_meta_t *meta) {
  switch (meta->arg1_type) {
  case ARG_R8:
    switch (meta->arg1_value) {
    case R8_A:
      break;

    case R8_B:
      break;

    case R8_C:
      break;

    case R8_D:
      break;

    case R8_E:
      break;

    }

    break;

  case ARG_R16:

    break;

  case ARG_R16_DREF:

    break;
  }  
  return ERR_LD_OK;
}
