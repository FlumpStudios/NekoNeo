#ifndef CONSTANTS_H
#define CONSTANTS_H

#define EMPTY ""
#define MAX_LEVEL_PACK_NAME 50
#define MAX_SAVE_FILE_NAME 50
#define DOOR_TEXTURE_INDEX 15
#define MAX_BACKGROUND_VALUE 3
#define MAX_FLOOR_AND_CEIL_COLOUR 100
#define MAX_STEP_SIZE 10
#define BASE_LEVEL_NAME "base"
#define TILE_DICTIONARY_SIZE 64
#define PLAYER_SCALE 0.5f
#define PLAYER_SIZE { PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE }
#define M_PI 3.14159
#define LEVEL_STRING_MAX_LENGTH 100
#define MAX_INPUT_CHARS 500
#define CONSOLE_HISTORY_SIZE 5
#define CONSOLE_FONT_SIZE 16
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
#define BLOCK_WIDTH 1.0f
#define DEBUG_LEVEL "level99"
#define WALL_TEXTURE_COUNT 16
#define ITEM_COUNT 13
#define WEAPON_COUNT 5
#define MAP_ARRAY_SIZE 4096
#define MAX_ELEMENTS 128
#define WORKING_DIR ""
#define MAX_CEIL_HEIGHT 31
#define GUN_SCALE 4.0f

// The higest a step can be before a player can step on it
#define PLAYER_STAIR_HEIGHT 0.3f

#ifdef DEBUG
#define saveLocation "C:/Projects/NekoEngine/levels/"
#else
#define saveLocation "levels/"
#endif

// The highest a step can be other than the wall
#define MAX_STEP_HEIGHT 7
#endif