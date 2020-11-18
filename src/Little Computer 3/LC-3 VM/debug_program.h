#pragma once

#include <iostream>

uint16_t setinst(uint16_t val)
{
	return (val >> 8) | (val << 8);
}

uint16_t addout[] =
{
	// .ORIGIN x3000
	setinst(0x3000),
	// ---

	// ADD R0, R0, #15
	setinst(0b0001000000101111),
	// OUT
	setinst(0b1111000000100001),

	// ---
	// HALT
	setinst(0b1111000000100101),
	// .END
	setinst(0b0000000000000000)
};

uint16_t debug_program[] =
{
	// .ORIGIN x3000
	setinst(0x3000),
	// ---

	// ADD R0, R0, #15
	setinst(0b0001000000101111),
	// OUT
	setinst(0b1111000000100001),
	// AND R6, R6, #0
	setinst(0b0101110110100000),
	// ADD R6, R6, #3
	setinst(0b0001110110100011),
	// GETC
	setinst(0b1111000000100000),
	// OUT
	setinst(0b1111000000100001),
	// GETC
	//setinst(0b1111000000100000),
	// OUT
	//setinst(0b1111000000100001),
	// GETC
	//setinst(0b1111000000100000),
	// OUT
	//setinst(0b1111000000100001),
	// ADD R6, R6, #-1
	setinst(0b0001110110111111),
	// BRP
	setinst(0b0000001111111100),


	// ---
	// HALT
	setinst(0b1111000000100101),
	// .END
	setinst(0b0000000000000000)
};
