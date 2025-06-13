#include "memory_bank.h"
#include <assert.h>
#include "processor.h"
#include "CDC_160.h"

#undef OUT;

struct MemBank* mem;

struct CDC_160* mainframe;

void SET_MEM_BANK(struct MemBank* membank) {
    mem = membank;
    mem->data[0] = 07001; // see page 46: https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
}

void SET_MAINFRAME(struct CDC_160* _mainframe) {
    mainframe = _mainframe;
    SET_MEM_BANK(&mainframe->mem);
}

void READ(struct Processor* proc) {
    proc->regZ = mem->data[proc->regS];
}

void WRITE(struct Processor* proc) {
    mem->data[proc->regS] = proc->regZ;
}


void START(struct Processor* proc) {
    proc->regS = proc->regP;
    READ(proc);

    void(*f)(struct Processor*);
    callFunctionTranslator(proc, &f);
    f(proc);
}


/*****************************************************/
// returns false if a resume input timeout occurred
bool WAIT_FOR_RESUME(struct Processor* proc) {
    // wait until input request - this is the connection code
    for (uint32_t i = 0; i < 39062/*approx 250 ms, which is the timeout for peripherals*/; ++i)
    {
        sleepus(6.4); // sleep 1 clock cycle

        if (mainframe->inputRequestLine == true) {

            return true;
            break;
        }
    }

    mainframe->selectFailure = true;
    HLT(proc);

    return false;
}

/*****************************************************/
// Addressing modes
void readDirect(struct Processor* proc) {
    proc->regB = E_REG;
    proc->regS = proc->regB;
    READ(proc);
}

void readIndirect(struct Processor* proc) {
    proc->regB = E_REG;
    proc->regS = proc->regB;
    READ(proc);

    proc->regB = proc->regZ;
    proc->regS = proc->regB;
    READ(proc);
}

void readRelForward(struct Processor* proc) {
    proc->regB = Add_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;

    READ(proc);
}

void readRelBackward(struct Processor* proc) {
    proc->regB = Sub_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;
    READ(proc);
}



/*****************************************************/

void PTA(struct Processor* proc) {
    proc->regS = proc->regP;
    proc->regA = proc->regS;
    RNI(proc);
}

void STA(struct Processor* proc) { // store contents of A
    proc->regB = proc->regA;
    proc->regZ = proc->regB;

    WRITE(proc);
}

void writeDirect(struct Processor* proc) {
    proc->regB = E_REG;
    proc->regS = proc->regB;

    WRITE(proc);
}

void writeIndirect(struct Processor* proc) {
    proc->regB = E_REG;
    proc->regS = proc->regB;
    readDirect(proc);

    proc->regB = proc->regZ;
    proc->regS = proc->regB;

    WRITE(proc);
}

void writeRelForward(struct Processor* proc) {
    proc->regB = Add_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;
    
    WRITE(proc);
}

void writeRelBackward(struct Processor* proc) {
    proc->regB = Sub_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;

    WRITE(proc);
}

void printRegisters(struct Processor* proc) {
    char p[8];
    char a[8];
    char z[8];

    ToString_Word12(proc->regP, p);
    ToString_Word12(proc->regA, a);
    ToString_Word12(proc->regZ, z);

    printf("P: %s | ", p);
    printf("A: %s | ", a);
    printf("Z: %s", z);
    printf("\n");
}

void processorTick(struct Processor* proc) {

    void(*f)(struct Processor*) = NULL;
    callFunctionTranslator(proc, &f);
    if (f)
        f(proc);
}


