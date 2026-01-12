#include "instructions.h"
#include <stdbool.h>
#include <stdint.h>

#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

// Comments in this file are going to be very stupid because I am not used to
// using bitwise operations, and because the SM83 CPU has a couple quirks

// Update Flags

// If the result of an instruction is zero, we set the zero flag
static void update_zero_flag(uint8_t *flags, uint8_t result) {
	if (result == 0) {
		*flags |= FLAG_Z;
	}
}

// If our instruction is a subtract, we set the negative flag.
static void update_negative_flag(uint8_t *flags) { *flags |= FLAG_N; }

// If there is an overflow with an arithmatic instruction, we set the carry
// flag. Claude tells me this is not the standard way to do this, but I'm
// choosing not to care because I find it easier to understand
static void update_carry_flag(uint8_t *flags, uint8_t dest, uint8_t result,
                              bool add) {
	// If we add and the result is greater than the original, no overflow
	// happened
	if (add && result >= dest) {
		return;
	}

	// If we subtract and the result is less than the original, no overflow
	// happened
	if (!add && result <= dest) {
		return;
	}

	// An overflow happened
	*flags |= FLAG_C;
}

// Half-carry is checking if only the lower four bits overflow. If they do, we
// set the half-carry flag
static void update_half_flag(uint8_t *flags, uint8_t dest, uint8_t val,
                             bool add) {
	if (add) {
		if (((dest & 0x0F) + (val & 0x0F)) > 0x0F) {
			*flags |= FLAG_H;
		}
	} else {
		if ((dest & 0x0F) < (val & 0x0F)) {
			*flags |= FLAG_H;
		}
	}
}

// Instructions

// Load instruction, sets flags ----
void execute_load(uint8_t *dest, uint8_t src) { *dest = src; };

// Add instruction, sets flags Z0HC
void execute_add(uint8_t *dest, uint8_t val, uint8_t *flags) {
	uint8_t result = *dest + val;

	*flags = 0;
	update_zero_flag(flags, result);
	update_half_flag(flags, *dest, val, true);
	update_carry_flag(flags, *dest, result, true);

	*dest = result;
}

// Subtract instruction, sets flags Z1HC
void execute_subtract(uint8_t *flags, uint8_t *dest, uint8_t val) {
	uint8_t result = *dest - val;

	*flags = 0;
	update_zero_flag(flags, result);
	update_negative_flag(flags);
	update_half_flag(flags, *dest, val, false);
	update_carry_flag(flags, *dest, result, false);

	*dest = result;
}

// Decrement instruction, sets flags Z1N-
// This was initially going to be part of the subtract instruction, but the
// flags work differently than they do for subtraction.
void execute_decr(uint8_t *flags, uint8_t *dest) {
	uint8_t result = *dest - 1;

	// We don't clear flags for this because we need to preserve the carry flag
	// for the decr operation. I don't know why the SM83 does this... But here
	// we are preserving the carry flag while wiping the rest of the flag data
	*flags &= FLAG_C;

	update_zero_flag(flags, result);
	update_negative_flag(flags);
	update_half_flag(flags, *dest, 1, false);

	*dest = result;
}

// Increment instruction, sets flags Z0H-
// This was initially going to be part of the add instruction, but the flags
// work differently than they do for addition.
void execute_incr(uint8_t *flags, uint8_t *dest) {
	uint8_t result = *dest + 1;

	// We don't clear flags for this because we need to preserve the carry flag
	// for the incr operation. I don't know why the SM83 does this... But here
	// we are preserving the carry flag while wiping the rest of the flag data

	*flags &= FLAG_C;
	update_zero_flag(flags, result);
	update_half_flag(flags, *dest, 1, false);

	*dest = result;
}

// AND instruction, sets flags Z010
void execute_and(uint8_t *flags, uint8_t *dest, uint8_t val) {
	uint8_t result = *dest & val;

	*flags = 0;
	update_zero_flag(flags, result);
	// A weird quirk of this CPU is that 'and' instructions set the half-carry
	// flag to 1, I haven't found a good explenation for this.
	*flags |= FLAG_H;

	*dest = result;
}

// OR instruction, sets flags Z000
void execute_or(uint8_t *flags, uint8_t *dest, uint8_t val) {
	uint8_t result = *dest | val;

	*flags = 0;
	update_zero_flag(flags, result);

	*dest = result;
}

// XOR instruction, sets flags Z000
void execute_xor(uint8_t *flags, uint8_t *dest, uint8_t val) {
	uint8_t result = *dest ^ val;

	*flags = 0;
	update_zero_flag(flags, result);

	*dest = result;
}

void execute_nop(void) {};
