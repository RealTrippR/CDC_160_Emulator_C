#ifndef CDC_160_MAGNETIC_TAPE_UNIT_H
#define CDC_160_MAGNETIC_TAPE_UNIT_H

#include <stdbool.h>
#include "word_12.h"

enum CDC607_LOGICAL_UNIT_INDEX {
	CDC607_UNIT_INDEX_1,
	CDC607_UNIT_INDEX_2,
	CDC607_UNIT_INDEX_3,
	CDC607_UNIT_INDEX_4,
	CDC607_UNIT_INDEX_5,
	CDC607_UNIT_INDEX_6,
	CDC607_UNIT_INDEX_7,
	CDC607_UNIT_INDEX_STANDBY
};

struct CDC607TapeDrive {
	
	Word12 inputPort;
	Word12 outputPort;

	enum CDC607_LOGICAL_UNIT_INDEX unitIndex;
	bool on;
	bool fault;
	bool forwards;
	bool clear;
	bool rev;
	bool fwd;

	CDC607_LOGICAL_UNIT_INDEX mode;
	//CDC607_Tape_Reel* reel;
};

#endif // !CDC_160_MAGNETIC_TAPE_UNIT_H