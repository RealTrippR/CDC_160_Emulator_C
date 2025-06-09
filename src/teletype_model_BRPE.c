#include "teletype_model_BRPE.h"
#include "paper_tape.h"
#include "CDC_160.h"
#include "sleep_ms.h"
#include <math.h>

int TeletypeModelBPRE_DisconnectFromMainframe(struct TeletypeModelBRPE* teletype, struct CDC_160* mainframe) {

}

// returns 1 if success, 0 if failure
int TeletypeModelBPRE_ConnectToMainframe(struct TeletypeModelBRPE* teletype, struct CDC_160* mainframe) {
	teletype->isSelected = false;
	teletype->connectionPort = &(mainframe->outputLine);

	for (uint8_t i = 0; i < 12; ++i) {
		if (mainframe->tickPeripherals[i] == NULL) {

		}
	}
}

void TeletypeModelBRPE_PunchCharIntoTape(struct TeletypeModelBRPE* teletype, char c, struct PaperTape1Inch* tape)
{
	sleepms(15); // sleep for a total of 1/60th of a second
	if (teletype->tape == NULL) {
		return;
	}
	const uint16_t bitMask = 0x7F;
	tape->data[teletype->headPosHorz] = c & bitMask;


	teletype->headPosHorz++;
	if (teletype->headPosHorz == tape->level) {
		teletype->headPosVert++;
		teletype->headPosHorz = 0;
	}

	sleepms(2); // sleep for a total of 1/60th of a second
}

void TeletypeModelBRPE_Tick(struct TeletypeModelBRPE* teletype, struct CDC_160* mainframe)
{
	if (teletype->isSelected) {
		Word12 clearReadyBitMask = 0b011111111111;
		
		//mainframe->ready 
		//// if ready, set bitmask octal 4000 (set 12th bit)
		//if (ready) {

		//}
		//TeletypeModelBRPE_PunchCharIntoTape();
		//	mainframe->tapePunch.connectionPort;
	}
}