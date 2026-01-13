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
// choosing not to care because I find it easier to understand. These could be 8
// bit, but then we need an identical function that takes 16 bit parameters for
// the add_16 instruction, and it just doesn't feel necessary
static void update_carry_flag(uint8_t *flags, uint16_t dest, uint16_t result,
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
// set the half-carry flag. There are special instructions that we also have
// context of the carry value, which for some reason is called the nibble
static void update_half_flag(uint8_t *flags, uint8_t dest, uint8_t val,
                             uint8_t nib, bool add) {
	if (add) {
		if (((dest & 0x0F) + (val & 0x0F) + (nib & 0x0F)) > 0x0F) {
			*flags |= FLAG_H;
		}
	} else {
		if ((dest & 0x0F) < ((val & 0x0F) + (nib & 0x0F))) {
			*flags |= FLAG_H;
		}
	}
}

// We do need a 16 bit version of this, since we are relying on the bit size to
// detect lower half overflows
static void update_half_flag_16(uint8_t *flags, uint16_t dest, uint16_t val,
                                uint8_t nib, bool add) {
	if (add) {
		if (((dest & 0x0FFF) + (val & 0x0FFF) + (nib & 0x0FFF)) > 0x0FFF) {
			*flags |= FLAG_H;
		}
	} else {
		if ((dest & 0x0FFF) < ((val & 0x0FFF) + (nib & 0x0FFF))) {
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
	update_half_flag(flags, *dest, val, 0, true);
	update_carry_flag(flags, *dest, result, true);

	*dest = result;
}

// 16 bit add instruction, sets flags -0HC
void execute_add_16(uint16_t *dest, uint16_t val, uint8_t *flags) {
	uint16_t result = *dest + val;

	*flags &= FLAG_Z;
	update_half_flag_16(flags, *dest, val, 0, true);
	update_carry_flag(flags, *dest, result, true);

	*dest = result;
}

// Add-carry instruction, sets flags Z0HC. This adds the carry value as well
void execute_adc(uint8_t *dest, uint8_t val, uint8_t *flags) {
	uint8_t carry_value = (*flags & FLAG_C) ? 1 : 0;
	uint8_t result = *dest + val + carry_value;

	*flags = 0;
	update_zero_flag(flags, result);
	update_half_flag(flags, *dest, val, carry_value, true);
	update_carry_flag(flags, *dest, result, true);

	*dest = result;
}

// Subtract instruction, sets flags Z1HC
void execute_subtract(uint8_t *flags, uint8_t *dest, uint8_t val) {
	uint8_t result = *dest - val;

	*flags = 0;
	update_zero_flag(flags, result);
	update_negative_flag(flags);
	update_half_flag(flags, *dest, val, 0, false);
	update_carry_flag(flags, *dest, result, false);

	*dest = result;
}

// Subtract-carry instruction, sets flags Z1HC
void execute_sbc(uint8_t *flags, uint8_t *dest, uint8_t val) {
	uint8_t carry_value = (*flags & FLAG_C) ? 1 : 0;
	uint8_t result = *dest - val - carry_value;

	*flags = 0;
	update_zero_flag(flags, result);
	update_negative_flag(flags);
	update_half_flag(flags, *dest, val, carry_value, false);
	update_carry_flag(flags, *dest, result, false);

	*dest = result;
}

// Compare instruction, sets flags Z1HC but does not save the value to any
// register
void execute_cp(uint8_t *flags, uint8_t dest, uint8_t val) {
	uint8_t result = dest - val;

	*flags = 0;
	update_zero_flag(flags, result);
	update_negative_flag(flags);
	update_half_flag(flags, dest, val, 0, false);
	update_carry_flag(flags, dest, result, false);
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
	update_half_flag(flags, *dest, 1, 0, false);

	*dest = result;
}

// 16 bit decriment instruction, sets flags ----
void execute_decr_16(uint16_t *dest) { *dest -= 1; }

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
	update_half_flag(flags, *dest, 1, 0, true);

	*dest = result;
}

// 16 bit increment instruction, sets flags ----
void execute_incr_16(uint16_t *dest) { *dest += 1; }

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

// Compliment instruction, sets flags -11-
void execute_cpl(uint8_t *dest, uint8_t *flags) {
	*dest = ~*dest;

	*flags |= FLAG_N;
	*flags |= FLAG_H;
}

// Compliment-carry flag instruction, sets flags -00f
void execute_ccf(uint8_t *flags) {
	*flags &= ~FLAG_N;
	*flags &= ~FLAG_H;
	*flags ^= FLAG_C;
}

// Set-carry flag instruction, sets flags -001
void execute_scf(uint8_t *flags) {
	*flags &= ~FLAG_N;
	*flags &= ~FLAG_H;
	*flags |= FLAG_C;
}

// Rotate-left-carry instruction, sets flags z00c
void execute_rlc(uint8_t *dest, uint8_t *flags) {
	// The & 1 here is ensuring that we have a 0 or a 1. Not sure why this is
	// convention. We are moving bit 7 to bit 1, and saving the value.
	uint8_t bit7 = (*dest >> 7) & 1;
	// move all bits over by 1 and then assign them to our register
	*dest <<= 1;
	// set the value of bit 1 to bit 7 to complete the rotation
	// normally we do this after all other updates, but in this case, the update
	// is what we are evaluating for our zero flag
	*dest |= bit7;

	*flags = 0;
	update_zero_flag(flags, *dest);
	// if bit seven has a value, we want to set the carry flag
	if (bit7) {
		*flags |= FLAG_C;
	}
}

// Rotate-left instruction, sets flags z00c
// Old Carry value gets set to bit0
void execute_rl(uint8_t *dest, uint8_t *flags) {
	uint8_t bit7 = (*dest >> 7) & 1;
	*dest <<= 1;
	uint8_t carry_value = (*flags & FLAG_C) ? 1 : 0;
	*dest |= carry_value;

	*flags = 0;
	update_zero_flag(flags, *dest);
	// if bit seven has a value, we want to set the carry flag
	if (bit7) {
		*flags |= FLAG_C;
	}
}

// Rotate-right-carry instruction, sets flags z00c
void execute_rrc(uint8_t *dest, uint8_t *flags) {
	uint8_t bit0 = *dest & 1;
	*dest >>= 1;
	*dest |= (bit0 << 7);

	*flags = 0;
	update_zero_flag(flags, *dest);
	if (bit0) {
		*flags |= FLAG_C;
	}
}

// Rotate-right instruction, sets flags z00c
// Old Carry value gets set to bit7
void execute_rr(uint8_t *dest, uint8_t *flags) {
	uint8_t bit0 = *dest & 1;
	*dest >>= 1;
	uint8_t carry_value = (*flags & FLAG_C) ? 1 : 0;
	*dest |= (carry_value << 7);

	*flags = 0;
	update_zero_flag(flags, *dest);
	if (bit0) {
		*flags |= FLAG_C;
	}
}

void execute_nop(void) {};
