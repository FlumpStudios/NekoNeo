#ifndef CORE_H
#define CORE_H
#include "raylib.h"
#include <stdio.h>
#include <stdint.h>

enum ElementTypes {
    Wall,
    ENemy,
    Weapon,
    Pickup,  
};

enum Mode {
  Mode_Editor,
  Mode_Game,
  Mode_Scene
};

enum EditorRenderMode {
    RenerMode_Textured,
    RenerMode_Colored,
    RenerMode_CollisionBlock
};

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
} MapBlock;

typedef struct {
    struct BaseEntity;
    enum ElementTypes type;
    float size;
} Element;


typedef struct {
    Camera* camera;
    int arrayCell;
    bool wallCollision;
    uint8_t floorHeight;
} DebugInfo;

typedef struct {
    Vector3 position;
    int mapArrayIndex;
    int blockIndex;
    bool hasSelection;
    bool hasBlock;
} SelectedEntity;

#endif
