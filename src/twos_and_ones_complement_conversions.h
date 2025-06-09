#ifndef TWO_AND_ONES_COMPLEMENT_CONVERSION_H
#define TWO_AND_ONES_COMPLEMENT_CONVERSION_H

inline Word12 ToOnesComplement(Word12 twos) {
	// if the number if negative, subtract 1
	if (twos < 0) {
		Word12 ones = (twos - 1) & 0xFFF;
		return ones;
	}
	return twos;
}

inline int16_t ToTwosComplement(Word12 ones) {
	// if the number is negative, add 1

	if (ones == 0xFFF /* -0 */) {
		return 0;
	}
	if (ones & 0x800 /*check sign*/) {
		int16_t twos = (ones + 1);
		return twos;
	}
	return ones;
}

#endif // !TWO_AND_ONES_COMPLEMENT_H