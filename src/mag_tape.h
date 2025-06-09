const uint8_t bitMask = 0x3F;
c &= bitMask;
tape->data[teletype->headPosHorz] = c;
uint8_t evenBitCnt = 0;

/// count the number of 1s
for (uint8_t i = 0; i < 6; i++) {
	if ((c >> i) & 1) {
		evenBitCnt++;
	}
}

if (evenBitCnt % 2 != 0) {
	// write parity bit if odd number of bits
	c |= 1 << 6;
}