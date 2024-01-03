#include <assert.h>
#include "raylib.h"
#include "screens.h"
#include "level.h"
#include <stdint.h>
#include <stdio.h>
#include "rl_utils.h"
#include "core.h"
#include "neko_utils.h"
#include "constants.h"
#include <string.h>

static uint32_t _blockCount;
static uint16_t _elementCounts;

static bool drawHelpText = 0;
static int framesCounter = 0;
static int finishScreen = 0;
static int cameraMode = CAMERA_FREE;
static uint8_t currentLevel = DEBUG_LEVEL;
static SFG_Level* level;
static Camera camera = { 0 };

Texture2D wallTextures[WALL_TEXTURE_COUNT];
Texture2D itemTextures[ITEM_COUNT];
Texture2D weaponsTextures[WEAPON_COUNT];

Texture2D spiderEnemy;
Texture2D destroyerEnemy;
Texture2D warriorEnemy;
Texture2D plasmaBotEnemy;
Texture2D enderEnemy;
Texture2D turretEnemy;
Texture2D exploderEnemy;
Texture2D blocker;
Shader alphaDiscard;

Element* items;
MapBlock* mapBlocks;

Texture GetTextureFromElementType(uint8_t i)
{
    assert(i > 0);

    if (i <= ITEM_COUNT)
    {
        return itemTextures[i - 1];
    }

    else
    {
        switch (i)
        {
        case 0x0d:
        case 0x0e:
        case 0x0f:
            return itemTextures[12];

        case 0x20:
            return spiderEnemy;
        case 0x21:
            return destroyerEnemy;
        case 0x22:
            return warriorEnemy;
        case 0x23:
            return plasmaBotEnemy;
        case 0x24:
            return enderEnemy;
        case 0x25:
            return turretEnemy;
        case 0x26:
            return exploderEnemy;
        case 0x13:
            return blocker;
        }
    }
    return itemTextures[0];
}

void InitElements(void)
{
    size_t size = sizeof(Element) * (MAP_ARRAY_SIZE * MAX_CEIL_HEIGHT);
    if (!items)
    {
        items = MemAlloc(size);
        if (!items)
        {
            TraceLog(LOG_ERROR, "Memory allocation failed on items!");
            return;
        }
    }
    memset(items, 0, size);

    for (size_t i = 0; i < MAX_ELEMENTS; i++)
    {
        if (level->elements[i].type > 0)
        {
            float size = 1.f;

            int mapArrayindex = GetMapArrayIndex(level->elements[i].coords[0], level->elements[i].coords[1]);

            uint8_t stepSize = GetMapArrayDrawHeightFromIndex(level->mapArray[mapArrayindex], level->ceilHeight);

            if (stepSize > MAX_STEP_HEIGHT)
            {
                stepSize = 0;
            }

            Texture t = GetTextureFromElementType(level->elements[i].type);
            float h = 0.25f * stepSize;

            items[i].position = (Vector3){ level->elements[i].coords[1] + 0.5, (size / 2) + h, level->elements[i].coords[0] + 0.5 };
            items[i].texture = t;
            items[i].size = size;
            _elementCounts++;
        }
    }
}

void InitWalls(void)
{
    size_t size = sizeof(MapBlock) * (MAP_ARRAY_SIZE * MAX_CEIL_HEIGHT);
    if (!mapBlocks)
    {
        mapBlocks = MemAlloc(size);
        if (!mapBlocks)
        {
            TraceLog(LOG_ERROR, "Memory allocation failed on mapblocks!");
            return;
        }
    }
    memset(mapBlocks, 0, size);

    uint8_t col = 0;
    uint8_t row = 0;

    for (size_t i = 0; i < MAP_ARRAY_SIZE; i++)
    {
        if (i > 0 && i % 64 == 0)
        {
            row++;
            col = 0;
        }

        if (level->mapArray[i] > 0)
        {
            uint8_t v = level->mapArray[i];
            uint8_t height = GetMapArrayDrawHeightFromIndex(level->mapArray[i], level->ceilHeight);
            uint8_t textureIndexRef = GetTetureIndex(level->mapArray[i]);
            uint8_t textureIndex = level->textureIndices[textureIndexRef];

            for (uint8_t k = 0; k < height; k++)
            {
                uint8_t skipCount = 0;
                float drawHeight = BLOCK_HEIGHT;

                for (uint8_t j = 4; j > 1; j--)
                {
                    if ((height - k) >= j)
                    {
                        drawHeight = BLOCK_HEIGHT * j;
                        skipCount = j - 1;
                        break;
                    }
                }

                float x = 1.0f * row;
                float y = 1.0f;
                float z = 1.0f * col;

                bool isDoor = v >= DOOR_MASK;

                if (isDoor)
                {
                    if (k < 4)
                    {
                        mapBlocks[_blockCount].position = (Vector3){ x + 0.5f, (k * BLOCK_HEIGHT) + drawHeight / 2, z + 0.5f };
                        mapBlocks[_blockCount].texture = wallTextures[level->doorTextureIndex];
                        
                    }
                    else
                    {
                        mapBlocks[_blockCount].position = (Vector3){ x + 0.5f, (k * BLOCK_HEIGHT) + drawHeight / 2, z + 0.5f };
                        mapBlocks[_blockCount].texture = wallTextures[textureIndex];
                        
                    }
                }
                else
                {
                    mapBlocks[_blockCount].position = (Vector3){ x + 0.5f, (k * BLOCK_HEIGHT) + drawHeight / 2, z + 0.5f };
                    mapBlocks[_blockCount].texture = wallTextures[textureIndex];
                    
                }

                mapBlocks[_blockCount].drawHeight = drawHeight;
                _blockCount++;
                k += skipCount;
            }
        }
        col++;
    }
}

