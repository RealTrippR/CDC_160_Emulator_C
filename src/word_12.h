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


// see page 11: http://www.bitsavers.org/pdf/cdc/160/CDC160A/60014500G_CDC160A_Reference_Manual_196503.pdf
/* (yes, I know that manual is for the CDC-160A, not the CDC-160,
but the arithmetic is the same for both machines.
See page 33: https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf)

The internal arithmetic of the computer is based on subtraction. Addition is
performed by subtracting the complement of the addend from the augend.
In subtraction no complementing is necessary.
*/
inline Word12 Sub_Word12(Word12 A, Word12 B)
{
	/*
	Word12 res = 0x0;
	Word12 onesComplementOfB = BitwiseNot_Word12(B);
	// do it bit by bit.
	for (uint8_t i = 0; i < 12; ++i)
	{
		Word12 bitmask = 1 << i;
		if ((A & bitmask) != 0 && (B & bitmask) != 0)
		{
			res |= (bitmask << 1);
		}
		else if (A & bitmask || B & bitmask) {
			res |= bitmask;
		}
	}
	return res & 0xFFF;*/

	Word12 result = 0;
	Word12 borrow = 0;

	for (int i = 0; i < 12; ++i) {
		Word12 a_bit = (A >> i) & 1;
		Word12 b_bit = (B >> i) & 1;

		Word12 sub = a_bit - b_bit - borrow;

		if (sub & 0x800) { // result is negative (since a < b + borrow)
			sub += 2;     // +2 because it's in mod 2, so -1 becomes 1
			borrow = 1;
		}
		else {
			borrow = 0;
		}

		result |= (sub << i);
	}

	return result & 0xFFF;
}

inline Word12 Add_Word12(Word12 wordA, Word12 wordB)
{
	/*
	The internal arithmetic of the computer is based on subtraction. Addition is
	performed by subtracting the complement of the addend from the augend. 
	In subtraction no complementing is necessary. 
	*/
	return Sub_Word12(wordA/*augend*/, BitwiseNot_Word12(wordB)/*addend*/);
	/*
	uint16_t sum = (wordA + wordB);
	if (sum & 0b0001000000000000) { // if carry out of bit 11 (leaked into bit 13)
		sum = (sum & 0xFFF) + 1; // end-around carry
	}

	if ((sum & 0xFFF) == 0xFFF) {
		return 0xFFF; // -0;
	}
	return sum & 0xFFF;*/
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

inline void Word12ToStrBinary(Word12 word, char* buff13Bytes) 
{
	buff13Bytes[12] = '\0'; // null terminate
	for (uint8_t i = 0; i < 12; ++i) 
	{
		Word12 bitmask = 1 << i;

		if (word & bitmask)
		{
			buff13Bytes[11-i] = '1';
		}
		else {
			buff13Bytes[11-i] = '0';
		}
	}
}

inline void Word12toWStr(Word12 word, wchar_t* buff8Chars) {
	memset(buff8Chars, 0, sizeof(wchar_t)*8);

	_snwprintf_s(buff8Chars, 8, _TRUNCATE, L"%u", word);
}

inline void Word12toStrOctalFillZeros(Word12 word, char* buff8Bytes) {
	memset(buff8Bytes, 0, 8);
	snprintf(buff8Bytes, 8, "%04o", word);
}

inline void Word12toStrOctal(Word12 word, char* buff8Bytes) {
	memset(buff8Bytes, 0, 8);
	snprintf(buff8Bytes, 8, "%o", word);
}

inline void Word12toStr(Word12 word, char* buff8Bytes) {
	memset(buff8Bytes, 0, 8);
	snprintf(buff8Bytes, 8, "%u", word);
}

inline void Word12toStrFillZeros(Word12 word, char* buff8Bytes) {
	memset(buff8Bytes, 0, 8);
	snprintf(buff8Bytes, 8, "%04u", word);
}

inline void SignedWord12toStr(Word12 word, char* buff8Bytes) {

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