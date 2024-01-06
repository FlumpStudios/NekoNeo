#ifndef CONSTANTS_H
#define CONSTANTS_H


#define TRANS_RED        CLITERAL(Color){ 230, 41, 55, 100 } 

#define PROJECT_NAME "NekoNeo"
#define MAP_DIMENSION 64
#define PLAYER_HEIGHT 0.65f
#define DOOR_MASK 0xc0
#define BLOCK_HEIGHT 0.25f
#define DEBUG_LEVEL 99
#define WALL_TEXTURE_COUNT 15
#define ITEM_COUNT 13
#define WEAPON_COUNT 5
#define MAP_ARRAY_SIZE 4096
#define MAX_ELEMENTS 128
#define WORKING_DIR ""
#define MAX_CEIL_HEIGHT 31
#define GUN_SCALE 4.0f

// The higest a step can be before a player can step on it
#define PLAYER_STAIR_HEIGHT 0.3f

// The highest a step can be other than the wall
#define MAX_STEP_HEIGHT 7
#endif