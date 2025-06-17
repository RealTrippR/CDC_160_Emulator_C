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

	Word12 a = 1;
	Word12 b = 1;
	//Word12 c = Add_Word12(a, b);
	Word12 c = Sub_Word12(a, b);







	// Load paper tape
	struct PaperTape1Inch tape = { 0 };
	asmToTape(&tape, "exampleASM\\Routine1.asm");

	mainframe.tapeReader.tape = &tape;

	CDC_160_InitCLI();
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