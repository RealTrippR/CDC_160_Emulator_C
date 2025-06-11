#include "paper_tape.h"
#include <stdio.h>
#include <string.h>


void intToStr(int N, char* buff, size_t buffSize) { // https://www.geeksforgeeks.org/how-to-convert-an-integer-to-a-string-in-c/
	if (buffSize == 0) {
		return;
	}
	int i = 0;

	// Save the copy of the number for sign
	int sign = N;

	// If the number is negative, make it positive
	if (N < 0)
		N = -N;

	// Extract digits from the number and add them to the
	// string
	while (N > 0) {
		if (i == buffSize - 2) {
			goto bail;
		}
		// Convert integer digit to character and store
		// it in the str
		buff[i] = N % 10 + '0';
		i++;
		N /= 10;
	}

	// If the number was negative, add a minus sign to the
	// string
	if (sign < 0) {
		if (i == buffSize - 2) {
			goto bail;
		}
		buff[i] = '-';
		i++;
	}

	// Reverse the string to get the correct order
	for (int j = 0, k = i - 1; j < k; j++, k--) {
		char temp = buff[j];
		buff[j] = buff[k];
		buff[k] = temp;
	}

bail:

	// Null-terminate the string
	buff[i] = '\0';
	buff[buffSize - 1] = '\0';
}





bool PaperTape1Inch_SaveToDisk(struct PaperTape1Inch* tape, const char* filename) {
	if (tape->data != NULL) {
		FILE* fptr;

		fopen_s(&fptr, filename, "wb");
		if (fptr == NULL) {
			return 0;
		}

		char num[10];
		intToStr(tape->level, num, sizeof(num));
		fwrite("<tape level:", 1, 13, fptr);
		fwrite(num, 0, 9, fptr);
		fwrite(">",0,1,fptr);
		fwrite(tape->data, 1, tape->rowCount, fptr);
		
		fclose(fptr);

		return 1;
	}

	return 0;
}

bool PaperTape1Inch_ReadFromDisk(struct PaperTape1Inch* tape, const char* filename) {
	if (tape->data == NULL) {
		FILE* fptr;

		fopen_s(&fptr, filename, "rb");
		if (fptr == NULL) {
			return 0;
		}
	
		fseek(fptr, 0L, SEEK_END);
		size_t fsize = ftell(fptr);
		rewind(fptr);

		fseek(fptr, 13, SEEK_SET);

		uint16_t tapeLevel = 0u;
		char c = fgetc(fptr);
		while (c != EOF && c != '>')
		{
			static size_t n=0u;
			n+=10;
			tapeLevel += (c - 48) * n;
		}
		if (tapeLevel > 8) {
			goto bail;
		}

		rewind(fptr);


		char* buff = malloc(fsize);
		if (!buff) {
			goto bail;
		}

		fread_s(buff, fsize, 1, fsize, fptr);




		tape->data = buff;
		tape->rowCount = fsize;
		tape->level = tapeLevel;
	bail:
		fclose(fptr);
		return 1;
	}
	else {
		return 0;
	}
}

uint32_t PaperTape1Inch_GetLengthIn(struct PaperTape1Inch* tape) {
	return tape->rowCount / 10; // 10 rows per inch
}

void PaperTape1Inch_Create(struct PaperTape1Inch* tape, uint32_t lengthInInches) {
	if (tape->data) {
		free(tape->data);
	}

	tape->level = 7;
	tape->rowCount = (lengthInInches) * 10; // 10 rows per inch
	tape->data = malloc(tape->rowCount); // max of 8 holes per row

	memset(tape->data, 0, tape->rowCount);
}

void PaperTape1Inch_Destroy(struct PaperTape1Inch* tape) {
	if (tape->data) {
		free(tape->data);
	}
}