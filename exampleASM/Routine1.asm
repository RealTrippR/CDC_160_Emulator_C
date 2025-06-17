		00		12
START	LCB		01 ; place (-10) in A
		STF		CT ; store (-10) in counter location
OPN		AOF		CT ; operation, count + 1
		NZB		OPN ; count = 0? no, return to operation
		HLT ; yes, stop
		CT ; counter location