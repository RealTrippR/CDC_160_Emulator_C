#include "src/CDC_160.h"
#include "src/asmToTape.h"

#include <threads.h>


int main()
{
	// Refer to the CDC programming manual for more information
	// https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
	// https://www.ed-thelen.org/comp-hist/BRL61-c.html

	struct CDC_160 mainframe = { 0 };
	CDC_160_LoadStateFromDisk(&mainframe, "CDC_160.state");
	SET_MAINFRAME(&mainframe);

	//CDC_160_TurnOn(&mainframe);
	//CDC_160_Clear(&mainframe);


	// Load paper tape
	struct PaperTape1Inch tape = { 0 };
	asmToTape(&tape, "exampleASM\\Routine1.asm");

	mainframe.tapeReader.tape = &tape;

	while (1)
	{
		if (CDC_160_PrintCLI(&mainframe) == false) {
			break;
		}
		CDC_160_Tick(&mainframe);
	}

	CDC_160_SaveStateToDisk(&mainframe, "CDC_160.state");
	PaperTape1Inch_Destroy(&tape);
}