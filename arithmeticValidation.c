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
	char res_int_signed[8];
	SignedWord12toStr(res, res_int_signed);
	char res_int_unsigned[8];
	Word12toStr(res, res_int_unsigned);
	char res_oct[8];
	Word12toStrOctal(res, res_oct);

	printf("\n");
	printf("    %4s | %12s\n", astr_int, astr_bin);
	printf("(-) %4s | %12s\n", bstr_int, bstr_bin);
	printf("-----------------------\n");
	printf("%4s,%4s| %12s \n", res_int_signed, res_int_unsigned, res_bin);
	printf("  octal: | %s\n", res_oct);
}

void add_print(Word12 A, Word12 B) {
	char astr_bin[13];
	char bstr_bin[13];

	Word12ToStrBinary(A, astr_bin);
	Word12ToStrBinary(B, bstr_bin);

	char astr_int[8] = { 0 };
	char bstr_int[8] = { 0 };

	Word12toStr(A, astr_int);
	Word12toStr(B, bstr_int);


	Word12 res = Add_Word12(A, B);

	char res_bin[13];
	Word12ToStrBinary(res, res_bin);
	char res_int_signed[8];
	SignedWord12toStr(res, res_int_signed);
	char res_int_unsigned[8];
	Word12toStr(res, res_int_unsigned);
	char res_oct[8];
	Word12toStrOctal(res, res_oct);

	printf("\n");
	printf("    %4s | %12s\n", astr_int, astr_bin);
	printf("(+) %4s | %12s\n", bstr_int, bstr_bin);
	printf("-----------------------\n");
	printf("%4s,%4s| %12s \n", res_int_signed, res_int_unsigned, res_bin);
	printf("  octal: | %s\n", res_oct);
}
int main()
{
	// validate subtraction. See pages 12 and 33, respectively:
	// http://www.bitsavers.org/pdf/cdc/160/CDC160A/60014500G_CDC160A_Reference_Manual_196503.pdf
	// https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
	
	sub_print(15, 5);

	sub_print(07777, 0);

	add_print(4, 3);
	
	add_print(1, 4);

	add_print(07105, 055);

	add_print(07776, 01);

	sub_print(07776, 07776);

	add_print(07777, 07777);

	sub_print(07777, 0);

	add_print(07776, 1);

}