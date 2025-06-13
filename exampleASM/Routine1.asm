START	LCN		12 ; place (-10) in A
		STF		CT ; place (-10) in counter operation
OPN		AOF		CT ; operation, count + 1
		NZB		OPN ; count = 0? no, return to operation
		HLT ; yes, stop
		CT ; Counter location
