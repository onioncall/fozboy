#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "../memory/mmu.h"

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

// Data Transfer

// Load 8-bit value from src to dest register
void execute_load(uint8_t *dest, uint8_t src);

// Load 16-bit value from src to dest register
void execute_ld_16(uint16_t *dest, uint16_t src);

// 8-bit Arithmetic

// Add value to dest register
void execute_add(uint8_t *dest, uint8_t val, uint8_t *flags);

// Add value plus carry flag to dest register
void execute_adc(uint8_t *dest, uint8_t val, uint8_t *flags);

// Subtract value from dest register
void execute_subtract(uint8_t *flags, uint8_t *dest, uint8_t val);

// Subtract value and carry flag from dest register
void execute_sbc(uint8_t *flags, uint8_t *dest, uint8_t val);

// Compare dest with val (sets flags but doesn't store result)
void execute_cp(uint8_t *flags, uint8_t dest, uint8_t val);

// Increment 8-bit register
void execute_incr(uint8_t *flags, uint8_t *dest);

// Decrement 8-bit register
void execute_decr(uint8_t *flags, uint8_t *dest);

// 16-bit Arithmetic

// Add 16-bit value to dest register
void execute_add_16(uint16_t *dest, uint16_t val, uint8_t *flags);

// Increment 16-bit register
void execute_incr_16(uint16_t *dest);

// Decrement 16-bit register
void execute_decr_16(uint16_t *dest);

// Logic Operations

// Bitwise AND dest with val
void execute_and(uint8_t *flags, uint8_t *dest, uint8_t val);

// Bitwise OR dest with val
void execute_or(uint8_t *flags, uint8_t *dest, uint8_t val);

// Bitwise XOR dest with val
void execute_xor(uint8_t *flags, uint8_t *dest, uint8_t val);

// Flag Manipulation

// Complement (invert all bits) of dest register
void execute_cpl(uint8_t *dest, uint8_t *flags);

// Complement (flip) carry flag
void execute_ccf(uint8_t *flags);

// Set carry flag to 1
void execute_scf(uint8_t *flags);

// Rotation Operations

// Rotate left circular (bit 7 wraps to bit 0)
void execute_rlc(uint8_t *dest, uint8_t *flags, bool update_z);

// Rotate left through carry
void execute_rl(uint8_t *dest, uint8_t *flags, bool update_z);

// Rotate right circular (bit 0 wraps to bit 7)
void execute_rrc(uint8_t *dest, uint8_t *flags, bool update_z);

// Rotate right through carry
void execute_rr(uint8_t *dest, uint8_t *flags, bool update_z);

// Shift Operations

// Shift left arithmetic (bit 7 to carry, 0 to bit 0)
void execute_sla(uint8_t *dest, uint8_t *flags);

// Shift right arithmetic (bit 0 to carry, bit 7 preserved)
void execute_sra(uint8_t *dest, uint8_t *flags);

// Shift right logical (bit 0 to carry, 0 to bit 7)
void execute_srl(uint8_t *dest, uint8_t *flags);

// Nibble Swap

// Swap upper and lower nibbles
void execute_swap(uint8_t *dest, uint8_t *flags);

// Bit Operations

// Test bit in value (sets Z flag if bit is 0)
void execute_bit(uint8_t bit_num, uint8_t val, uint8_t *flags);

// Set bit to 1
void execute_set(uint8_t bit_num, uint8_t *dest);

// Reset bit to 0
void execute_res(uint8_t bit_num, uint8_t *dest);

// Control Flow

// Jump to 16-bit address
void execute_jp(uint16_t *pc, uint16_t address);

// Jump relative by signed offset
void execute_jr(uint16_t *pc, int8_t offset);

// CPU Control

// Disable interrupts
void execute_di(bool *ime);

// Enable interrupts
void execute_ei(bool *ime);

// Halt CPU until interrupt
void execute_halt(bool *halt);

// No operation
void execute_nop(void);

// Memory Operations

// Load from memory address into dest register
void execute_ld_mem(uint8_t *dest, uint16_t address, mmu_t *mmu);

// Store value to memory address
void execute_st_mem(uint8_t value, uint16_t address, mmu_t *mmu);

// Load from memory then increment address
void execute_ldi_mem(uint8_t *dest, uint16_t *address, mmu_t *mmu);

// Load from memory then decrement address
void execute_ldd_mem(uint8_t *dest, uint16_t *address, mmu_t *mmu);

// Store to memory then increment address
void execute_sti_mem(uint8_t value, uint16_t *address, mmu_t *mmu);

// Store to memory then decrement address
void execute_std_mem(uint8_t value, uint16_t *address, mmu_t *mmu);

// Stack Operations

// Push 16-bit value onto stack
void execute_push(uint16_t *value, uint16_t *sp_addr, mmu_t *mmu);

// Pop 16-bit value from stack
void execute_pop(uint16_t *value, uint16_t *sp_addr, mmu_t *mmu);

// Call subroutine (push PC, jump to address)
void execute_call(uint16_t *pc, uint16_t *sp, uint16_t address, mmu_t *mmu);

// Return from interrupt (pop PC, enable interrupts)
void execute_reti(uint16_t *pc, uint16_t *sp, bool *ime, mmu_t *mmu);

// Special

// Decimal adjust accumulator for BCD arithmetic
void execute_daa(uint8_t *a, uint8_t *flags);

#endif
