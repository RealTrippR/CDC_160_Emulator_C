#include <stdio.h>
#include <time.h>
#include "src/CDC_160.h"
#include <threads.h>

void runTeletype(struct CDC_160* mainframe) {
	while (1) {
		TeletypeModelBRPE_Tick(&mainframe->tapePunch, mainframe);
	}
}

void readTapeReader(struct CDC_160* mainframe) {
	while (1) {
		FerrantiPhotoelectricReader_Tick(&mainframe->tapePunch, mainframe);
	}
}
 
int main() 
{
	// Refer to the CDC programming manual for more information
	// https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
	// https://www.ed-thelen.org/comp-hist/BRL61-c.html

	struct CDC_160 mainframe = { 0 };
	SET_MAINFRAME(&mainframe);

	CDC_160_TurnOn(&mainframe);



	// Load paper tape
	struct PaperTape1Inch tape = { 0 };
	PaperTape1Inch_Create(&tape, 50);
	TeletypeModelBPRE_LoadTape(&mainframe.tapePunch, &tape);


	// all addressees here are in decimal

	struct MemBank mem = mainframe.mem;

	mem.data[1] = 8; // mem[1] is the entry point of the program
	// addresses 2-7 should be used for subroutine exits (but don't have to be)
	// mem.data+8 is the counter location (CT), but also the first instruction (smashed to save space)
	writeASM(mem.data + 8, LCN_E, 012); // put -10 in A
	writeASM(mem.data + 9, STD_E, 8); // put A in counter location
	writeASM(mem.data + 10, AOD_E, 8); // increment CT
	writeASM(mem.data + 11, NZB_E, 1); // jump back 1 address if non zero


	// form the code 04104 in A
	writeASM(mem.data + 12, LDN_E, 041);
	// shift left by 6 bits
	writeASM(mem.data + 13, SHA_E, 0110);
	writeASM(mem.data + 14, SHA_E, 0110);
	// OR 04 with contents of A
	writeASM(mem.data + 15, LSN_E, 04);
	writeASM(mem.data + 16, STD_E, 41); // store A in address 41


	writeASM(mem.data + 17, LDN_E, 0); // put 0 in A
	writeASM(mem.data + 18, ZJF_E, 12); // jump 13 locations


	// the PTA control comamnd is only available on serial numbers 37 and above. Note that some of the 36 others were retrofitted to include this instruction.
	writeASM_CTRL_INSTR(mem.data + 30, P_TO_A); 
	writeASM(mem.data + 31, EXF_E, 10);
	writeASM(mem.data + 32, LCN_E, 012);


	//mem.data[41] = 04104;
	
	writeASM(mem.data + 32, OTN_E, 07); // write 07 to punch tape
	writeASM(mem.data + 33, INA_E, 00); // Read to A register
	
	writeASM_CTRL_INSTR(mem.data + 34, HALT);


	mainframe.mem = mem;

	START(&mainframe.proc);


	thrd_t thread;
	thrd_create(&thread, runTeletype, &mainframe);
	thrd_detach(thread);
	

	while (1)
	{
		printf("\033[1A");
		printf("                                                   \r");
		printRegisters(&mainframe.proc);
		CDC_160_Tick(&mainframe);

		
	}
}