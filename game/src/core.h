#ifndef CORE_H
#define CORE_H
#include "raylib.h"
#include <stdio.h>
#include <stdint.h>
#include "constants.h"  

enum EntityType {
    Entity_Type_None,
    Entity_Type_Wall,
    Entity_Type_Item,
    Entity_Type_Free
};

typedef enum Mode {
  Mode_Editor,
  Mode_Game,
  Mode_Scene,
  Mode_Console
} Mode;

typedef enum UiMode {
    Off,
    DebugOnly,
    DebugAndHelp
} UiMode;

typedef enum EditorRenderMode {
    RenerMode_Textured,
    RenerMode_Colored,
    RenerMode_CollisionBlock
} EditorRenderMode;

typedef struct {
    bool isColliding;
    Vector3 wallPositions;
} levelCollisionData;

struct BaseEntity {
    Vector3 position;                
    Texture texture;
};

// Each of these will contain their own unique elements soon
typedef struct {
    struct BaseEntity;
    float drawHeight;
    BoundingBox boundingBox;
    uint8_t textureIndex;
    bool hasBlock;
    bool isDoor;
} MapBlock;

typedef struct {
    struct BaseEntity;
    enum ElementTypes type;
    BoundingBox boundingBox;
    float size;
} Element;

typedef struct {
    Camera* camera;
    int arrayCell;
    uint8_t floorHeight;
    bool isOutside;
    int fps;
    uint8_t stepSize;
    char* levelPack;

} DebugInfo;

typedef struct {
    Vector3 position;
    int itemIndex;
    int mapArrayIndex;
    enum EntityType entityType;
} SelectedEntity;

#endif
