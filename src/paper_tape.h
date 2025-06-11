#ifndef PAPER_TAPE_H
#define PAPER_TAPE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// https://www.chilton-computing.org.uk/acl/literature/chapman/p015.htm
// standard 1 inch paper tape (7-level punch tape is the standard for the CDC 160)
struct PaperTape1Inch {
	// an array of bits. A 0 bit represents a punched hole, a 1 bit is unpunched.
	char* data;
	size_t rowCount;

	
	uint8_t level; // 7 by default - the CDC can only write level 7 tape but can read from tape of 5,6,7, or 8 levels.
				   // note that the 7th bit is for parity - only 6 bit chars can be written
};

bool PaperTape1Inch_SaveToDisk(struct PaperTape1Inch* tape, const char* filename);

bool PaperTape1Inch_ReadFromDisk(struct PaperTape1Inch* tape, const char* filename);

uint32_t PaperTape1Inch_GetLengthIn(struct PaperTape1Inch* tape);

void PaperTape1Inch_Create(struct PaperTape1Inch* tape, uint32_t lengthInInches);

void PaperTape1Inch_Destroy(struct PaperTape1Inch* tape);


#endif // !PAPER_TAPE_H