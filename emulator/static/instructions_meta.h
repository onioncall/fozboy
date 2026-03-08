#ifndef INSTRUCTIONS_META_H
#define INSTRUCTIONS_META_H

#include <stdint.h>

typedef enum {
  IST_NOP = 0, // NOOP
  IST_ADC, // Add value plus carry flag to A
  IST_ADD // Add value
} instruction_type_enum;

typedef enum {
  IAT_NONE = 0, // default for args not defined/used
  IAT_R8, // 8-bit cpu register
  IAT_R16, // 16-bit cpu register
  IAT_N8, // 8-bit immediate value
  IAT_N16, // 16-bit immediate value
  IAT_E8, // 8-bit signed offset
  IAT_U3, // 3-bit unsigned bit index
  IAT_CC, // condition code (Z, NZ, C, NC)
  IAT_VEC // an RST vector (0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38)
} instruction_arg_type_enum;

// Not actually sure if we will be using these enums
// just creating an ordered map of numbers to values
typedef enum {
  R8_PARSE = 0, // get the value from the byte(s) passed to the instruction
  R8_A,
  R8_B,
  R8_C,
  R8_D,
  R8_E
} cpu_registers_8bit_enum;

typedef enum {
  R16_PARSE = 0, // get the value from the byte(s) passed to the instruction
  R16_HL,
  R16_SP,
  R16_PC
} cpu_registers_16bit_enum;

typedef enum {
  CC_Z = 0,
  CC_NZ,
  CC_C, 
  CC_NC
} condition_code_enum;

typedef struct {
  uint8_t bytes; // including opcode byte
  uint8_t cycles; // T-Cycles (NOT M-cycles)
  instruction_arg_type_enum arg1_type;
  uint8_t arg1_value; // Only present if defined statically in the instruction
  instruction_arg_type_enum arg2_type;
  uint8_t arg2_value; // Only present if defined statically in the instruction
} instruction_meta_t;

#endif