void callFunctionTranslator(struct Processor* proc, void(**inst_func)(struct Processor*)) 
{
    *inst_func = NULL; // default
    float fTimeUsec = 0.0;

    // handle primary asm instructions
    proc->regF = Word12ToWord6(ShiftRight_Word12(proc->regZ, 6));
    enum FunctionCode opCode = ToTwosComplement(proc->regF);
   
    if (opCode == ERR_E) {
        *inst_func = ERR;
        fTimeUsec = 6.4;
    }
    else if (opCode == HLT_E) {
        *inst_func = HLT;
        fTimeUsec = 6.4;
    
/*****************************************************/
// PTA control command
    } else if (proc->regZ == 0b000001000001) {
        *inst_func = PTA;
        fTimeUsec = 6.4;
    }
// Shift Left
    else if (opCode == SHA_E) {
        *inst_func = SHA;
        fTimeUsec = 6.4;
    }
/*****************************************************/
// Load Product
    else if (opCode == LPN_E) {
        *inst_func = LPN;
        fTimeUsec = 6.4;
    } else if (opCode == LPD_E) {
        *inst_func = LPD;
        fTimeUsec = 12.8;
    } else if (opCode == LPI_E) {
        *inst_func = LPI;
        fTimeUsec = 19.2;
    }
    else if (opCode == LPF_E) {
        *inst_func = LPF;
        fTimeUsec = 12.8;
    }
    else if (opCode == LPB_E) {
        *inst_func = LPB;
        fTimeUsec = 12.8;
    }
/*****************************************************/
// Logical Sum
    else if (opCode == LSN_E) {
        *inst_func = LSN;
        fTimeUsec = 6.4;
    }
    else if (opCode == LSD_E) {
        *inst_func = LSD;
        fTimeUsec = 12.8;
    }
    else if (opCode == LSI_E) {
        *inst_func = LSI;
        fTimeUsec = 19.2;
    }
    else if (opCode == LSF_E) {
        *inst_func = LSF;
        fTimeUsec = 12.8;
    }
    else if (opCode == LSB_E) {
        *inst_func = LSB;
        fTimeUsec = 12.8;
    }
/*****************************************************/
// Load
    else if (opCode == LDN_E) {
        *inst_func = LDN;
        fTimeUsec = 6.4;
    }
    else if (opCode == LDD_E) {
        *inst_func = LDD;
        fTimeUsec = 12.8;
    }
    else if (opCode == LDI_E) {
        *inst_func = LDI;
        fTimeUsec = 19.2;
    }
    else if (opCode == LDF_E) {
        *inst_func = LDF;
        fTimeUsec = 12.8;
    }
    else if (opCode == LDB_E) {
        *inst_func = LDB;
        fTimeUsec = 12.8;
    }
/*****************************************************/
// Load Complement
    else if (opCode == LCN_E) {
        *inst_func = LCN;
        fTimeUsec = 6.4;
    }
    else if (opCode == LCD_E) {
        *inst_func = LCD;
        fTimeUsec = 12.8;
    }
    else if (opCode == LCI_E) {
        *inst_func = LCI;
        fTimeUsec = 19.2;
    }
    else if (opCode == LCF_E) {
        *inst_func = LCF;
        fTimeUsec = 12.8;
    }
    else if (opCode == LCB_E) {
        *inst_func = LCB;
        fTimeUsec = 12.8;
    }
/*****************************************************/
// Add
    else if (opCode == ADN_E) {
        *inst_func = ADN;
        fTimeUsec = 6.4;
    }
    else if (opCode == ADD_E) {
        *inst_func = ADD;
        fTimeUsec = 12.8;
    }
    else if (opCode == ADI_E) {
        *inst_func = ADI;
        fTimeUsec = 19.2;
    }
    else if (opCode == ADF_E) {
        *inst_func = ADF;
        fTimeUsec = 12.8;
    }
    else if (opCode == ADB_E) {
        *inst_func = ADB;
        fTimeUsec = 12.8;
    }
/*****************************************************/
// Subtract
    else if (opCode == SBN_E) {
        *inst_func = SBN;
        fTimeUsec = 6.4;
    }
    else if (opCode == SBD_E) {
        *inst_func = SBD;
        fTimeUsec = 12.8;
    }
    else if (opCode == SBI_E) {
        *inst_func = SBI;
        fTimeUsec = 19.2;
    }
    else if (opCode == SBF_E) {
        *inst_func = SBF;
        fTimeUsec = 12.8;
    }
    else if (opCode == SBB_E) {
        *inst_func = SBB;
        fTimeUsec = 12.8;
    }
/*****************************************************/
// Shift
    else if (opCode == STD_E) {
        *inst_func = STD;
        fTimeUsec = 19.2;
    }
    else if (opCode == STI_E) {
        *inst_func = STI;
        fTimeUsec = 25.6;
    }
    else if (opCode == STB_E) {
        *inst_func = STB;
        fTimeUsec = 19.2;
    }
    else if (opCode == STF_E) {
        *inst_func = STF;
        fTimeUsec = 19.2;
    }
/*****************************************************/
// Shift Replace
    else if (opCode == SRD_E) {
        *inst_func = SRD;
        fTimeUsec = 19.2;
    }
    else if (opCode == SRI_E) {
        *inst_func = SRI;
        fTimeUsec = 25.6;
    }
    else if (opCode == SRB_E) {
        *inst_func = SRB;
        fTimeUsec = 19.2;
    }
    else if (opCode == SRF_E) {
        *inst_func = SRF;
        fTimeUsec = 19.2;
    }
/*****************************************************/
// Replace Add
    else if (opCode == RAD_E) {
        *inst_func = RAD;
        fTimeUsec = 19.2;
    }
    else if (opCode == RAI_E) {
        *inst_func = RAI;
        fTimeUsec = 25.6;
    }
    else if (opCode == RAF_E) {
        *inst_func = RAF;
        fTimeUsec = 19.2;
    }
    else if (opCode == RAB_E) {
        *inst_func = RAB;
        fTimeUsec = 19.2;
    }
/*****************************************************/
// Replace Add One
    else if (opCode == AOD_E) {
        *inst_func = AOD;
        fTimeUsec = 19.2;
    }
    else if (opCode == AOI_E) {
        *inst_func = AOI;
        fTimeUsec = 25.6;
    }
    else if (opCode == AOF_E) {
        *inst_func = AOF;
        fTimeUsec = 19.2;
    }
    else if (opCode == AOB_E) {
        *inst_func = AOB;
        fTimeUsec = 19.2;
    }
/*****************************************************/
// Zero Jump
    else if (opCode == ZJF_E) {
        *inst_func = ZJF;
        fTimeUsec = 6.4;
    }
    else if (opCode == ZJB_E) {
        *inst_func = ZJB;
        fTimeUsec = 6.4;
    }
/*****************************************************/
// Non-Zero Jump
    else if (opCode == NZF_E) {
        *inst_func = NZF;
        fTimeUsec = 6.4;
    }
    else if (opCode == NZB_E) {
        *inst_func = NZB;
        fTimeUsec = 6.4;
    }
/*****************************************************/
// Positive Jump
    else if (opCode == PJF_E) {
        *inst_func = PJF;
        fTimeUsec = 6.4;
    }
    else if (opCode == PJB_E) {
        *inst_func = PJB;
        fTimeUsec = 6.4;
    }
/*****************************************************/
// Negative Jump
    else if (opCode == NJF_E) {
        *inst_func = NJF;
        fTimeUsec = 6.4;
    }
    else if (opCode == NJB_E) {
        *inst_func = NJB;
        fTimeUsec = 6.4;
    }
/*****************************************************/
// Jump Indirect
    else if (opCode == JPI_E) {
        *inst_func = JPI;
        fTimeUsec = 12.8;
    }
/*****************************************************/
// Jump Forward Indirect
    else if (opCode == JFI_E) {
        *inst_func = JFI;
        fTimeUsec = 12.8;
    }

/*****************************************************/
// Input
    else if (opCode == INP_E) {
        *inst_func = INP;
        fTimeUsec = 12.8;
        if (mem->data[proc->regP + 2] > mem->data[proc->regP + 1]) {
            fTimeUsec += 12.8 * (mem->data[proc->regP + 2] - mem->data[proc->regP + 1]);
        }
    }
/*****************************************************/
// Output
    else if (opCode == OUT_E) {
        *inst_func = OUT;
        fTimeUsec = 12.8;
        if (mem->data[proc->regP + 2] > mem->data[proc->regP + 1]) {
            fTimeUsec += 12.8 * (mem->data[proc->regP + 2] - mem->data[proc->regP + 1]);
        }
    }
    else if (opCode == OTN_E) {
        *inst_func = OTN;
        fTimeUsec = 6.4;
    }
    else if (opCode == EXF_E) {
        *inst_func = EXF;
        fTimeUsec = 12.8;
    }
    else if (opCode == INA_E) {
        *inst_func = INA;
        fTimeUsec = 6.4;
    }



/*****************************************************/
if (fTimeUsec != 0.0) {
    sleepus(fTimeUsec);
}
}