void InitGameplayScreen(void)
{

#ifdef DEBUG
    alphaDiscard = LoadShader(NULL, "C:/Projects/NekoNeo/game/src/alphaDiscard.fs");
    
    level = MemAlloc(sizeof(SFG_Level));

    if (level == NULL) {
        TraceLog(LOG_ERROR, "Memory allocation failed on level!");
    }
    else {
        memset(level, 0, sizeof(SFG_Level));

        if (!SFG_loadLevelFromFile(level, currentLevel))
        {
            TraceLog(LOG_ERROR, "Error Loading level from file");
        }
        else
        {
            TraceLog(LOG_INFO, "Level loaded OK");
        }
    }

    

    for (uint8_t i = 0; i < WALL_TEXTURE_COUNT; i++)
    {
        char fullPath[256];
        sprintf(fullPath, "C:\\Projects\\NekoEngine\\WallTextures\\o_%u.png", i);
        wallTextures[i] = LoadTexture(fullPath);
    }

    for (uint8_t i = 0; i < ITEM_COUNT; i++)
    {
        char fullPath[256];
        sprintf(fullPath, "C:\\Projects\\NekoEngine\\Items\\o_%u.png", i);
        itemTextures[i] = LoadTexture(fullPath);
    }

    for (uint8_t i = 0; i < WEAPON_COUNT; i++)
    {
        char fullPath[256];
        sprintf(fullPath, "C:\\Projects\\NekoEngine\\Weapons\\o_%u.png", i);
        weaponsTextures[i] = LoadTexture(fullPath);
    }

    spiderEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_0.png");
    destroyerEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_3.png");
    warriorEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_6.png");
    plasmaBotEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_8.png");
    enderEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_10.png");
    turretEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_13.png");
    exploderEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_16.png");
    blocker = LoadTexture("C:/Projects/NekoEngine/Items/blocker.png");

#else
    alphaDiscard = LoadShader(NULL, "data/alphaDiscard.fs");

    if (!SFG_loadLevelFromFile(&level, currentLevel))
    {
        TraceLog(LOG_ERROR, "Error Loading level from file");
    }
    else
    {
        TraceLog(LOG_INFO, "Level loaded OK");
}

    for (uint8_t i = 0; i < WALL_TEXTURE_COUNT; i++)
    {
        char fullPath[256];
        sprintf(fullPath, "WallTextures/o_%u.png", i);
        wallTextures[i] = LoadTexture(fullPath);
    }

    for (uint8_t i = 0; i < ITEM_COUNT; i++)
    {
        char fullPath[256];
        sprintf(fullPath, "/Items/o_%u.png", i);
        itemTextures[i] = LoadTexture(fullPath);
    }

    for (uint8_t i = 0; i < WEAPON_COUNT; i++)
    {
        char fullPath[256];
        sprintf(fullPath, "Weapons/o_%u.png", i);
        weaponsTextures[i] = LoadTexture(fullPath);
    }

    spiderEnemy = LoadTexture("Enemies/o_0.png");
    destroyerEnemy = LoadTexture("Enemies/o_3.png");
    warriorEnemy = LoadTexture("Enemies/o_6.png");
    plasmaBotEnemy = LoadTexture("Enemies/o_8.png");
    enderEnemy = LoadTexture("Enemies/o_10.png");
    turretEnemy = LoadTexture("Enemies/o_13.png");
    exploderEnemy = LoadTexture("Enemies/o_16.png");
    blocker = LoadTexture("Items/blocker.png");

#endif

    framesCounter = 0;
    finishScreen = 0;

    if (level)
    {
        camera.position = (Vector3){ level->playerStart[1] , 1.0f, level->playerStart[0] };
    }

    InitWalls();
    InitElements();

    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          
    camera.fovy = 60.0f;                                
    camera.projection = CAMERA_PERSPECTIVE;    
    DisableCursor();
}




// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_H))
    {
        drawHelpText = !drawHelpText;
    }

    if (IsKeyPressed(KEY_ONE))
    {
        cameraMode = CAMERA_FREE;
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; 
    }

    if (IsKeyPressed(KEY_TWO))
    {
        cameraMode = CAMERA_FIRST_PERSON;
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    }

    if (IsKeyPressed(KEY_THREE))
    {
        cameraMode = CAMERA_THIRD_PERSON;
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; 
    }

    if (IsKeyPressed(KEY_FOUR))
    {
        cameraMode = CAMERA_ORBITAL;
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; 
    }

    if (IsKeyPressed(KEY_P))
    {
        if (camera.projection == CAMERA_PERSPECTIVE)
        {
            // Create isometric view
            cameraMode = CAMERA_THIRD_PERSON;
            // Note: The target distance is related to the render distance in the orthographic projection
            camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
            camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
            camera.projection = CAMERA_ORTHOGRAPHIC;
            camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
            CameraYaw(&camera, -135 * DEG2RAD, true);
            CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
        }
        else if (camera.projection == CAMERA_ORTHOGRAPHIC)
        {
            // Reset to default view
            cameraMode = CAMERA_THIRD_PERSON;
            camera.position = (Vector3){ 0.0f, 2.0f, 15.0f };
            camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
            camera.projection = CAMERA_PERSPECTIVE;
            camera.fovy = 60.0f;
        }   
    }

    if (IsKeyDown(KEY_E))
    {
        camera.position.y += 0.1f;
    }
    else if (IsKeyDown(KEY_Q))
    {
        camera.position.y -= 0.1f;
    }
    else
    {
        UpdateCamera(&camera, cameraMode);
    }
}



// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    ClearBackground(SKYBLUE);

    BeginMode3D(camera);

    DrawPlane((Vector3) { 0.0f, -0.1f, 0.0f }, (Vector2) { 256.0f, 256.0f }, GRAY);

    DrawGrid(4096, 1.0f);
    DrawWalls();

    BeginShaderMode(alphaDiscard);
    DrawElements();
    EndShaderMode(alphaDiscard);

    if (DRAW_PLAYER)
    {
        DrawPlayer();
    }

    EndMode3D();

    // UI
    DrawCrossHair();
    if (drawHelpText)
    {
        DrawDebugData();
    }
}

void DrawCrossHair(void)
{
    DrawText("+", GetScreenWidth() / 2, GetScreenHeight() / 2, 24, WHITE);
}

void DrawPlayer(void)
{
    if (cameraMode == CAMERA_THIRD_PERSON)
    {
        DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
        DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
    }
}

void DrawDebugData(void)
{
    // Draw info boxes
    DrawRectangle(5, 5, 330, 120, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 120, BLUE);

    DrawText("Camera controls:", 15, 15, 10, BLACK);
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
    DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
    DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
    DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
    DrawText("- Camera projection key: P", 15, 90, 10, BLACK);
    DrawText("- SHow/Hide help and debug info: H", 15, 105, 10, BLACK);

    DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(600, 5, 195, 100, BLUE);

    DrawText("Camera status:", 610, 15, 10, BLACK);
    DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
        (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
        (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
        (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 610, 30, 10, BLACK);
    DrawText(TextFormat("- Projection: %s", (camera.projection == CAMERA_PERSPECTIVE) ? "PERSPECTIVE" :
        (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC" : "CUSTOM"), 610, 45, 10, BLACK);
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);
}

void DrawElements(void)
{
    for (size_t i = 0; i < _elementCounts; i++)
    {
        DrawBillboard(camera, items[i].texture, items[i].position, items[i].size, WHITE);
    }
}

void DrawWalls(void)
{   
    if (mapBlocks)
    {
        for (size_t i = 0; i < _blockCount; i++)
        {
            if (mapBlocks[i].drawHeight > 0)
            {            
                UTL_DrawCubeTexture(mapBlocks[i].texture, mapBlocks[i].position, 1.0f, mapBlocks[i].drawHeight, 1.0f, WHITE);
            }
        }
    }
}



// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    MemFree(level);
    level = NULL;
    _blockCount = 0;
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}