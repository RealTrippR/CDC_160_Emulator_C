#include "src/word_12.h"
#include <stdio.h>

void sub_print(Word12 A, Word12 B) {
	char astr_bin[13];
	char bstr_bin[13];
	
	Word12ToStrBinary(A, astr_bin);
	Word12ToStrBinary(B, bstr_bin);

	char astr_int[8] = { 0 };
	char bstr_int[8] = { 0 };

	Word12toStr(A, astr_int);
	Word12toStr(B, bstr_int);


	Word12 res = Sub_Word12(A, B);

	char res_bin[13];
	Word12ToStrBinary(res, res_bin);
	char res_int[8];
	SignedWord12toStr(res, res_int);
	char res_oct[8];
	Word12toStrOctal(res, res_oct);

	printf("\n");
	printf("    %4s | %12s\n", astr_int, astr_bin);
	printf("(-) %4s | %12s\n", bstr_int, bstr_bin);
	printf("-----------------------\n");
	printf("    %4s | %12s \n", res_int, res_bin);
	printf("  octal: | %s\n", res_oct);
}
int main()
{
	// validate subtraction. See page 12 and 33, respectively:
	// http://www.bitsavers.org/pdf/cdc/160/CDC160A/60014500G_CDC160A_Reference_Manual_196503.pdf
	// https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
	sub_print(555, 555);
	//add_print(1, 4);
}