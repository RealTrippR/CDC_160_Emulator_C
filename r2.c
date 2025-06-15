#include <stdio.h>
#include <time.h>
#include "src/CDC_160.h"
#include "src/asmToTape.h"

#include <threads.h>

 void runTeletype(struct CDC_160* mainframe) {
	while (1) {
		TeletypeModelBRPE_Tick(&mainframe->tapePunch, mainframe);
	}
}

void readTapeReader(struct CDC_160* mainframe) {
	while (1) {
		FerrantiPhotoelectricReader_Tick(&mainframe->tapeReader, mainframe);
	}
}
 
int main() 
{
	// Refer to the CDC programming manual for more information
	// https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
	// https://www.ed-thelen.org/comp-hist/BRL61-c.html

	struct CDC_160 mainframe = { 0 };
	CDC_160_LoadStateFromDisk(&mainframe, "CDC_160.state");
	SET_MAINFRAME(&mainframe);

	CDC_160_TurnOn(&mainframe);
	CDC_160_Clear(&mainframe);


	// Start peripheral devices
	thrd_t threadTT;
	thrd_create(&threadTT, runTeletype, &mainframe);
	thrd_detach(threadTT);

	thrd_t threadTR;
	thrd_create(&threadTR, readTapeReader, &mainframe);
	thrd_detach(threadTR);

	// Load paper tape
	struct PaperTape1Inch tape = { 0 };
	asmToTape(&tape, "exampleASM\\Routine2.asm");

	CDC_160_SetRegP(&mainframe, 01); // begin loading at address 1. Attempting to load at address
	mainframe.tapeReader.tape = &tape;
	CDC_160_Load(&mainframe);

	CDC_160_SetRegP(&mainframe, 02); // set starting address of program
	CDC_160_RunMode(&mainframe);

	while (1)
	{
		printf("\033[1A");
		printf("                                                   \r");
		printRegisters(&mainframe.proc);
		CDC_160_Tick(&mainframe);
	}

	CDC_160_SaveStateToDisk(&mainframe, "CD_C160.state");
}