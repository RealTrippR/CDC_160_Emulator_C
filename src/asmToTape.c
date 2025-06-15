#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "word_12.h"
#include "asmToTape.h"
#include "op_code_enum.h"

enum TokenType {
	NONE,
	ASM_CODE,
	CONSTANT_VAL,
	VARIABLE,
	//CONTROL_CODE: NOT YET IMPLEMENTED
};

enum AddressingMode {
	NONE_AM,
	DIRECT_AM,
	INDIRECT_AM,
	FORWARD_AM,
	BACKWARD_AM
};

struct Token {
	enum TokenType type;
	Word12 address;
	Word12 valF; /*F value*/
	Word12 valE; /*E value*/

	char varName[16]; // this is a var name, typically empty
};

struct TokenizedLine {
	struct Token tokens[3];
};

int getTokenCountOfTokenizedLine(struct TokenizedLine* line) 
{
	int c = 0;
	if (line->tokens[0].type != NONE) {
		c++;
	}
	if (line->tokens[1].type != NONE) {
		c++;
	}
	if (line->tokens[1].type != NONE) {
		c++;
	}
	return c;
}


int atoOn(const char* str, size_t len) {
	int result = 0;
	int sign = 1;
	size_t i = 0;

	// Skip leading whitespace and 9s
	while (i < len && (isspace((unsigned char)str[i]) || str[i] == '0')) 
	{
		i++;
	}

	// Handle optional sign
	if (i < len && (str[i] == '-' || str[i] == '+')) {
		if (str[i] == '-') {
			sign = -1;
		}
		i++;
	}

	// Parse digits
	while (i < len && (str[i] > '0' && str[i] < '8')) {
		result = result * 8 + (str[i] - '0');
		i++;
	}

	return sign * result;
}

int atoin(const char* str, size_t len) {
	int result = 0;
	int sign = 1;
	size_t i = 0;

	// Skip leading whitespace
	while (i < len && isspace((unsigned char)str[i])) {
		i++;
	}

	// Handle optional sign
	if (i < len && (str[i] == '-' || str[i] == '+')) {
		if (str[i] == '-') {
			sign = -1;
		}
		i++;
	}

	// Parse digits
	while (i < len && isdigit((unsigned char)str[i])) {
		result = result * 10 + (str[i] - '0');
		i++;
	}

	return sign * result;
}


bool isStrNumber(const char* str, size_t len) {
	for (size_t i = 0; i < len; ++i) {
		if (str[i] < '0' || str[i] > '9') {
			return false;
		}
	}

	return true;
}
// returns the size of the new str
// removes all blank spaces and comments
size_t trimSpaces(char* dstBuff, const size_t dstBuffSze, const char* srcBuff, const size_t srcBuffSize) 
{
	if (dstBuffSze == 0u) {
		return 0;
	}

	size_t cpyLen = 0;
	if (dstBuffSze < srcBuffSize) {
		cpyLen = dstBuffSze;
	}
	else {
		cpyLen = srcBuffSize;
	}

	bool isFirstSpace = false;
	bool alreadyTrailingSpace = false;
	size_t j = 0;
 	for (size_t i = 0; i < cpyLen; ++i) {
		if (srcBuff[i] == ';') {
			break;
		}
		if (j > 0 && alreadyTrailingSpace == false && (srcBuff[i] == '\t' || srcBuff[i] == ' ' || srcBuff[i] == '\r')) {
			// only add 1 space, ignore the rest and leading spaces
			alreadyTrailingSpace = true;
			dstBuff[j] = ' ';
			j++;
		} else if (! (srcBuff[i] == '\t' || srcBuff[i] == ' ' || srcBuff[i] == '\r')) {
			// add char if it's not a space
			alreadyTrailingSpace = false;
			dstBuff[j] = srcBuff[i];
			j++;
		}
	}

	if (j == 0) {
		return 0;
	}
	else {
		j--;
	}
	
	dstBuff[j] = '\0';

	for (j; j > 0; --j) {
		char c = dstBuff[j];
		if (!(dstBuff[j] == ' ' || dstBuff[j] == '\t' || dstBuff[j] == '\r')) {
			break; // break if a char is found
		}
		else {
			// turn blanks to null terminators
			dstBuff[j] = '\0';
		}
	}


	dstBuff[dstBuffSze - 1] = '\0';

	return j ;
}

void Token_New(struct Token* t) {
	t->address = 0x0;
	t->valE = 0x0;
	t->valF = 0x0;
	t->varName[sizeof(t->varName) - 1] = '\0'; t->varName[0] = '\0';
	t->type = NONE;
}

