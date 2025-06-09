#include "paper_tape.h"

uint32_t PaperTape1Inch_GetLengthIn(struct PaperTape1Inch* tape) {
	return tape->rowCount / 10; // 10 rows per inch
}

void PaperTape1Inch_Create(struct PaperTape1Inch* tape, uint32_t lengthInInches) {
	tape->level = 7;
	tape->rowCount = (lengthInInches) * 10; // 10 rows per inch
	tape->data = malloc(tape->rowCount); // max of 8 holes per row
}

void PaperTape1Inch_Destroy(struct PaperTape1Inch* tape) {
	free(tape->data);
}