/*****************************************************/
// 
void RNI(struct Processor* proc) {
    proc->regB = Add_Word12(proc->regP, 1);
    proc->regS = proc->regB;
    proc->regP = proc->regS;
    READ(proc);
}

void JNI_FORWARD(struct Processor* proc) {
    proc->regB = Add_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;
    proc->regP = proc->regS;
    READ(proc);
}

void JNI_BACKWARD(struct Processor* proc) {
    proc->regB = Sub_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;
    proc->regP = proc->regS;
    READ(proc);
}

void JNI_INDIRECT(struct Processor* proc) {
    proc->regB = proc->regZ;
    proc->regS = proc->regB;
    proc->regP = proc->regS;
    READ(proc);
}

/*****************************************************/
// Error & Halt
void ERR(struct Processor* proc) {
    proc->regZ = 0b111111111111;
}
void HLT(struct Processor* proc) {
    proc->regZ = 0b000000000000;
}

/*****************************************************/
// Shift A
void SHA(struct Processor* proc) {
    // this function is unique because the codes here are dependent upon 
    // the contents of the Z register, because they include the function code 01 (SHA).
    if (proc->regZ == 0102) {
        proc->regA = RotateLeft_Word12(proc->regA, 1);
    }
    else if (proc->regZ == 0110) {
        proc->regA = RotateLeft_Word12(proc->regA, 3);
    }
    else if (proc->regZ == 0112) {
        proc->regA = Mult_Word12(proc->regA, 012);
    }
    else {
        ERR(proc);
        return;
    }
    RNI(proc);
}

