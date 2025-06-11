#include "memory_bank.h"
#include "processor.h"
#include "teletype_model_BRPE.h"
#include "ferrranti_photoelectric_reader.h"

#include <stdbool.h>
struct CDC_160
{
	bool on;

	struct MemBank mem;

	// see page 15: https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
	// The reader and punch are located in the computer and are not considered external equipment.
	struct FerrantiPhotoelectricReader tapeReader;
	struct TeletypeModelBRPE tapePunch;

	struct Processor proc;

	uint8_t stepMode; //0: run, 1: paused, 2: step one instruction, then pause

	int EnterNoneSweep;

	float margin;

	bool readyToOperate;

	// IO
	Word12 inputLine;
	Word12 outputLine;
	bool functionReadyLine;
	
	bool resumeLine;

	bool inputRequestLine;

	bool outputReadyLine;

	bool selectFailure;
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

inline void CDC_160_TurnOn(struct CDC_160* cdc) {
	cdc->on = true;
	cdc->inputLine = false;
	cdc->inputRequestLine = false;
	cdc->outputReadyLine = false;
	cdc->outputLine = false;
	cdc->resumeLine = false;
	cdc->selectFailure = false;
	cdc->readyToOperate = false;
}

inline bool CDC_160_RunMode(struct CDC_160* cdc) {
	cdc->stepMode = 0;
}

inline bool CDC_160_PauseMode(struct CDC_160* cdc) {
	cdc->stepMode = 1;
}

inline bool CDC_160_StepMode(struct CDC_160* cdc) {
	cdc->stepMode = 2;
}



inline void CDC_160_Tick(struct CDC_160* cdc) {
	if (cdc->stepMode == 2) {
		processorTick(&cdc->proc);
		CDC_160_PauseMode(cdc);
	}
	else if (cdc->stepMode == 0) {
		processorTick(&cdc->proc);

	}
}

// returns true if successful
inline bool CDC_160_PowerOnTapePunch(struct CDC_160* cdc) {

}
// returns true if successful
inline bool CDC_160_PowerOffTapePunch(struct CDC_160* cdc) {

}

inline bool CDC_160_PowerOnTapeReader(struct CDC_160* cdc) {

}

inline bool CDC_160_PowerOffTapeReader(struct CDC_160* cdc) {

}

inline bool CDC_160_Load(struct CDC_160* cdc) {
	if (cdc->on == false) {
		return false;
	}

	// note that the computer should stop until the tape is done reading.
	
	// see page 61: https://bitsavers.org/pdf/cdc/160/CDC160A/60014500G_CDC160A_Reference_Manual_196503.pdf?utm_source=chatgpt.com
	// by default, the computer loads programs in a two-frames per word format of 7-level tape,
	// where the 7th level punch is not part of this data.
	/*
	Successive words must follow each other on tape. The automatic load will stop
	when a frame is read which should contain a 7th level punch and none exists.
	Tape may be placed in the reader any place on the leader; the automatic load
	will not begin until the first 7th level punch is sensed. Prior to starting automatic load, the FWA where the data is to be stored must be placed in P, and A
	should be cleared. When the load is completed, P will contain the LWA where
	data was stored and A will contain a check sum of the data read, modulus 212-1. */

	// expects 7 level tape
	if (cdc->tapeReader.tape == NULL || cdc->tapeReader.tapeLevel != 7) {
		return false;
	}

	uint32_t frameIndex = 0;


	cdc->proc.regS = cdc->proc.regP;

	// TODO: IMPLEMENT READER RETURN CODES
	while (true) {
		cdc->functionReadyLine = true;
		cdc->outputLine = 4102; // ferranti function code.
		WAIT_FOR_RESUME(cdc);
		cdc->functionReadyLine = false;





		cdc->inputRequestLine = true;
		WAIT_FOR_RESUME(cdc);
		cdc->inputRequestLine = false;

		if ((cdc->tapePunch.tape->data[cdc->tapePunch.headPosVert] >> 6) == 0) {
			cdc->proc.regZ |= (cdc->inputLine < 6) & 0xFC0; // first frame contains higher order bits
		} else {
			// this is electrically accurate, it thinks it's on the second frame but is actually on the first
			cdc->proc.regZ |= cdc->inputLine & 0x3F; // second frame contains lower order bits
		}

		cdc->inputRequestLine = true;
		WAIT_FOR_RESUME(cdc);
		cdc->inputRequestLine = false;
		cdc->proc.regZ |= cdc->inputLine & 0x3F; // second frame contains lower order bits

		// get 7th bit of 2nd frame.
		if ((cdc->tapePunch.tape->data[cdc->tapePunch.headPosVert] >> 6) == 0) {
			// stop: there is no 7th level punch.
			break;

		}

		// increment S & P reg
		cdc->proc.regB = Add_Word12(cdc->proc.regP, 1);
		cdc->proc.regS = cdc->proc.regB;
		cdc->proc.regP = cdc->proc.regS;
	}

	HLT(&cdc->proc);
	return true;
}

// returns true if successful
inline bool CDC_160_Clear(struct CDC_160* cdc) {
	if (cdc->on == false) {
		return false;
	}
	// "Placing this switch in the CLEAR position causes the registers to be reset to zero. 
	// Core storage is unchanged by the clear operation."
	
	cdc->proc.regA = 0;
	cdc->proc.regB = 0;
	cdc->proc.regF = 0;
	cdc->proc.regP = 0;
	cdc->proc.regS = 0;
	cdc->proc.regZ = 0;
	//memset(cdc->mem.data, 0, sizeof(cdc->mem.data));

	cdc->readyToOperate = true;

	return true;
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
