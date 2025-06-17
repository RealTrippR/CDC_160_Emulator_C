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

	uint8_t loadNoneClear; //0: load, 1: none, 2: clear
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

void CDC_160_InitCLI();

bool CDC_160_PrintCLI(struct CDC_160* cdc);

void CDC_160_SaveStateToDisk(struct CDC_160* cdc, const char* stateFilePath);

void CDC_160_LoadStateFromDisk(struct CDC_160* cdc, const char* stateFilePath);

inline void CDC_160_TurnOn(struct CDC_160* cdc) 
{
	if (!cdc->on) {
		cdc->loadNoneClear = 1u; // none
		cdc->stepMode = 1u;
		cdc->on = true;
		cdc->inputLine = false;
		cdc->inputRequestLine = false;
		cdc->outputReadyLine = false;
		cdc->outputLine = false;
		cdc->resumeLine = false;
		cdc->selectFailure = false;
		cdc->readyToOperate = false;

		cdc->tapeReader.tapeLevel = 7;
	}

}


inline void CDC_160_TurnOff(struct CDC_160* cdc)
{
	if (cdc->on) {
		cdc->on = false;
		cdc->inputLine = false;
		cdc->inputRequestLine = false;
		cdc->outputReadyLine = false;
		cdc->outputLine = false;
		cdc->resumeLine = false;
		cdc->selectFailure = false;
		cdc->readyToOperate = false;

		cdc->tapeReader.tapeLevel = 7;
		cdc->stepMode = 1;
	}
}

inline bool CDC_160_RunMode(struct CDC_160* cdc) {
	cdc->stepMode = 0;
	cdc->proc.regS = cdc->proc.regP;
	READ(&cdc->proc);

	int timeUS;
	void(*f)(struct Processor*) = NULL;
	callFunctionTranslator(&cdc->proc, &f, &timeUS);
	if (f)
		f(&cdc->proc);
}

inline bool CDC_160_PauseMode(struct CDC_160* cdc) {
	cdc->stepMode = 1;
}

inline bool CDC_160_StepMode(struct CDC_160* cdc) {
	cdc->stepMode = 2;
	cdc->proc.regS = cdc->proc.regP;
	READ(&cdc->proc);
}


inline void CDC_160_SetRegA(struct CDC_160* cdc, Word12 regA)
{
	cdc->proc.regA = regA & 0xFFF;
}
inline void CDC_160_SetRegP(struct CDC_160* cdc, Word12 regP)
{
	cdc->proc.regP = regP & 0xFFF;
}
inline void CDC_160_SetRegB(struct CDC_160* cdc, Word12 regB)
{
	cdc->proc.regB = regB & 0xFFF;
}


inline void CDC_160_FlipBitRegP(struct CDC_160* cdc, uint8_t bit) 
{
	if (bit > 11) {return;}
	Word12 mask = 1 << bit;
	cdc->proc.regP ^= mask;
}

inline void CDC_160_FlipBitRegA(struct CDC_160* cdc, uint8_t bit)
{
	if (bit > 11) { return; }
	Word12 mask = 1 << bit;
	cdc->proc.regA ^= mask;
}

inline void CDC_160_FlipBitRegZ(struct CDC_160* cdc, uint8_t bit)
{
	if (bit > 11) { return; }
	Word12 mask = 1 << bit;
	cdc->proc.regZ ^= mask;
}




inline bool CDC_160_Load(struct CDC_160* cdc) 
{
	cdc->loadNoneClear = 0;
}