bool str3eql(const char* s1, const char* s2) {
	for (uint8_t i = 0; i < 3; ++i) {
		if (s1[i] != s2[i]) {
			return false;
		}
	}
	return true;
}

enum AddressingMode getAddressingModeFromFunctionCode(const enum FunctionCode F_REG)
{
	// The Z-Higher is laid out like so:
	// +-----------+-------+
	// |  0 0 0 0  |  0 0  |
	//   <F CODE>  <AD. CODE>

	// AD. CODE, BINARY:
	// N: 10 (but all bits above 3 are 0) 
	// D: 00
	// I: 01
	// F: 10
	// B: 11
	const uint8_t AD_CODE = F_REG & 03;
	

	if (AD_CODE == 0b00) 
	{
		return DIRECT_AM;
	}
	else if (AD_CODE == 0b01)
	{
		return INDIRECT_AM;
	}
	else if (AD_CODE == 0b10) 
	{
		const uint8_t bitsAbove3 = F_REG >> 3;
		if (bitsAbove3 > 0) {
			return FORWARD_AM;
		}
		return NONE_AM;
	}
	else if (AD_CODE == 0b11) 
	{
		return BACKWARD_AM;
	}
}






enum FunctionCode strToFCode(const char* str, uint8_t* eValue) {
	*eValue = 0x0;
	if (str3eql(str, "HLT")) {
		*eValue = 0x0;
		return HLT_E;
	} 
	else if (str3eql(str, "ERR")) {
		*eValue = 0x3F;
		return ERR_E;
	}
	/**********************/
	else if (str3eql(str, "PTA")) {
		*eValue = 0x1;
		return PTA_E;
	}
	else if (str3eql(str, "STP")) {
		return SHA_E;
	}
	else if (str3eql(str, "SHA")) {
		return SHA_E;
	}
	/**********************/
	else if (str3eql(str, "LPN")) {
		return LPN_E;
	}
	else if (str3eql(str, "LPD")) {
		return LPD_E;
	}
	else if (str3eql(str, "LPI")) {
		return LPI_E;
	}
	else if (str3eql(str, "LPF")) {
		return LPF_E;
	}
	else if (str3eql(str, "LPB")) {
		return LPB_E;
	}
	/**********************/
	else if (str3eql(str, "LSN")) {
		return LSN_E;
	}
	else if (str3eql(str, "LSD")) {
		return LSD_E;
	}
	else if (str3eql(str, "LSI")) {
		return LSI_E;
	}
	else if (str3eql(str, "LSF")) {
		return LSF_E;
	}
	else if (str3eql(str, "LSB")) {
		return LSB_E;
	}
	/**********************/
	else if (str3eql(str, "LDN")) {
		return LDN_E;
	}
	else if (str3eql(str, "LDD")) {
		return LDD_E;
	}
	else if (str3eql(str, "LDI")) {
		return LDI_E;
	}
	else if (str3eql(str, "LDF")) {
		return LDF_E;
	}
	else if (str3eql(str, "LDB")) {
		return LDB_E;
	}
	/**********************/
	else if (str3eql(str, "LCN")) {
		return LCN_E;
	}
	else if (str3eql(str, "LCD")) {
		return LCD_E;
	}
	else if (str3eql(str, "LCI")) {
		return LCI_E;
	}
	else if (str3eql(str, "LCF")) {
		return LCF_E;
	}
	else if (str3eql(str, "LCB")) {
		return LCB_E;
	}
	/**********************/
	else if (str3eql(str, "ADN")) {
		return ADN_E;
	}
	else if (str3eql(str, "ADD")) {
		return ADD_E;
	}
	else if (str3eql(str, "ADI")) {
		return ADI_E;
	}
	else if (str3eql(str, "ADF")) {
		return ADF_E;
	}
	else if (str3eql(str, "ADB")) {
		return ADB_E;
	}
	/**********************/
	else if (str3eql(str, "SBN")) {
		return SBN_E;
	}
	else if (str3eql(str, "SBD")) {
		return SBD_E;
	}
	else if (str3eql(str, "SBI")) {
		return SBI_E;
	}
	else if (str3eql(str, "SBF")) {
		return SBF_E;
	}
	else if (str3eql(str, "SBB")) {
		return SBB_E;
	}
	/**********************/
	else if (str3eql(str, "STD")) {
		return STD_E;
	}
	else if (str3eql(str, "STI")) {
		return STI_E;
	}
	else if (str3eql(str, "STF")) {
		return STF_E;
	}
	else if (str3eql(str, "STB")) {
		return STB_E;
	}
	/**********************/
	else if (str3eql(str, "SRD")) {
		return SRD_E;
	}
	else if (str3eql(str, "SRI")) {
		return SRI_E;
	}
	else if (str3eql(str, "SRF")) {
		return SRF_E;
	}
	else if (str3eql(str, "SRB")) {
		return SRB_E;
	}
	/**********************/
	else if (str3eql(str, "RAD")) {
		return RAD_E;
	}
	else if (str3eql(str, "RAI")) {
		return RAI_E;
	}
	else if (str3eql(str, "RAF")) {
		return RAF_E;
	}
	else if (str3eql(str, "RAB")) {
		return RAB_E;
	}
	/**********************/
	else if (str3eql(str, "AOD")) {
		return AOD_E;
	}
	else if (str3eql(str, "AOI")) {
		return AOI_E;
	}
	else if (str3eql(str, "AOF")) {
		return AOF_E;
	}
	else if (str3eql(str, "AOB")) {
		return AOB_E;
	}
	/**********************/
	else if (str3eql(str, "ZJF")) {
		return ZJF_E;
	}
	else if (str3eql(str, "ZJB")) {
		return ZJB_E;
	}
	/**********************/
	else if (str3eql(str, "NZF")) {
		return NZF_E;
	}
	else if (str3eql(str, "NZB")) {
		return NZB_E;
	}
	/**********************/
	else if (str3eql(str, "PJF")) {
		return PJF_E;
	}
	else if (str3eql(str, "PJB")) {
		return PJB_E;
	}
	/**********************/
	else if (str3eql(str, "NJF")) {
		return NJF_E;
	}
	else if (str3eql(str, "NJB")) {
		return NJB_E;
	}
	/**********************/
	else if (str3eql(str, "JPI")) {
		return JPI_E;
	}
	/**********************/
	else if (str3eql(str, "JFI")) {
		return JFI_E;
	}
	/**********************/
	else if (str3eql(str, "INP")) {
		return INP_E;
	}
	/**********************/
	else if (str3eql(str, "OUT")) {
		return OUT_E;
	}
	/**********************/
	else if (str3eql(str, "OTN")) {
		return OTN_E;
	}
	/**********************/
	else if (str3eql(str, "EXF")) {
		return EXF_E;
	}
	/**********************/
	else if (str3eql(str, "INA")) {
		return INA_E;
	}