/*****************************************************/
// Logical Product
void LPN(struct Processor* proc) {
    proc->regA = BitwiseAnd_Word12(E_REG, proc->regA);
    RNI(proc);
}
void LPD(struct Processor* proc) {
    readDirect(proc);
    proc->regA = BitwiseAnd_Word12(proc->regZ, proc->regA);
    RNI(proc);
}
void LPI(struct Processor* proc) {
    readIndirect(proc);
    proc->regA = BitwiseAnd_Word12(proc->regZ, proc->regA);
    RNI(proc);
}
void LPF(struct Processor* proc) {
    readRelForward(proc);
    proc->regA = BitwiseAnd_Word12(proc->regZ, proc->regA);
    RNI(proc);
}
void LPB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regA = BitwiseAnd_Word12(proc->regZ, proc->regA);
    RNI(proc);
}

/*****************************************************/
// Logical Sum
void LSN(struct Processor* proc) {
    proc->regA = BitwiseOR_Word12(E_REG, proc->regA);
    RNI(proc);
}

void LSD(struct Processor* proc) {
    readDirect(proc);
    proc->regA = BitwiseOR_Word12(proc->regZ, proc->regA);
    RNI(proc);
}

void LSI(struct Processor* proc) {
    readIndirect(proc);
    proc->regA = BitwiseOR_Word12(proc->regZ, proc->regA);
    RNI(proc);
}


void LSF(struct Processor* proc) {
    readRelForward(proc);
    proc->regA = BitwiseOR_Word12(proc->regZ, proc->regA);
    RNI(proc);
}

void LSB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regA = BitwiseOR_Word12(proc->regZ, proc->regA);
    RNI(proc);
}

/*****************************************************/
// Load

void LDN(struct Processor* proc) {
    proc->regB = E_REG;
    proc->regA = proc->regB;
    RNI(proc);
}
void LDD(struct Processor* proc) {
    readDirect(proc);
    proc->regB = proc->regZ;
    proc->regA = proc->regB;
    RNI(proc);
}
void LDI(struct Processor* proc) {
    readIndirect(proc);
    proc->regB = proc->regZ;
    proc->regA = proc->regB;
    RNI(proc);
}
void LDF(struct Processor* proc) {
    readRelForward(proc);
    proc->regB = proc->regZ;
    proc->regA = proc->regB;
    RNI(proc);
}
void LDB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regB = proc->regZ;
    proc->regA = proc->regB;
    RNI(proc);
}

/*****************************************************/
// Load Complement
void LCN(struct Processor* proc) {
    proc->regA = BitwiseNot_Word12(E_REG);
    RNI(proc);
}

void LCD(struct Processor* proc) {
    readDirect(proc);
    proc->regA = BitwiseNot_Word12(proc->regZ);
    RNI(proc);
}
void LCI(struct Processor* proc) {
    readIndirect(proc);
    proc->regA = BitwiseNot_Word12(proc->regZ);
    RNI(proc);
}
void LCF(struct Processor* proc) {
    readRelForward(proc);
    proc->regA = BitwiseNot_Word12(proc->regZ);
    RNI(proc);
}
void LCB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regA = BitwiseNot_Word12(proc->regZ);
    RNI(proc);
}

