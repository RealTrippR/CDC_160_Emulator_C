; Performs operation one hundred times (operation takes 700 location).
; Unless specified otherwise, all numbers are in octal notation.
LIM		01		44 ; load 01 into F reg and 44 into E, reg, resulting in 100 decimal
START	LCB		LIM ; replace A with the ones complement of 100 decimal, -100 decimal
		STD		77 ; replace counter at address 63 decimal with contains of A (-100 decimal)
OPN		AOD		77 ; operation -> increment counter at address 63 decimal by 1. Note that is also replaces the A register with the result of the increment operation.
		ZJF		DONE ; done? (CTR=0)
		JFI		01 ; if not, return to operation
			OPN
DONE	HLT ; if yes, stop program