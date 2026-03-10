#ifndef INSTRUCTIONS_META_H
#define INSTRUCTIONS_META_H

#include <stdint.h>

typedef enum {
  OP_NOP = 0, // NOOP
  OP_ADC, // Add value plus carry flag to A
  OP_ADD, // Add value
  OP_LD, // Load
  OP_LDI, // Load and increment
  OP_LDD, // Load and decrement
  OP_INC, // Increment
  OP_DEC, // Decrement
} op_type_enum;

typedef enum {
  ARG_NONE = 0, // default for args not defined/used
  ARG_R8, // 8-bit cpu register
  ARG_R8_DREF, // 8-bit cpu register as mem address
  ARG_R16, // 16-bit cpu register
  ARG_R16_DREF, // 16-bit cpu register as mem address
  ARG_N8, // 8-bit immediate value
  ARG_N16, // 16-bit immediate value
  ARG_A8, // 8-bit mem address (based on 0xFF00)
  ARG_A16, // 16-bit mem address
  ARG_E8, // 8-bit signed offset
  ARG_U3, // 3-bit unsigned bit index
  ARG_CC, // condition code (Z, NZ, C, NC)
  ARG_VEC // an RST vector (0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38)
} arg_type_enum;

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
  R16_AF,
  R16_BC,
  R16_DE,
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
  op_type_enum op; // Instruction type
  arg_type_enum arg1_type;
  uint8_t arg1_value; // Only present if defined statically in the instruction
  arg_type_enum arg2_type;
  uint8_t arg2_value; // Only present if defined statically in the instruction
} instruction_meta_t;

#endif