/*****************************************************/
// Add
void ADN(struct Processor* proc) {
    proc->regB = Add_Word12(E_REG, proc->regA);
    proc->regA = proc->regB;
    RNI(proc);
}
void ADD(struct Processor* proc) {
    readDirect(proc);
    proc->regB = Add_Word12(proc->regZ, proc->regA);
    proc->regA = proc->regB;
    RNI(proc);
}
void ADI(struct Processor* proc) {
    readIndirect(proc);
    proc->regB = Add_Word12(proc->regZ, proc->regA);
    proc->regA = proc->regB;
    RNI(proc);
}
void ADF(struct Processor* proc) {
    readRelForward(proc);
    proc->regB = Add_Word12(proc->regZ, proc->regA);
    proc->regA = proc->regB;
    RNI(proc);
}
void ADB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regB = Add_Word12(proc->regZ, proc->regA);
    proc->regA = proc->regB;
    RNI(proc);
}

/*****************************************************/
// Subtract
void SBN(struct Processor* proc) {
    proc->regB = Sub_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;
    RNI(proc);
}
void SBD(struct Processor* proc) {
    readDirect(proc);
    proc->regB = Sub_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;
    RNI(proc);
}
void SBI(struct Processor* proc) {
    readIndirect(proc);
    proc->regB = Sub_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;
    RNI(proc);
}
void SBF(struct Processor* proc) {
    readRelForward(proc);
    proc->regB = Sub_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;
    RNI(proc);
}
void SBB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regB = Sub_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;
    RNI(proc);
}

/*****************************************************/
// Store
void STD(struct Processor* proc) {
    proc->regB = E_REG;
    proc->regS = proc->regB;

    STA(proc);
    RNI(proc);
}
void STI(struct Processor* proc) {
    proc->regB = E_REG;
    proc->regS = proc->regB;

    readDirect(proc);

    proc->regB = proc->regZ;
    proc->regS = proc->regB;
    STA(proc);
    RNI(proc);
}
void STF(struct Processor* proc) {
    proc->regB = Add_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;
    STA(proc);
    RNI(proc);
}
void STB(struct Processor* proc) {
    proc->regB = Sub_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;
    STA(proc);
    RNI(proc);
}

/*****************************************************/
// Shift Replace
void SRD(struct Processor* proc) {
    readDirect(proc);
    proc->regA = ShiftLeft_Word12(proc->regZ, ToOnesComplement(1));
    WRITE(proc);
    RNI(proc);
}
void SRI(struct Processor* proc) {
    readIndirect(proc);
    proc->regA = ShiftLeft_Word12(proc->regZ, ToOnesComplement(1));
    WRITE(proc);
    RNI(proc);
}
void SRF(struct Processor* proc) {
    readRelForward(proc);
    proc->regA = ShiftLeft_Word12(proc->regZ, ToOnesComplement(1));
    WRITE(proc);
    RNI(proc);
}
void SRB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regA = ShiftLeft_Word12(proc->regZ, ToOnesComplement(1));
    WRITE(proc);
    RNI(proc);
}

/*****************************************************/
// Replace Add
void RAD(struct Processor* proc) {
    readDirect(proc);
    proc->regB = Add_Word12(proc->regZ, proc->regA);
    proc->regA = proc->regB;
    proc->regZ = proc->regB;
    
    WRITE(proc);
    RNI(proc);
}
void RAI(struct Processor* proc) {
    readIndirect(proc);
    proc->regB = Add_Word12(proc->regZ, proc->regA);
    proc->regA = proc->regB;
    proc->regZ = proc->regB;

    WRITE(proc);
    RNI(proc);
}
void RAF(struct Processor* proc) {
    readRelForward(proc);
    proc->regB = Add_Word12(proc->regZ, proc->regA);
    proc->regA = proc->regB;
    proc->regZ = proc->regB;

    WRITE(proc);
    RNI(proc);
}
void RAB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regB = Add_Word12(proc->regZ, proc->regA);
    proc->regA = proc->regB;
    proc->regZ = proc->regB;

    WRITE(proc);
    RNI(proc);
}

