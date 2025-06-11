#include "teletype_model_BRPE.h"
#include "paper_tape.h"
#include "CDC_160.h"
#include "sleep_ms.h"
#include <math.h>

// returns 1 if success, 0 if failure
int TeletypeModelBPRE_ConnectToMainframe(struct TeletypeModelBRPE* teletype, struct CDC_160* mainframe) {
	teletype->isSelected = false;
	teletype->connectionPort = &(mainframe->outputLine);
}

void TeletypeModelBRPE_PunchCharIntoTape(struct TeletypeModelBRPE* teletype, char c)
{
	teletype->inputReady = false;
	teletype->outputReady = false;

	sleepms(15); // sleep for a total of 1/60th of a second
	if (teletype->tape == NULL) {
		goto end;
	}
	const uint16_t bitMask = 0x7F;
	teletype->tape->data[teletype->headPosHorz] = c & bitMask;

end:
	teletype->headPosHorz++;
	if (teletype->headPosHorz == 7/*TAPE LEVEL*/) {
		teletype->headPosVert++;
		teletype->headPosHorz = 0;
	}

	sleepms(2); // sleep for a total of 1/60th of a second

	teletype->inputReady = true;
}

void TeletypeModelBRPE_Tick(struct TeletypeModelBRPE* teletype, struct CDC_160* mainframe)
{
	if (mainframe->functionReadyLine) {
		const Word12 UNIT_DESIGNATOR = mainframe->outputLine >> 6;
		const Word12 FUNCTION_DESIGNATOR = mainframe->outputLine & 0x77;
		const Word12 FUNCTION_CODE = mainframe->outputLine;
		if (FUNCTION_CODE == 04104) {
			mainframe->resumeLine = false;

			mainframe->inputRequestLine = true;

			if (teletype->inputReady == false) {
				mainframe->inputLine |= 04000;
			}
			else if (teletype->tape == NULL) {
				mainframe->inputLine |=  0400;
			}
			else if (teletype->headPosVert == teletype->tape->rowCount) {
				mainframe->inputLine |= 02000;
			}

			teletype->isSelected = true;

			mainframe->resumeLine = true;
		}
		else {
			mainframe->resumeLine = false;
			teletype->isSelected = false;
			return;
		}
	}

	if (mainframe->outputReadyLine && teletype->isSelected == true) {
		TeletypeModelBRPE_PunchCharIntoTape(teletype, mainframe->outputLine);

		mainframe->resumeLine = true;
	}
}

void TeletypeModelBPRE_LoadTape(struct TeletypeModelBRPE* teletype, struct PaperTape1Inch* tape) {
	teletype->tape = tape;
	teletype->headPosHorz = 0;
	teletype->headPosVert = 0;
}