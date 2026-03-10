#include "instructions_meta.h"

const instruction_meta_t INSTRUCTIONS[256] = {
  [0x00] = { 1, 4, OP_NOP },
  [0x01] = { 3, 12, OP_LD, ARG_R16, R16_BC, ARG_N16, 0 },
  [0x02] = { 1, 8, OP_LD, ARG_R16_DREF, R16_BC, ARG_R8, R8_A },
  [0x03] = { 1, 8, OP_INC, ARG_R16, R16_BC },
  [0x04] = { 1, 4, OP_INC, ARG_R8, R8_B },
  [0x05] = { 1, 4, OP_DEC, ARG_R8, R8_B },

  [0x06] = { 2, 8, OP_LD, ARG_R8, R8_B, ARG_N8 },

  [0x88] = { 1, 4, OP_ADC, ARG_R8, R8_A, ARG_R8, R8_PARSE },
};
