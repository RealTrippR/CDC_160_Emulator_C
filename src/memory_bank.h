



#ifndef CDC_160_MEMORY_BANK_H
#define CDC_160_MEMORY_BANK_H

#include <stdint.h>
#include "word_12.h"

struct MemBank {
	uint16_t size;
	Word12 data[4096];
};

#endif // !CDC_160_MEMORY_BANK_H