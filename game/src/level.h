
#ifndef _SFG_LEVELS_H
#define _SFG_LEVELS_H
#define SFG_MAP_SIZE 64
#define SFG_TILE_DICTIONARY_SIZE 64
#include "constants.h"
#include "neko_utils.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>


/**
  Defines a single game map tile. The format is following:

    MSB aaabbbbb cccddddd LSB

   aaa:   ceiling texture index (from texture available on the map), 111
          means completely transparent texture
   bbbbb: ceiling height (1111 meaning no ceiling) ABOVE the floor
   ccc:   floor texture index, 111 means completely transparent texture
   ddddd: floor height
*/
typedef uint16_t SFG_TileDefinition;

#define SFG_TILE_CEILING_MAX_HEIGHT 31
#define SFG_TILE_TEXTURE_TRANSPARENT 7

typedef SFG_TileDefinition SFG_TileDictionary[SFG_TILE_DICTIONARY_SIZE];

/// helper macros for SFG_TileDefinition
#define SFG_TD(floorH, ceilH, floorT, ceilT)\
  ((floorH & 0x001f) |\
   ((floorT & 0x0007) << 5) |\
   ((ceilH & 0x001f) << 8) |\
   ((ceilT & 0x0007) << 13))

#define SFG_TILE_FLOOR_HEIGHT(tile) (tile & 0x1f)
#define SFG_TILE_FLOOR_TEXTURE(tile) ((tile & 0xe0) >> 5)
#define SFG_TILE_CEILING_HEIGHT(tile) ((tile & 0x1f00) >> 8)
#define SFG_TILE_CEILING_TEXTURE(tile) ((tile & 0xe000) >> 13)

#define SFG_OUTSIDE_TILE SFG_TD(63,0,7,7)

/**
  Game map represented as a 2D array. Array item has this format:

    MSB aabbbbbb LSB

    aa:     type of square, possible values:
      00:   normal
      01:   moving floor (elevator), moves from floor height to ceililing height
            (there is no ceiling above)
      10:   moving ceiling, moves from ceiling height to floor height
      11:   door
    bbbbbb: index into tile dictionary
*/
typedef uint8_t SFG_MapArray[SFG_MAP_SIZE * SFG_MAP_SIZE];

#define SFG_TILE_PROPERTY_MASK 0xc0
#define SFG_TILE_PROPERTY_NORMAL 0x00
#define SFG_TILE_PROPERTY_ELEVATOR 0x40
#define SFG_TILE_PROPERTY_SQUEEZER 0x80
#define SFG_TILE_PROPERTY_DOOR 0xc0

/**
  Serves to place elements (items, enemies etc.) into the game level.
*/
typedef struct
{
    uint8_t type;
    uint8_t coords[2];
} SFG_LevelElement;

#define SFG_MAX_LEVEL_ELEMENTS 128

/*
  Definitions of level element type. These values must leave the highest bit
  unused because that will be used by the game engine, so the values must be
  lower than 128.
*/
#define SFG_LEVEL_ELEMENT_NONE 0
#define SFG_LEVEL_ELEMENT_BARREL 0x01
#define SFG_LEVEL_ELEMENT_HEALTH 0x02
#define SFG_LEVEL_ELEMENT_BULLETS 0x03
#define SFG_LEVEL_ELEMENT_ROCKETS 0x04
#define SFG_LEVEL_ELEMENT_PLASMA 0x05
#define SFG_LEVEL_ELEMENT_TREE 0x06
#define SFG_LEVEL_ELEMENT_FINISH 0x07
#define SFG_LEVEL_ELEMENT_TELEPORTER 0x08
#define SFG_LEVEL_ELEMENT_TERMINAL 0x09
#define SFG_LEVEL_ELEMENT_COLUMN 0x0a
#define SFG_LEVEL_ELEMENT_RUIN 0x0b
#define SFG_LEVEL_ELEMENT_LAMP 0x0c
#define SFG_LEVEL_ELEMENT_CARD0 0x0d ///< Access card, unlocks doors with lock.
#define SFG_LEVEL_ELEMENT_CARD1 0x0e
#define SFG_LEVEL_ELEMENT_CARD2 0x0f
#define SFG_LEVEL_ELEMENT_LOCK0 0x10 /**< Special level element that must be
                                     placed on a tile with door. This door is
                                     then locked until taking the corresponding
                                     access card. */
#define SFG_LEVEL_ELEMENT_LOCK1 0x11
#define SFG_LEVEL_ELEMENT_LOCK2 0x12
#define SFG_LEVEL_ELEMENT_BLOCKER 0x13 ///< invisible wall

