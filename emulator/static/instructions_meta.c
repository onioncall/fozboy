#include "instructions_meta.h"

const instruction_meta_t INSTRUCTIONS[256] = {
  [0x00] = { 1, 4 },
  [0x88] = { 1, 4, IAT_R8, R8_A, IAT_R8, R8_PARSE },
};