	return INVALID_CODE_E;
}





enum TokenType getTokenTypeOfWord(const char* word, size_t wordLen, Word12* fValue /*OPTIONAL*/, Word12* eValue/*OPTIONAL*/) {
	uint8_t tmpE;
	if (eValue == NULL) {
		eValue = &tmpE;
	}
	uint8_t tmpF;
	if (fValue == NULL) {
		fValue = &tmpF;
	}

	enum FunctionCode fc = strToFCode(word, &eValue);
	if (fc != INVALID_CODE_E) {
		*fValue = fc;
		return ASM_CODE;
	}
	else if (isStrNumber(word, wordLen)) {
		return CONSTANT_VAL;
	}
	else {
		return VARIABLE;
	}
	return NONE;
}


void TokenizedLine_Create(struct TokenizedLine* TL, const char* line, size_t lineIdx, size_t lineLen) 
{

	Token_New(&(TL->tokens[0]));
	Token_New(&(TL->tokens[1]));
	Token_New(&(TL->tokens[2]));

	size_t wordIdx = 0;

	size_t wordSize = 0;
	char* wordBegin = line;

	// read each word in the line
	for (size_t k = 0; k <= lineLen; ++k) {
		if (line[k] == ' ' || line[k] == '\r' || line[k] == '\t' || k == lineLen) {
			struct Token* tk = &TL->tokens[wordIdx];

			tk->type = getTokenTypeOfWord(wordBegin, wordSize, &(tk->valF), &(tk->valE));

			tk->address = lineIdx;

			if (tk->type == VARIABLE) {
				memcpy_s(tk->varName, sizeof(tk->varName), wordBegin, wordSize);
				tk->varName[wordSize] = '\0';
			}
			else if (tk->type == CONSTANT_VAL) {
				tk->valE = atoOn(wordBegin, wordSize);
			}
			else if (tk->type == ASM_CODE) {
				tk->valF = strToFCode(wordBegin, &tk->valE);
			}

			wordBegin = line + k + 1;
			wordSize = 0u;
			wordIdx++;
		}
		else {
			wordSize++;
		}
	}



	for (uint16_t i = 0; i < 3-1; ++i) {
		if (TL->tokens[i].type == NONE && TL->tokens[i+1].type != NONE) {
			TL->tokens[i] = TL->tokens[i + 1];
			TL->tokens[i + 1].type = NONE;
		}
	}

}


