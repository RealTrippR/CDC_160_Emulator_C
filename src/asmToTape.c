#include "asmToTape.h"
#include "op_code_enum.h"
#include <stdio.h>
#include <string.h>

struct token {
	enum Label label;
	enum FunctionCode fCode;
	uint8_t eValue;
};

enum FunctionCode strToFCode(const char* str, uint8_t* eValue) {
	eValue = 0x0;
	if (strcmp(str, "HLT") == 0) {
		*eValue = 0x0;
		return HLT_E;
	} 
	else if (strcmp(str, "ERR") == 0) {
		*eValue = 0x3F;
		return ERR_E;
	}
	/**********************/
	else if (strcmp(str, "SHA") == 0) {
		return SHA_E;
	}
	/**********************/
	else if (strcmp(str, "LPN") == 0) {
		return LPN_E;
	}
	else if (strcmp(str, "LPD") == 0) {
		return LPD_E;
	}
	else if (strcmp(str, "LPI") == 0) {
		return LPI_E;
	}
	else if (strcmp(str, "LPF") == 0) {
		return LPF_E;
	}
	else if (strcmp(str, "LPB") == 0) {
		return LPB_E;
	}
	/**********************/
	else if (strcmp(str, "LSN") == 0) {
		return LSN_E;
	}
	else if (strcmp(str, "LSD") == 0) {
		return LSD_E;
	}
	else if (strcmp(str, "LSI") == 0) {
		return LSI_E;
	}
	else if (strcmp(str, "LSF") == 0) {
		return LSF_E;
	}
	else if (strcmp(str, "LSB") == 0) {
		return LSB_E;
	}
	/**********************/

}

bool asmToTape(const struct PaperTape1Inch* tape, const char* dstFilename) 
{
	FILE* fptr = NULL;

	bool retCode = true;

	fopen_s(&fptr, dstFilename, "rb");
	if (fptr == NULL) {
		fseek(fptr, 0L, SEEK_END);
		size_t fsize = ftell(fptr);
		rewind(fptr);

		char* data = malloc(fsize);

		if (!data) {
			fclose(fptr);
			return false;
		}
		// read asm instructions into data
		fread_s(data, fsize, 1, fsize, fptr);


		// note that all numbers are in octal

		// Parse labels, assign addresses

		// look for START <instruction>


		// Look for LIM <address, value>

	}

	return true;
}