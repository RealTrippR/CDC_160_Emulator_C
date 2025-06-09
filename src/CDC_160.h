#include "memory_bank.h"
#include "processor.h"
#include "teletype_model_BRPE.h"
#include "ferrranti_photoelectric_reader.h"

#include <stdbool.h>
struct CDC_160
{
	// see page 15: https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
	// The reader and punch are located in the computer and are not considered external equipment.
	struct FerrantiPhotoelectricReader tapeReader;
	struct TeletypeModelBRPE tapePunch;

	


	// IO
	Word12* inputLines[12];
	Word12 outputLine;
	bool functionReadyLine;
	
	bool inputRequestLine;

	bool outputReadyLine;

	void (*tickPeripherals[12])();

	/*The External Function instruction places a 12-bit code on the output lines and places a
									signal on the external function ready line. Upon receiving this signal, all external
									devices examine the code contained on the output lines. If a device recognizes its external function code, it sends an acknowledgment (resume signal) to the computer,
									which then proceeds to the next instruction. If no external unit sends back a resume,
									the computer will stop and display a SEL signal on the console.
									*/

								/*
								The 12 input lines are also dual-purpose. Normally they carry up to 12 bits of input
								information from the external device to the computer. After a status request external
								function, the input lines carry 12 bits of status information, which is read into the computer by an input instruction. Status information is identified by the fact that it is the
								first information read in by an input instruction after a status request.
								*/
};

// returns the logical sum of all the input lines (a.k.a the input of the current device)
inline Word12 CDC_160_SumInputLines(struct CDC_160* cdc) {
	Word12 v = 0x0;

	for (uint8_t i = 0; i < 12; ++i) {
		if (cdc->inputLines[i] != NULL) {
			v |= *(cdc->inputLines[i]);
		}
	}

	return v;
}

inline void CDC_160_Tick(struct CDC_160* cdc160) {
	TeletypeModelBRPE_Tick(&cdc160->tapePunch, cdc160);
}

inline void writeASM_CTRL_INSTR(Word12* dst, enum ControlInstruction inst) {
	if (inst == HALT) {
		*dst = 0b000000000000;
	}
	else if (inst == ERROR) {
		*dst = 0b111111111111;
	}
	else if (inst == P_TO_A) {
		*dst = 0b000001000001;
	}
	else {
		printf("ASM WRITE CONTROL INSTR ERROR: INVALID CONTROL INSTRUCTION!\n");
		return;
	}
}

inline void writeASM(Word12* dst, enum FunctionCode op, uint8_t E__) {
	Word12 inst = 0x0;
	
	E__ &= 0x3F;

	inst |= E__;
	op = ToOnesComplement(op);

	inst |= (op << 6);

	*dst = inst;
}
