#include "word_12.h"
#include "paper_tape.h"
#include <stdbool.h>

#ifndef TELETYPE_MODEL_BRPE_H
#define TELETYPE_MODEL_BRPE_H

// Manual
// https://bitsavers.org/communications/teletype/brochures/BRPE_Brochure_1966.pdf



// Teletype BRPE
// high - speed
// paper tape punch
// note that the CDC 160 uses 7-level punch tape, this cannot be adjusted. See page 20. https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
// Operates at 60 frames per second.
struct TeletypeModelBRPE { // https://www.navy-radio.com/manuals/tty/jk/592-802-100-iss3-6403.pdf ?
	Word12* connectionPort; // pointer to the output line of the CDC 160

	// the tape that is currently being read from.
	struct PaperTape1Inch* tape;
	// the current bit position of the tape that is being written
	uint8_t headPosHorz;
	// the current bit position of the tape that is being written
	uint32_t headPosVert;

	bool inputReady; // can only be input ready or output ready
	bool outputReady;
	bool isSelected;



	uint8_t peripheralIdx;
};

int TeletypeModelBPRE_ConnectToMainframe(struct TeletypeModelBRPE* teletype, struct CDC_160* mainframe);

void TeletypeModelBRPE_PunchCharIntoTape(struct TeletypeModelBRPE* teletype, char c, struct PaperTape1Inch* tape);

void TeletypeModelBRPE_Tick(struct TeletypeModelBRPE* teletype, struct CDC_160* mainframe);

#endif // !TELETYPE_MODEL_BRPE_H