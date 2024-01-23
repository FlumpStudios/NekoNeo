#ifndef CONSTANTS_H
#define CONSTANTS_H

#define INITIAL_HISTORY_BUFFER_SIZE 1000
#define SELECTION_BOX_SIZE 1.005f
#define DEGRESS_TO_BYTE_CONVERSION 1.4117
#define OUTSIDE_CEIL_VALUE 31
#define MAX_WALL_HEIGHT 30
#define MIN_WALL_HEIGHT 4

#define DEFAULT_FOV  60.0f
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define TRANS_RED        CLITERAL(Color){ 230, 41, 55, 100 } 
#define TRANS_BLUE       CLITERAL(Color){ 135, 206, 235, 100 }
#define TEXTURE_INDICES_COUNT 7
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