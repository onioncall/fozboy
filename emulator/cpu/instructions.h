#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <stdbool.h>

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

// 8-bit arithmetic
void execute_add(uint8_t *dest, uint8_t val, uint8_t *flags);
void execute_adc(uint8_t *dest, uint8_t val, uint8_t *flags);
void execute_subtract(uint8_t *flags, uint8_t *dest, uint8_t val);
void execute_sbc(uint8_t *flags, uint8_t *dest, uint8_t val);
void execute_cp(uint8_t *flags, uint8_t dest, uint8_t val);
void execute_incr(uint8_t *flags, uint8_t *dest);
void execute_decr(uint8_t *flags, uint8_t *dest);

// 16-bit arithmetic
void execute_add_16(uint16_t *dest, uint16_t val, uint8_t *flags);
void execute_incr_16(uint16_t *dest);
void execute_decr_16(uint16_t *dest);

// Logic operations
void execute_and(uint8_t *flags, uint8_t *dest, uint8_t val);
void execute_or(uint8_t *flags, uint8_t *dest, uint8_t val);
void execute_xor(uint8_t *flags, uint8_t *dest, uint8_t val);

// Flag manipulation
void execute_cpl(uint8_t *dest, uint8_t *flags);
void execute_ccf(uint8_t *flags);
void execute_scf(uint8_t *flags);

// Rotation operations
void execute_rlc(uint8_t *dest, uint8_t *flags);
void execute_rl(uint8_t *dest, uint8_t *flags);
void execute_rrc(uint8_t *dest, uint8_t *flags);
void execute_rr(uint8_t *dest, uint8_t *flags);

// Data transfer
void execute_load(uint8_t *dest, uint8_t src);

// Misc
void execute_nop(void);

#endif
