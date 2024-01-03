
	#ifndef NEKOUTILS_H
	#define NEKOUTILS_H
	#include <stdint.h>
	
	int GetMapArrayIndex(uint8_t col, uint8_t row);
	uint8_t GetTetureIndex(uint8_t i);
	uint8_t GetMapArrayDrawHeightFromIndex(uint8_t index, uint8_t ceilHeight);
	#endif