inline bool CDC_160_LoadTick(struct CDC_160* cdc)
{
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

	static size_t stage = 0;



	uint32_t frameIndex = 0;

	cdc->proc.regS = cdc->proc.regP;

	bool isDone = false;
	// TODO: IMPLEMENT READER RETURN CODES
	//while (true) {

	if (stage == 0) {
		cdc->proc.regZ = 0x0;
		cdc->proc.regB = 0x0;

		cdc->functionReadyLine = true;
		cdc->outputLine = 04102; // ferranti function code.
		stage++;
	}
	else if (stage == 1) {
		if (WAIT_FOR_RESUME(cdc)) {
			cdc->functionReadyLine = false;
			stage++;
		}
	}
	else if (stage == 2) {
		cdc->functionReadyLine = false;
		cdc->inputLine = 0x0;
		cdc->inputRequestLine = true;
		stage++;
	}
	else if (stage == 3) {
		if (WAIT_FOR_RESUME(cdc)) {
			cdc->functionReadyLine = false;
			cdc->inputRequestLine = false;
			stage++;
		}
	}
	else if (stage == 4) {

		if (cdc->inputLine == 02000 || cdc->inputLine == 0400 || cdc->inputLine == 04000) {
			isDone = true;
		}

		// load first frame into register B
		cdc->proc.regZ |= (cdc->inputLine << 6); // first frame contains higher order bits
		cdc->proc.regB = cdc->proc.regZ;
		cdc->functionReadyLine = true;
		cdc->outputLine = 04102; // ferranti function code.
		stage++;
	}
	else if (stage == 5) {
		if (WAIT_FOR_RESUME(cdc)) {
			cdc->functionReadyLine = false;
			cdc->inputRequestLine = false;
			stage++;
		}
	}
	else if (stage == 6) {
		// request 2nd frame
		cdc->inputLine = 0x0;
		cdc->inputRequestLine = true;
		stage++;
	}
	else if (stage == 7) {
		if (WAIT_FOR_RESUME(cdc)) {
			cdc->functionReadyLine = false;
			cdc->inputRequestLine = false;
			stage++;
		}
	}
	else if (stage==8) {
		cdc->inputRequestLine = false;
		cdc->proc.regZ = cdc->inputLine;

		if (cdc->proc.regZ == 02000 || cdc->proc.regZ == 0400 || cdc->proc.regZ == 04000) {
			isDone = true;
		}

		// load second frame and OR it with first
		cdc->proc.regZ |= cdc->inputLine & 0x3F; // second frame contains lower order bits
		cdc->proc.regZ |= cdc->proc.regB;
		cdc->proc.regA = cdc->proc.regZ;

		WRITE(&cdc->proc);

		Word12 tmp = cdc->tapeReader.tape->data[cdc->tapeReader.headPosVert];
		tmp >>= 6;
		// get 7th bit of 2nd frame.
		if (tmp == 1 || tmp == -1/*on some compilers, bitshift keeps sign*/) {
			// stop: there is no 7th level punch.
			isDone = true;
		}

		// increment S & P reg
		cdc->proc.regB = Add_Word12(cdc->proc.regP, 1);
		cdc->proc.regS = cdc->proc.regB;
		cdc->proc.regP = cdc->proc.regS;

		stage = 0;
	}


	if (isDone) {
		cdc->loadNoneClear = 1;
		HLT(&cdc->proc);
	}
	return true;
}

inline void CDC_160_Tick(struct CDC_160* cdc) {
	static uint16_t tdms = 0;
	static uint64_t lastTimeMS = 0;
	if (lastTimeMS == 0) {
		lastTimeMS = GetTickCount64();
	}
	if (cdc->on) {
		FerrantiPhotoelectricReader_Tick(&cdc->tapeReader, cdc);
		//TeletypeModelBRPE_Tick(&cdc->tapeReader, cdc);

		if (0 == cdc->loadNoneClear) {
			CDC_160_LoadTick(cdc);
		}
		else {

			if (cdc->stepMode == 2) {
				if (processorTick(&cdc->proc, tdms)) {
					CDC_160_PauseMode(cdc);
				}
			}
			else if (cdc->stepMode == 0) {
				processorTick(&cdc->proc, tdms);
			}
		}
	}
	tdms = GetTickCount64() - lastTimeMS;
	lastTimeMS = GetTickCount64();
}

// returns true if successful
inline bool CDC_160_Clear(struct CDC_160* cdc) 
{
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

	cdc->loadNoneClear = 1;
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