/*****************************************************/
// Replace Add One
void AOD(struct Processor* proc) {
    readDirect(proc);
    proc->regB = Add_Word12(proc->regZ, ToOnesComplement(1));
    proc->regA = proc->regB;
    proc->regZ = proc->regB;
    WRITE(proc);
    RNI(proc);
}
void AOI(struct Processor* proc) {
    readIndirect(proc);
    proc->regB = Add_Word12(proc->regZ, ToOnesComplement(1));
    proc->regA = proc->regB;
    proc->regZ = proc->regB;
    WRITE(proc);
    RNI(proc);
}
void AOF(struct Processor* proc) {
    readRelForward(proc);
    proc->regB = Add_Word12(proc->regZ, ToOnesComplement(1));
    proc->regA = proc->regB;
    proc->regZ = proc->regB;
    WRITE(proc);
    RNI(proc);
}
void AOB(struct Processor* proc) {
    readRelBackward(proc);
    proc->regB = Add_Word12(proc->regZ, ToOnesComplement(1));
    proc->regA = proc->regB;
    proc->regZ = proc->regB;
    WRITE(proc);
    RNI(proc);
}

/*****************************************************/
// Zero Jump

void ZJF(struct Processor* proc) {
    if (Word12_IsZero(proc->regA)) {
        JNI_FORWARD(proc);
        return;
    }
    RNI(proc);
    return;
}
void ZJB(struct Processor* proc) {
    if (Word12_IsZero(proc->regA)) {
        JNI_BACKWARD(proc);
    }
    RNI(proc);
}

/*****************************************************/
// Non-Zero Jump

void NZF(struct Processor* proc) {
    if (!Word12_IsZero(proc->regA)) {
        JNI_FORWARD(proc);
        return;
    }
    RNI(proc);
    return;
}
void NZB(struct Processor* proc) {
    if (!Word12_IsZero(proc->regA)) {
        JNI_BACKWARD(proc);
        return;
    }
    RNI(proc);
    return;
}

/*****************************************************/
// Positive Jump

void PJF(struct Processor* proc) {
    if ((proc->regA & 0x800) == 0) {
        JNI_FORWARD(proc);
        return;
    }
    RNI(proc);
    return;
}
void PJB(struct Processor* proc) {
    if ((proc->regA & 0x800) == 0) {
        JNI_BACKWARD(proc);
        return;
    }
    RNI(proc);
    return;
}

/*****************************************************/
// Negative Jump

void NJF(struct Processor* proc) {
    if (proc->regA & 0x800) {
        JNI_FORWARD(proc);
    }
    RNI(proc);
}
void NJB(struct Processor* proc) {
    if (proc->regA & 0x800) {
        JNI_BACKWARD(proc);
    }
    RNI(proc);
}

/*****************************************************/
// Indirect Jump (70) - https://homepage.cs.uiowa.edu/~jones/cdc160/man/jump.html
void JPI(struct Processor* proc) {
    proc->regZ = E_REG | 1;
    proc->regB = proc->regZ;
    proc->regA = proc->regB;
    proc->regZ = proc->regB;
}

/*****************************************************/
// Forward Indirect Jump (71)
void JFI(struct Processor* proc) {
    proc->regB = Add_Word12(proc->regP, E_REG);
    proc->regS = proc->regB;
    READ(proc);
    JNI_INDIRECT(proc);
}

/*****************************************************/
// Input (72).
// The input instruction is unique because it spans 3 words:
// 1) <The actual instruction>
// 2) <The starting address of the buffer>
// 3) <The end address of the buffer (one past the last location to store)>

void INP(struct Processor* proc)
{
    uint16_t cycleIdx = 0u;

    proc->regS++;
    READ(proc); // read starting address of buffer
    proc->regB = proc->regZ;
    proc->regA = proc->regB;

step_four:
    // <initiate input>
    
    /* <on resume> : */ proc->regB = proc->regA;
    proc->regS = proc->regB;
    WRITE(proc);
    proc->regB = proc->regA + 1;
    proc->regA = proc->regB;
   
    (cycleIdx == 0) ? (proc->regB = proc->regP + 1) : (proc->regB = proc->regP); //  P + 1 first cycle only; P -> B all successive cycles)
    proc->regS = proc->regB; proc->regP = proc->regS;
    READ(proc);
    proc->regB = Sub_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;
    
    // Sense A = 0 or A ≠ 0
    uint8_t senseIsZero = Word12_IsZero(proc->regA);

    proc->regB = Add_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;

    cycleIdx++;

    if (senseIsZero == 1) {
        // add another 1 to regP becuase the instruction is 3 bytes long
        proc->regB = Add_Word12(proc->regP, 1);
        proc->regS = proc->regB;
        proc->regP = proc->regS;
        // add another 1 to regP becuase the instruction is 3 bytes long
        proc->regB = Add_Word12(proc->regP, 1);
        proc->regS = proc->regB;
        proc->regP = proc->regS;
        RNI(proc);
    } else {
        goto step_four;
    }
}

