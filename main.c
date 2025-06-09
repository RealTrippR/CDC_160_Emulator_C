#include <stdio.h>
#include <time.h>
#include "src/CDC_160.h"


int main() 
{
	// Refer to the CDC programming manual for more information
	// https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf

	struct CDC_160 mainframe = {0};
	struct Processor prc = {0};
	struct MemBank mem = { sizeof(mem.data)};

	SET_MAINFRAME(&mainframe);
	SET_MEM_BANK(&mem);
	// all addressees here are in decimal

	mem.data[1] = 8; // mem[1] is the entry point of the program
	// addresses 2-7 are reserved for subroutine exits
	// mem.data+8 is the counter location (CT), but also the first instruction (smashed to save space)
	writeASM(mem.data + 8, LCN_E, 012); // put -10 in A
	writeASM(mem.data + 9, STD_E, 8); // put A in counter location
	writeASM(mem.data + 10, AOD_E, 8); // increment CT
	writeASM(mem.data + 11, NZB_E, 1); // jump back 1 address if non zero


	writeASM(mem.data + 12, JFI_E, 2);
	mem.data[14] = 16; // 14 points to address 16
	writeASM(mem.data + 16, LDN_E, 0); // put 0 in A
	writeASM(mem.data + 17, ZJF_E, 13); // jump 13 locations

	writeASM_CTRL_INSTR(mem.data + 30, P_TO_A); // the PTA control comamnd is only available on serial numbers 37 and above. Note that some of the 36 others were retrofitted to include this instruction.
	writeASM_CTRL_INSTR(mem.data + 31, HALT);
	START(&prc);
	while (1)
	{
		printf("\033[1A");
		printf("                                                   \r");
		printRegisters(&prc);
		processorTick(&prc);
	}
}