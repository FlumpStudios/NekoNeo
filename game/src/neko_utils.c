
#include "neko_utils.h"
#include "constants.h"
#include <math.h>


uint8_t GetTetureIndex(uint8_t i)
{
    uint8_t r = 0;
    switch (i)
    {
    case 1 | DOOR_MASK:
        r = 0;
        break;
    case 2 | DOOR_MASK:
        r = 1;
        break;
    case 3 | DOOR_MASK:
        r = 2;
        break;
    case 4 | DOOR_MASK:
        r = 3;
        break;
    case 5 | DOOR_MASK:
        r = 4;
        break;
    case 6 | DOOR_MASK:
        r = 5;
        break;
    case 7 | DOOR_MASK:
        r = 6;
        break;
    case 8:
    case 8 + 7:
    case 8 + 14:
    case 8 + 21:
    case 8 + 28:
    case 8 + 35:
    case 8 + 42:
    case 8 + 49:
        r = 0;
        break;
    case 9:
    case 9 + 7:
    case 9 + 14:
    case 9 + 21:
    case 9 + 28:
    case 9 + 35:
    case 9 + 42:
    case 9 + 49:
        r = 1;
        break;
    case 10:
    case 10 + 7:
    case 10 + 14:
    case 10 + 21:
    case 10 + 28:
    case 10 + 35:
    case 10 + 42:
    case 10 + 49:
        r = 2;
        break;
    case 11:
    case 11 + 7:
    case 11 + 14:
    case 11 + 21:
    case 11 + 28:
    case 11 + 35:
    case 11 + 42:
    case 11 + 49:
        r = 3;
        break;
    case 12:
    case 12 + 7:
    case 12 + 14:
    case 12 + 21:
    case 12 + 28:
    case 12 + 35:
    case 12 + 42:
    case 12 + 49:
        r = 4;
        break;
    case 13:
    case 13 + 7:
    case 13 + 14:
    case 13 + 21:
    case 13 + 28:
    case 13 + 35:
    case 13 + 42:
    case 13 + 49:
        r = 5;
        break;
    case 14:
    case 14 + 7:
    case 14 + 14:
    case 14 + 21:
    case 14 + 28:
    case 14 + 35:
    case 14 + 42:
    case 14 + 49:
        r = 6;
        break;
    }

    return r;
}

void GetEntityPositionFromPosition(Vector3 location, uint8_t* col, uint8_t* row)
{
    *col = (int)(floor(location.x)) + (MAP_DIMENSION / 2);
    *row = (int)(floor(location.z)) + (MAP_DIMENSION / 2);
}

int GetMapArrayIndex(uint8_t col, uint8_t row)
{
    int index = (row * 64) + col;
    return index;
}

int GetMapIndeFromPosition(Vector3 location)
{
    int col = (int)(floor(location.x)) + (MAP_DIMENSION / 2);
    int row = (int)(floor(location.z)) + (MAP_DIMENSION / 2);

    int index = (row * 64) + col;
    return index;
}

uint8_t GetMapArrayHeightFromIndex(uint8_t index, uint8_t baseValue)
{
    // Empty space
    if (index == 0) { return 0; }
    if (index >= 15 && index < 22) { return 1; }
    if (index >= 22 && index < 29) { return 2; }
    if (index >= 29 && index < 36) { return 3; }
    if (index >= 36 && index < 43) { return 4; }
    if (index >= 43 && index < 50) { return 5; }
    if (index >= 50 && index < 57) { return 6; }
    if (index >= 57 && index < 64) { return 7; }
        
    return baseValue;
}

Vector3 NormalizeVector(Vector3 v) {
    float magnitude = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

    Vector3 normalized;
    normalized.x = v.x / magnitude;
    normalized.y = v.y / magnitude;
    normalized.z = v.z / magnitude;

    return normalized;
}

Vector3 CalculateCameraRayDirection(Camera* cam) {
    Vector3 direction;
    direction.x = cam->target.x - cam->position.x;
    direction.y = cam->target.y - cam->position.y;
    direction.z = cam->target.z - cam->position.z;
    return direction;
}

uint8_t GetNextElementType(uint8_t currentItem)
{
    if (currentItem < 0x0f)
    {
        return ++currentItem;
    }

    if (currentItem == 0x0f)
    {
        return 0x13;
    }

    if (currentItem == 0x13)
    {
        return 0x20;
    }
    if (currentItem >= 0x20 && currentItem < 0x26)
    {
        return ++currentItem;
    }
    return 0x01;
}

uint8_t GetPreviousElementType(uint8_t currentItem)
{

    if (currentItem == 0x0f)
    {
        return 0x13;
    }

    if (currentItem == 0x13)
    {
        return 0x20;
    }

    if (currentItem >= 0x20 && currentItem < 0x26)
    {
        return --currentItem;
    }

    if (currentItem > 0x01)
    {
        return --currentItem;
    }

    return 0x26;
}