#define SFG_LEVEL_ELEMENT_MONSTER_SPIDER 0x20
#define SFG_LEVEL_ELEMENT_MONSTER_DESTROYER 0x21
#define SFG_LEVEL_ELEMENT_MONSTER_WARRIOR 0x22
#define SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT 0x23
#define SFG_LEVEL_ELEMENT_MONSTER_ENDER 0x24
#define SFG_LEVEL_ELEMENT_MONSTER_TURRET 0x25
#define SFG_LEVEL_ELEMENT_MONSTER_EXPLODER 0x26

#define SFG_MONSTERS_TOTAL 7

#define SFG_MONSTER_TYPE_TO_INDEX(monsterType) \
  ((monsterType) - SFG_LEVEL_ELEMENT_MONSTER_SPIDER)

#define SFG_MONSTER_INDEX_TO_TYPE(monsterIndex) \
  ((monsterIndex) + SFG_LEVEL_ELEMENT_MONSTER_SPIDER)

#define SFG_LEVEL_ELEMENT_TYPE_IS_MOSTER(t) \
  ((t) >= SFG_LEVEL_ELEMENT_MONSTER_SPIDER)

typedef struct
{
    SFG_MapArray mapArray;
    SFG_TileDictionary tileDictionary;
    uint8_t textureIndices[TEXTURE_INDICES_COUNT]; /**< Says which textures are used on the map. There
                               can be at most 7 because of 3bit indexing (one
                               value is reserved for special transparent
                               texture). */
    uint8_t doorTextureIndex; /**< Index (global, NOT from textureIndices) of a
                               texture used for door. */
    uint8_t floorColor;
    uint8_t ceilingColor;
    uint8_t playerStart[3];   /**< Player starting location: square X, square Y,
                                   direction (fourths of RCL_Unit). */
    uint8_t backgroundImage;  ///< Index of level background image.
    SFG_LevelElement elements[SFG_MAX_LEVEL_ELEMENTS];
    uint8_t ceilHeight;
    uint8_t floorHeight;
} SFG_Level;

#define SFG_NUMBER_OF_LEVELS 10



static void InvertElements(SFG_LevelElement* inputArray, size_t arrayLength)
{
    for (size_t i = 0; i < arrayLength; i++)
    {
        inputArray[i].coords[0] = MAP_DIMENSION - 1 - inputArray[i].coords[0];
    }
}

void InvertArrayWidth(uint8_t* inputArray, size_t arrayLength)
{
    uint8_t* tempArray = (uint8_t*)MemAlloc(arrayLength);

    for (size_t i = 0; i < arrayLength; i++) {
        size_t x = i % MAP_DIMENSION;
        size_t y = i / MAP_DIMENSION;

        size_t adjustedX = MAP_DIMENSION - 1 - x;

        size_t newIndex = adjustedX + y * MAP_DIMENSION;


        tempArray[newIndex] = inputArray[i];
    }

    memcpy(inputArray, tempArray, arrayLength);

    MemFree(tempArray);
}
uint8_t SFG_loadLevelFromFile(SFG_Level* buffer, uint8_t level)
{
    if (buffer == NULL) {
        return 0;
    }

    char levelString[512];

#ifdef DEBUG
    
    snprintf(levelString, sizeof(levelString), "C:\\Projects\\NekoEngine\\levels\\level%02u.HAD", level);
#else
    snprintf(levelString, sizeof(levelString), "levels/level%02u.HAD", level);
#endif
    
    FILE* file = fopen(levelString, "rb");
    if (file == NULL) {
         perror("Error opening file");
        return 0;
    }

    if (fread(buffer, sizeof(SFG_Level), 1, file) != 1) {
        fclose(file);
        // Handle the error
        return 0;
    }

    fclose(file);

    InvertElements(&buffer->elements, MAX_ELEMENTS);
    InvertArrayWidth(&buffer->mapArray, MAP_DIMENSION * MAP_DIMENSION);
    buffer->playerStart[0] = MAP_DIMENSION - 1 - buffer->playerStart[0];
    return 1;
}


bool SaveLevel(SFG_Level *level)
{
#ifdef DEBUG
    const char* saveLocation ="C:/Projects/NekoEngine/levels/level99.HAD";
#else
    const char* saveLocation = "levels/level99.HAD";
#endif

    SFG_Level* tempLevel;
    tempLevel = MemAlloc(sizeof(SFG_Level));
    
    if (!tempLevel)
    {
        return false;
    }

    memcpy(tempLevel, level, sizeof(SFG_Level));
    
    InvertArrayWidth(&tempLevel->mapArray, MAP_DIMENSION * MAP_DIMENSION);
    InvertElements(&tempLevel->elements, MAX_ELEMENTS);
    tempLevel->playerStart[0] = MAP_DIMENSION - 1 - tempLevel->playerStart[0];
    
    FILE* file = fopen(saveLocation, "wb");

    if (file != NULL) {
        fwrite(tempLevel, sizeof(SFG_Level), 1, file);
        fclose(file);        
        MemFree(tempLevel);
        return true;
    }
    else {
        MemFree(tempLevel);
        return false;
    }
}
#endif // guard

