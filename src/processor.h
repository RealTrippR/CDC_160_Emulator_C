#ifndef CDC_160_PROCESSOR_H
#define CDC_160_PROCESSOR_H

#define E_REG  (proc->regZ & 0x3F)

#include "op_code_enum.h"
#include "twos_and_ones_complement_conversions.h"
#include "word_12.h"
#include "word_6.h"

inline Word6 Word12ToWord6(Word12 word) {
	return word & 0x3F;
}


void SET_MAINFRAME(struct CDC_160* mainframe);

void SET_MEM_BANK(struct MemBank* membank);

struct Processor
{
	Word12 regA;
	Word12 regB;
	Word12 regP;
	Word12 regS;
	Word12 regZ;
	Word6 regF;
};

void START(struct Processor* proc);
/*****************************************************/

void printRegisters(struct Processor* proc);

void processorTick(struct Processor* proc);

void callFunctionTranslator(struct Processor* proc, void(**inst_func)(struct Processor*));

// Addressing modes
void readDirect(struct Processor* proc);

void readIndirect(struct Processor* proc);

void readRelForward(struct Processor* proc);

void readRelBackward(struct Processor* proc);


void STA(struct Processor* proc); // store contents of A

void writeDirect(struct Processor* proc);

void writeIndirect(struct Processor* proc);

void writeRelForward(struct Processor* proc);

void writeRelBackward(struct Processor* proc);
/*****************************************************/
// Next Instruction
void RNI(struct Processor* proc);

/*****************************************************/
// ASM instructions


void ERR(struct Processor* proc);
void HLT(struct Processor* proc);


void PTA(struct Processor* proc); // https://homepage.cs.uiowa.edu/~jones/cdc160/man/oper.html#pa
void SHA(struct Processor* proc);

void LPN(struct Processor* proc);
void LPD(struct Processor* proc);
void LPI(struct Processor* proc);
void LPF(struct Processor* proc);
void LPB(struct Processor* proc);

void LSN(struct Processor* proc);
void LSD(struct Processor* proc);
void LSI(struct Processor* proc);
void LSF(struct Processor* proc);
void LSB(struct Processor* proc);

void LDN(struct Processor* proc);
void LDD(struct Processor* proc);
void LDI(struct Processor* proc);
void LDF(struct Processor* proc);
void LDB(struct Processor* proc);

void LCN(struct Processor* proc);
void LCD(struct Processor* proc);
void LCI(struct Processor* proc);
void LCF(struct Processor* proc);
void LCB(struct Processor* proc);

void ADN(struct Processor* proc);
void ADD(struct Processor* proc);
void ADI(struct Processor* proc);
void ADF(struct Processor* proc);
void ADB(struct Processor* proc);

void SBN(struct Processor* proc);
void SBD(struct Processor* proc);
void SBI(struct Processor* proc);
void SBF(struct Processor* proc);
void SBB(struct Processor* proc);

void STD(struct Processor* proc);
void STI(struct Processor* proc);
void STF(struct Processor* proc);
void STB(struct Processor* proc);

void SRD(struct Processor* proc);
void SRI(struct Processor* proc);
void SRF(struct Processor* proc);
void SRB(struct Processor* proc);

void RAD(struct Processor* proc);
void RAI(struct Processor* proc);
void RAF(struct Processor* proc);
void RAB(struct Processor* proc);

void AOD(struct Processor* proc);
void AOI(struct Processor* proc);
void AOF(struct Processor* proc);
void AOB(struct Processor* proc);

void ZJF(struct Processor* proc);
void ZJB(struct Processor* proc);

void NZF(struct Processor* proc);
void NZB(struct Processor* proc);

void PJF(struct Processor* proc);
void PJB(struct Processor* proc);

void NJF(struct Processor* proc);
void NJB(struct Processor* proc);

void JPI(struct Processor* proc);

void JFI(struct Processor* proc);

void INP(struct Processor* proc);

//void OUT(struct Processor* proc);

void OTN(struct Processor* proc);

void EXF(struct Processor* proc);

void INA(struct Processor* proc);

#endif // !CDC_160_PROCESSOR_H