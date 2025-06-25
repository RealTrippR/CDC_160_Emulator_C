;this example is from page 43 of the CDC 160 Compute Programming Manual, circa 1960
;you can find it here: https://ia802905.us.archive.org/12/items/bitsavers_cdc160023aingManual1960_4826291/023a_160_Computer_Programming_Manual_1960_text.pdf
START	LCN		12 ; place (-10) in A
		STF		CT ; store (-10) in counter location
OPN		AOF		CT ; operation, count + 1
		NZB		OPN ; count = 0? no, return to operation
		HLT ; yes, stop
		CT ; counter location