void OUT(struct Processor* proc) {
    uint16_t cycleIdx = 0u;

    proc->regS++;
    READ(proc); // read starting address of buffer

    proc->regB = proc->regZ;
    proc->regA = proc->regB;
step_four:
    proc->regB = proc->regA;
    proc->regS = proc->regB;

    READ(proc);

    // <initiate output>

    proc->regB = proc->regA + 1;
    proc->regA = proc->regB;
    (cycleIdx == 0) ? (proc->regB = proc->regP + 1) : (proc->regB = proc->regP); //  P + 1 first cycle only; P -> B all successive cycles)

    proc->regS = proc->regB;
    proc->regP = proc->regS;
    READ(proc);

    proc->regB = Sub_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;

    // Sense A = 0 or A ≠ 0
    uint8_t senseIsZero = Word12_IsZero(proc->regA);

    proc->regB = Add_Word12(proc->regA, proc->regZ);
    proc->regA = proc->regB;

    if (senseIsZero == 1) {
        // add another 1 to regP becuase the instruction is 3 bytes long
        proc->regB = Add_Word12(proc->regP, 1);
        proc->regS = proc->regB;
        proc->regP = proc->regS;
        // add another 1 to regP becuase the instruction is 3 bytes long
        proc->regB = Add_Word12(proc->regP, 1);
        proc->regS = proc->regB;
        proc->regP = proc->regS;
        RNI(proc);
    }
    else {
        goto step_four;
    }
}

void OTN(struct Processor* proc)
{
    mainframe->outputLine = E_REG;
    mainframe->outputReadyLine = true;
    WAIT_FOR_RESUME(proc);
    mainframe->outputReadyLine = false;
    RNI(proc);
}

/*
The 75 (EXF) instruction signals the appropriate peripheral device to connect to
the input or output lines.
*/
void EXF(struct Processor* proc) {
    /*
    EXF (75) External Function
    The External Function instruction places a 12-bit code on the output lines and places a
    signal on the external function ready line. Upon receiving this signal, all external
    devices examine the code contained on the output lines. If a device recognizes its external function code,
    it sends an acknowledgment (resume signal) to the computer,
    which then proceeds to the next instruction. If no external unit sends back a resume,
    the computer will stop and display a SEL signal on the console.
    */

    // read the function code, which is E positions from the EXF instruction.
    readRelForward(proc);
    
    

    // set external function lines and output lines
     
    // external function octal 4102 - input from photoelectric tape reader
    // external function octal 4104 - output to built-in paper tape punch
    // 
    // external function octal 4210- output to typewriter
    // external function octal 4220 - input from typewriter
    // external function octal 4240 - request typewriter status

    mainframe->outputLine = proc->regZ;
    mainframe->functionReadyLine = true;
    
    WAIT_FOR_RESUME(proc);

    mainframe->functionReadyLine = false;
    RNI(proc);

    
    

    //if (proc->regZ == 04104) {

        //CDC_160_SetOutputLines(proc->regZ);

        //mainframe.

        /* possible return cards for card reader or punch
        > 0400 no card in magazine
        >2000 failed to feed card
        > 1000 card stacker full 
        > 4000 not ready*/

        // On paper tape and other non-block-transfer devices once an input or output selection has been made by the EXF instruction any number of successive inputs or outputs may be made without another EXF.
        
        // the tape punch is output only, so we don't need to wait for it to respond.
        
        // this is done later
        /*TeletypeModelBRPE_PunchCharIntoTape(&mainframe->tapePunch, c, mainframe->tapePunch.tape);

        mainframe->tapePunch;*/
    //}
}

void INA(struct Processor* proc) {
    mainframe->inputRequestLine = true;
    WAIT_FOR_RESUME(proc);

    // <initiate output>
    /* <on resume> : */ proc->regB = proc->regZ;
    proc->regA = proc->regB;
    RNI(proc);
}