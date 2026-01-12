#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <stdbool.h>

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

void execute_load(uint8_t *dest, uint8_t src);
void execute_add(uint8_t *dest, uint8_t val, uint8_t *flags);
void execute_subtract(uint8_t *flags, uint8_t *dest, uint8_t val);
void execute_decr(uint8_t *flags, uint8_t *dest);
void execute_incr(uint8_t *flags, uint8_t *dest);
void execute_and(uint8_t *flags, uint8_t *dest, uint8_t val);
void execute_or(uint8_t *flags, uint8_t *dest, uint8_t val);
void execute_xor(uint8_t *flags, uint8_t *dest, uint8_t val);
void execute_nop(void);

#endif
