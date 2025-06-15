#include "ferrranti_photoelectric_reader.h"
#include "CDC_160.h"
#include "sleep_ms.h"
#include <math.h>

bool FerrantiPhotoelectricReader_SetTapeLevel(struct FerrantiPhotoelectricReader* tapeReader, uint8_t tapeLevel)
{
	if (tapeLevel < 5 || tapeLevel > 8) {
		tapeLevel = 7;
		return false;
	}

	tapeReader->tapeLevel = tapeLevel;

	return true;
}

bool FerrantiPhotoElectricReader_ReadNextFrame(struct FerrantiPhotoelectricReader* tapeReader, struct CDC_160* mainframe)
{
	if (tapeReader->tapeLevel < 5 || tapeReader->tapeLevel > 8) {
		tapeReader->tapeLevel = 7;
		return false;
	}

	tapeReader->notReady = true;
	if (tapeReader->tape == NULL) {
		return false;
	}

	if (tapeReader->headPosVert >= tapeReader->tape->rowCount) {
		return false;
	}

	for (uint16_t i = 0; i < tapeReader->tapeLevel; ++i)
	{
		tapeReader->headPosHorz++;
		if (tapeReader->headPosHorz == tapeReader->tapeLevel) {
			tapeReader->headPosHorz = 0;
		}
		//mainframe->inputLine |= tapeReader->tape->data[tapeReader->headPosVert] & (1 << tapeReader->headPosHorz);
	}

	mainframe->inputLine |= tapeReader->tape->data[tapeReader->headPosVert] & 0x3F;
#ifndef NDEBUG
	const char dbgc = tapeReader->tape->data[tapeReader->headPosVert] & 0x3F;

#endif // !NDEBUG

	tapeReader->headPosVert++;

	tapeReader->notReady = false;
	return true;
}

void FerrantiPhotoelectricReader_Tick(struct FerrantiPhotoelectricReader* tapeReader, struct CDC_160* mainframe)
{
	if (mainframe->on) {

		static size_t msWaitCounter = 0x0;
		static unsigned long timeLastMS = 0x0;
		static bool sleeping = false;
		static float timeMS = 0;

		sleepmsNonBlocking((unsigned long)timeMS, &msWaitCounter, &timeLastMS, &sleeping);
		timeLastMS = getTickCount32();

		if (!sleeping) {
			if (mainframe->functionReadyLine)
			{
				timeMS = 0; /*no wait*/

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
					else {
						tapeReader->isSelected = true;
					}

					mainframe->resumeLine = true;

					goto end;
				}
				else {
					//mainframe->resumeLine = false;
					tapeReader->isSelected = false;

					goto end;
				}
			}


			if (tapeReader->isSelected == true) {
				mainframe->resumeLine = false;
				if (mainframe->inputRequestLine == true && mainframe->resumeLine == false) {
					FerrantiPhotoElectricReader_ReadNextFrame(tapeReader, mainframe);
					timeMS = 171; /*wait for a 350th of a minute*/

					mainframe->resumeLine = true;

					tapeReader->isSelected = false;
					/*if (msWaitCounter > timeMS) {
						tapeReader->isSelected = false;
						timeMS = 0;
					}*/
					//tapeReader->isSelected = false;
				}
				goto end;
			}
		}
	end:
		return;
	};
}