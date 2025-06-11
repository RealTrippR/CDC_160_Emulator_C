#include "ferrranti_photoelectric_reader.h"
#include "CDC_160.h"
#include "sleep_ms.h"
#include <math.h>

bool FerrantiPhotoelectricReader_SetTapeLevel(struct FerrantiPhotoelectricReader* tapeReader, uint8_t tapeLevel)
{
	if (tapeLevel < 5 || tapeLevel > 8) {
		return false;
	}

	tapeReader->tapeLevel = tapeLevel;

	return true;
}

bool FerrantiPhotoElectricReader_ReadNextFrame(struct FerrantiPhotoelectricReader* tapeReader, struct CDC_160* mainframe)
{
	tapeReader->notReady = true;
	if (tapeReader->tape == NULL || tapeReader->notReady) {
		return false;
	}
	for (uint16_t i = 0; i < tapeReader->tapeLevel; ++i)
	{
		tapeReader->headPosHorz++;
		if (tapeReader->headPosHorz == tapeReader->tapeLevel) {
			tapeReader->headPosHorz = 0;
		}
		mainframe->inputLine |= tapeReader->tape->data[tapeReader->headPosVert] & (1 << tapeReader->headPosHorz);
	}

	mainframe->inputRequestLine = true;


	sleepms(171);

	tapeReader->headPosVert++;

	tapeReader->notReady = false;

	return true;
}

void FerrantiPhotoelectricReader_Tick(struct FerrantiPhotoelectricReader* tapeReader, struct CDC_160* mainframe)
{
	if (mainframe->on) {

		if (mainframe->functionReadyLine) {

			const Word12 UNIT_DESIGNATOR = mainframe->outputLine >> 6;
			const Word12 FUNCTION_DESIGNATOR = mainframe->outputLine & 0x77;
			const Word12 FUNCTION_CODE = mainframe->outputLine;
			if (FUNCTION_CODE == 04102) 
			{
				mainframe->resumeLine = false;


				mainframe->inputRequestLine = true;

				if (tapeReader->notReady == true) {
					mainframe->inputLine |= 04000;
				}
				else if (tapeReader->tape == NULL) {
					mainframe->inputLine |= 0400;
				}
				else if (tapeReader->headPosVert == tapeReader->tape->rowCount) {
					mainframe->inputLine |= 02000;
				}

				mainframe->resumeLine = true;

				return;
			}
			else {
				mainframe->resumeLine = false;
				tapeReader->isSelected = false;

				return;
			}
		}
		

		if (tapeReader->isSelected == true) {
			mainframe->resumeLine = false;
			if (mainframe->inputRequestLine == true) {
				FerrantiPhotoElectricReader_ReadNextFrame(tapeReader, mainframe);
				mainframe->resumeLine = true;

				tapeReader->isSelected = false;
			}
			return;
		}
	};
}