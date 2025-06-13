; Performs operation one hundred times (operation takes 700 location)


LIM		01		44
START	LCB		LIM
		STD		77
OPN						; operation (left blank)

		AOD		77 ; counter + 1
		ZJF		DONE ; done? (CTR=0)
		JFI		01 ; if not, return to operation
			OPN
DONE	STP ; if yes, stop program