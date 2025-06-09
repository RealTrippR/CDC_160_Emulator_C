#ifndef CDC_160_FUNCTION_DURATION_TBL_H
#define CDC_160_FUNCTION_DURATION_TBL_H

const float FuncDurTableUsec[] = {
	6.4, //ERR
	6.4, //HLT
	6.4, //SHA

	6.4, //LPN
	12.8,//LPD
	19.2,//LPI
	12.8,//LPD
	12.8,//LPB
	
	6.4, //LSN
	12.8,//LSD
	19.2,//LSI
	12.8,//LSD
	12.8,//LSB

	6.4, //LDN
	12.8,//LDD
	19.2,//LDI
	12.8,//LDD
	12.8,//LDB

	6.4, //LCN
	12.8,//LCD
	19.2,//LCI
	12.8,//LCD
	12.8,//LCB

	6.4, //ADN
	12.8,//ADD
	19.2,//ADI
	12.8,//ADD
	12.8,//ADB

	6.4, //SBN
	12.8,//SBD
	19.2,//SBI
	12.8,//SBD
	12.8,//SBB

	19.2,//STD
	25.6,//STI
	19.2,//STD
	19.2,//STB

	19.2,//SRD
	25.6,//SRI
	19.2,//SRD
	19.2,//SRB

	19.2,//RAD
	25.6,//RAI
	19.2,//RAD
	19.2,//RAB
};



#endif // !CDC_160_FUNCTION_DURATION_TBL_H