bool asmToTape(const struct PaperTape1Inch* tape, const char* dstFilename) 
{
	FILE* fptr = NULL;

	bool retCode = true;

	fopen_s(&fptr, dstFilename, "rb");
	if (fptr != NULL) {
		fseek(fptr, 0L, SEEK_END);
		size_t fsize = ftell(fptr);
		rewind(fptr);

		char* srcCode = malloc(fsize+1);

		if (!srcCode) {
			fclose(fptr);
			return false;
		}

		// read asm instructions into data
		fread_s(srcCode, fsize, 1, fsize, fptr);
		srcCode[fsize] = '\0';

		fclose(fptr);


		// counts the number of lines, where a line is defined as
		// a block of data seperated by a null-terminator or newline character, where the size of the line, excluding comments, is greater than 0.
		size_t srcLineCount = 0; // includes blank or commented out lines
		size_t fLineCount = 0; // does not include blank or commented out lines
		size_t lsze = 0;
		bool c = false;
		for (size_t i = 0; i < fsize; ++i) {
			if (srcCode[i] == ';') { /*ignore comments*/
				c = true;
			}
			if (c == false && !(isspace((unsigned char)srcCode[i]))) {
				lsze++;
			}
			if (srcCode[i] == '\n' || i == fsize-1) {
				srcLineCount++;
				if (lsze > 0) {
					fLineCount++;
				}
				c = false;
				lsze = 0;
			}
		}

		
		char* beginningOfLine = srcCode;
		char* endOfLine = beginningOfLine;
		size_t lineLen = 0;

		struct TokenizedLine* tokenizedLines = malloc(sizeof(struct TokenizedLine) * fLineCount + 1);
		memset(tokenizedLines, 0, sizeof(struct TokenizedLine) * fLineCount + 1);
		size_t tokenizedLinesCount = 0;

		if (!tokenizedLines) {
			return false;
		}

		size_t li = 0;
		// read src code line by line
		for (size_t i = 0; i < srcLineCount; ++i) {

			// get end of line
			for (uint32_t ci = 0; ci < UINT16_MAX; ci++) {
				if (beginningOfLine[ci] == '\n' || beginningOfLine[ci] == '\0') {
					endOfLine = beginningOfLine + ci;
					break;
				}
			}
			size_t srcLineLen = endOfLine - beginningOfLine;
			char line[64];
			size_t lineLen = trimSpaces(line, sizeof(line), beginningOfLine, srcLineLen);
			if (lineLen > 0) {
				TokenizedLine_Create(&tokenizedLines[li], line, li, lineLen);
				li++;
			}
			

			if (i != srcLineCount - 1) {
				// set new beginning of line
				beginningOfLine = endOfLine + 1;
			}
		}

		tokenizedLinesCount = li;


		struct Token* varTokens = malloc(sizeof(struct Token)*fLineCount + 1);
		size_t varTokensCount = 0u;


		// find var tokens
		for (size_t i = 0; i < tokenizedLinesCount; ++i)
		{
			if (tokenizedLines[i].tokens[0].type == VARIABLE) {
				varTokens[varTokensCount] = tokenizedLines[i].tokens[0];
 				varTokensCount++;
			}
		}
		


		size_t curTapeFrame = 0;
		// 10 frames per inch
		PaperTape1Inch_Create(tape, (float)(tokenizedLinesCount / 5) + 1);


		// convert asm to assembly, line by line
		for (size_t i = 0; i < tokenizedLinesCount; ++i) {
			Word12 asmInst = 0x0;

			struct TokenizedLine line = tokenizedLines[i];

			// iterate over tokens
			enum AddressingMode am = NONE;
			for (uint16_t j = 0; j < 3; ++j) {
				// it's a variable declaration, skip word
				if (j == 0 && line.tokens[j].type == VARIABLE && (getTokenCountOfTokenizedLine(&line) > 1)) {
					continue;
				}
				else if (j == 0 && line.tokens[j].type == VARIABLE && (getTokenCountOfTokenizedLine(&line) == 1))
				{
					struct Token* varTK = NULL;
					// resolve var token
					for (size_t k = 0; k < varTokensCount; ++k) {
						if (strcmp(varTokens[k].varName, line.tokens[j].varName) == 0) {
							varTK = &varTokens[k];
							break;
						}
					}

					if (!varTK) {
						printf("Compile error, line %zu: Failed to find variable reference '%s'\n", i, line.tokens[j].varName);
						goto bail;
					}

					asmInst |= varTK->address;
					continue;
				}

				struct Token t = line.tokens[j];

				if (line.tokens[j].type == ASM_CODE) {
					asmInst |= (line.tokens[j].valF << 6);
					am = getAddressingModeFromFunctionCode(line.tokens[j].valF);
				}
				else if (line.tokens[j].type == CONSTANT_VAL) {

					uint16_t offset = 0;
					if (line.tokens[0].type == VARIABLE) {
						offset = 1;
					}

					if (j - offset == 0) {
						// F
						asmInst |= (line.tokens[j].valE << 6);
					}
					if (j - offset == 1) {
						// E
						asmInst |= line.tokens[j].valE;
					}
				}
				else if (line.tokens[j].type == VARIABLE) {

					struct Token* varTK = NULL;
					// resolve var token
					for (size_t k = 0; k < varTokensCount; ++k) {
						if (strcmp(varTokens[k].varName, line.tokens[j].varName) == 0) {
							varTK = &varTokens[k];
							break;
						}
					}

					if (!varTK) {
						printf("Compile error, line %zu: Failed to find variable reference '%s'\n", i, line.tokens[j].varName);
						goto bail;
					}


					if (varTK->address < 64 && (am == NONE_AM || am == DIRECT_AM || am == INDIRECT_AM))
					{
						asmInst |= varTK->address;
					}
					else if (am == FORWARD_AM || am == BACKWARD_AM) {
						if (am == FORWARD_AM) {
							if (line.tokens[j].address > varTK->address) {
								printf("Compile error, line %zu: ASM instruction uses forward addressing, but the variable that it is referencing '%s' comes before the ASM instruction.\n", i, line.tokens[j].varName);
							}
						}
						else {
							if (line.tokens[j].address < varTK->address) {
								printf("Compile error, line %zu: ASM instruction uses backward addressing, but the variable that it is referencing '%s' comes after the ASM instruction.\n", i, line.tokens[j].varName);
							}
						}
						size_t addressDiffAbs = abs(varTK->address - line.tokens[j].address);
						// note that the E value must be less than 64.
						if (addressDiffAbs < 64) {
							asmInst |= addressDiffAbs;
						}
						else {
							printf("Compile error, line %zu: Forward and backward addressing allows for at most a difference of 63 between the operand and the ASM instruction.\n", i);
						}
					}
				}
			}

			if (getTokenCountOfTokenizedLine(&line) == 2 && line.tokens[0].type == ASM_CODE
				|| getTokenCountOfTokenizedLine(&line) == 3 && line.tokens[1].type == ASM_CODE)
			{

				Word12 fcode = (asmInst >> 6) & 0x3F;
				if (fcode == HLT_E) {
					asmInst = 0b000000000000;
				}
				else if (fcode == ERR_E) {
					asmInst = 0b111111111111;
				}
				else if (fcode == PTA_E) {
					asmInst = 0b000001000001;
				}
			}





			if ((int64_t)curTapeFrame > (int64_t)tape->rowCount-2)
			{
				printf("Compile error: tape is not long enough to hold all the ASM instructions!");
				goto bail;
			}

			// the ASM instruction has been formed, now write it to the tape

			tape->data[curTapeFrame] = 0b0000000; // 7th level of 1st frame should be punched
			tape->data[curTapeFrame] |= (asmInst >> 6) & 0x3F; // higher order bits
			const char dbgc = (asmInst >> 6) & 0x3F;
			curTapeFrame++;

			if (i == tokenizedLinesCount - 1) {
				// by having no 7th level punch on the last 2nd frame we tell the reader to stop
				tape->data[curTapeFrame] = 0b1000000;
			}
			else {
				// the 2nd frame should have a punch on the 7th level
				tape->data[curTapeFrame] = 0b0000000;
			}
			tape->data[curTapeFrame] |= (asmInst) & 0x3F; // lower order bits
			curTapeFrame++;

		}





	bail:

		if (tokenizedLines) {
			free(tokenizedLines);
		}
		if (varTokens) {
			free(varTokens);
		}
		tokenizedLines = NULL;
		varTokens = NULL;
	}
	else {
		printf("Failed to read tape source file.\n");
		return false;
	}

	return true;
}