#ifndef FERRANTI_PHOTO_ELECTRIC_TAPE_READER_H
#define FERRANTI_PHOTO_ELECTRIC_TAPE_READER_H

#include <stdbool.h>
#include <stdint.h>
#include "word_12.h"

struct FerrantiPhotoelectricReader {

	// note that the reader has to be selected with EXF before every INP instruction.
	// see page 60 for documentation of a very similar reader to this one: https://bitsavers.org/pdf/cdc/160/CDC160A/60014500G_CDC160A_Reference_Manual_196503.pdf?utm_source=chatgpt.com

	uint8_t tapeLevel;

	// the tape that is currently being read from.
	struct PaperTape1Inch* tape;
	// the current bit position of the tape that is being written
	uint8_t headPosHorz;
	// the current bit position of the tape that is being written
	uint32_t headPosVert;

	bool notReady;
	bool isSelected;
};

// returns true if successful
bool FerrantiPhotoelectricReader_SetTapeLevel(struct FerrantiPhotoelectricReader* tapeReader, uint8_t tapeLevel);

// returns true if successful
bool FerrantiPhotoElectricReader_ReadNextFrame(struct FerrantiPhotoelectricReader* tapeReader, struct CDC_160* mainframe);

void FerrantiPhotoelectricReader_Tick(struct FerrantiPhotoelectricReader* tapeReader, struct CDC_160* mainframe);

#endif // !FERRANTI_PHOTO_ELECTRIC_TAPE_READER_H
