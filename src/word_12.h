#ifndef CDC_160_WORD_12_H
#define CDC_160_WORD_12_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef int16_t Word12;

inline uint8_t Word12_IsZero(Word12 word) {
	return (word == 0 || word == 0xFFF /* -0 */);
}

inline Word12 U16_ToWord12(uint16_t u) {
	return (u) & 0xFFF;
}

inline Word12 BitwiseOR_Word12(Word12 wordA, Word12 wordB)
{
	return (wordA | wordB) & 0xFFF;
}
inline Word12 BitwiseXOR_Word12(Word12 wordA, Word12 wordB)
{
	return (wordA ^ wordB) & 0xFFF;
}

inline Word12 BitwiseNot_Word12(Word12 word) {
	return (~word) & 0xFFF;
}

inline Word12 Add_Word12(Word12 wordA, Word12 wordB)
{
	uint16_t sum = (wordA + wordB);
	if (sum & 0b0001000000000000) { // if carry out of bit 11 (leaked into bit 13)
		sum = (sum & 0xFFF) + 1; // end-around carry
	}

	if ((sum & 0xFFF) == 0xFFF) {
		return 0xFFF; // -0;
	}
	return sum & 0xFFF;
}

inline Word12 Sub_Word12(Word12 wordA, Word12 wordB)
{
	return Add_Word12(wordA, BitwiseNot_Word12(wordB));
}

inline Word12 ShiftLeft_Word12(Word12 word, Word12 shl)
{
	return (word << shl) & 0xFFF;
};

inline Word12 Mult_Word12(Word12 wordA, Word12 wordB) {
	return (wordA * wordB) & 0xFFF;

}

// indentical to Shift Right, except end around wrapping occurs.
inline Word12 RotateRight_Word12(Word12 word, Word12 rr) {
	rr &= 0xFFF;
	return ((word << rr) | (word >> (12 - rr))) & 0xFFF;
}

// indentical to Shift Left, except end around wrapping occurs.
inline Word12 RotateLeft_Word12(Word12 word, Word12 ll) {
	ll &= 0xFFF;
	return ((word << ll) | (word >> (12 - ll))) & 0xFFF;
}


inline Word12 ShiftRight_Word12(Word12 word, Word12 shr)
{
	return (word >> shr) & 0xFFF;
};

inline Word12 BitwiseAnd_Word12(Word12 wordA, Word12 wordB)
{
	return (wordA & wordB) & 0xFFF;
}


// returns 1 if A is greater than B, 0 if not
inline int8_t Greater_Word12(Word12 A, Word12 B) {
	return A > B;
}

// returns 1 if A is less than B, 0 if not
inline int8_t Less_Word12(Word12 A, Word12 B) {
	return A < B;
}


#include "twos_and_ones_complement_conversions.h"


inline void ToString_Word12(Word12 word, char* buff8Bytes) {

	uint16_t w = ToTwosComplement(word);
	char* c = buff8Bytes;
	memset(buff8Bytes, 0, 8);
	if (word & 0x800)// check sign (true is negative)
	{
		c[0] = '-';
		w *= -1; // abs
		//w--;
		w &= 0xFFF;
	}

	if (c[0] == '-') {
		if (w == 0xFFF) { // '-0'
			buff8Bytes[1] = '0';
		}
		else {
			snprintf(buff8Bytes + 1, 7, "%u", w);
		}
	}
	else
	{
		snprintf(buff8Bytes, 8, "%u", w);
	}
}


inline void ToBinary_Word12(Word12 word, char* buff13Bytes) {
	buff13Bytes[12] = '\0';
	// Check for each bit.
	int j = 0;
	for (int i = 11; i >= 0; i--) {

		// If i'th bit is set 
		if (word & (1 << (Word12)i))
		{
			buff13Bytes[j] = '1';
		}
		else {
			buff13Bytes[j] = '0';
		}
		j++;
	}
}
#endif