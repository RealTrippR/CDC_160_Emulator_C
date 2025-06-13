// see page 6: https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf

#ifndef CDC_OP_CODE_ENUM_H

enum ControlInstruction {
    HALT,
    ERROR,
    P_TO_A // only present in serial number 37 and above.
};

enum FunctionCode
{
    /**********************/
    ERR_E = 000,
    HLT_E = 077,
    /**********************/
    PTA_E = 01,  /*This instruction was present in serial number 37 and above. It sets A to P. If the E register is 1, it will be called instead of SHA_E.*/
    SHA_E = 01,
    /**********************/
    LPN_E = 02,
    LPD_E = 010,
    LPI_E = 011,
    LPF_E = 012,
    LPB_E = 013,
    /**********************/
    LSN_E = 003,
    LSD_E = 014,
    LSI_E = 015,
    LSF_E = 016,
    LSB_E = 017,
    /**********************/
    LDN_E = 004,
    LDD_E = 020,
    LDI_E = 021,
    LDF_E = 022,
    LDB_E = 023,
    /**********************/
    LCN_E = 005,
    LCD_E = 024,
    LCI_E = 025,
    LCF_E = 026,
    LCB_E = 027,
    /**********************/
    ADN_E = 006,
    ADD_E = 030,
    ADI_E = 031,
    ADF_E = 032,
    ADB_E = 033,
    /**********************/
    SBN_E = 007,
    SBD_E = 034,
    SBI_E = 035,
    SBF_E = 036,
    SBB_E = 037,
    /**********************/
    STD_E = 040,
    STI_E = 041,
    STF_E = 042,
    STB_E = 043,
    /**********************/
    SRD_E = 044,
    SRI_E = 045,
    SRF_E = 046,
    SRB_E = 047,
    /**********************/
    RAD_E = 050,
    RAI_E = 051,
    RAF_E = 052,
    RAB_E = 053,
    /**********************/
    AOD_E = 054,
    AOI_E = 055,
    AOF_E = 056,
    AOB_E = 057,
    /**********************/
    ZJF_E = 060,
    ZJB_E = 064,
    /**********************/
    NZF_E = 061,
    NZB_E = 065,
    /**********************/
    PJF_E = 062,
    PJB_E = 066,
    /**********************/
    NJF_E = 063,
    NJB_E = 067,
    /**********************/
    JPI_E = 070,
    /**********************/
    JFI_E = 071,
    /**********************/
    INP_E = 072,
    /**********************/
    OUT_E = 073,
    /**********************/
    OTN_E = 074,
    /**********************/
    EXF_E = 075,
    /**********************/
    INA_E = 076,
    /**********************/


    INVALID_CODE_E = 0100
};

#endif // !CDC_OP_CODE_ENUM_H