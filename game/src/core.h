#ifndef CORE_H
#define CORE_H
#include "raylib.h"
#include <stdio.h>

enum ElementTypes {
    Enemies,
    Weapons,
    Pickups,    
};


static struct BaseEntity {
    Vector3 position;                
    Texture texture;
};

// Each of these will contain their own unique elements soon
typedef struct {
    struct BaseEntity;
    float drawHeight;
} MapBlock;

typedef struct {
    struct BaseEntity;
    enum ElementTypes type;
    float size;
} Element;

#endif
