
	#ifndef NEKOUTILS_H
	#define NEKOUTILS_H
	#include <stdint.h>
	#include "raylib.h"
	
	void GetEntityPositionFromPosition(Vector3 location, uint8_t* col, uint8_t* row);
	int GetMapArrayIndex(uint8_t col, uint8_t row);
	uint8_t GetTetureIndex(uint8_t i);
	uint8_t GetMapArrayHeightFromIndex(uint8_t index, uint8_t baseValue, uint8_t stepSize);
	int GetMapIndeFromPosition(Vector3 location);
	Vector3 NormalizeVector(Vector3 v);
	Vector3 CalculateCameraRayDirection(Camera* cam);
	uint8_t GetNextElementType(uint8_t currentItem);
	uint8_t GetPreviousElementType(uint8_t currentItem);
	bool IsFullWall(uint8_t index);
	#endif

