
	#ifndef NEKOUTILS_H
	#define NEKOUTILS_H
	#include <stdint.h>
	#include "raylib.h"

	int GetMapArrayIndex(uint8_t col, uint8_t row);
	uint8_t GetTetureIndex(uint8_t i);
	uint8_t GetMapArrayHeightFromIndex(uint8_t index, uint8_t baseValue);
	int GetMapIndeFromPosition(float x, float y);
	Vector3 NormalizeVector(Vector3 v);
	Vector3 CalculateCameraRayDirection(Camera* cam);
	#endif

