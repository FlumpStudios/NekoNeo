#include <assert.h>
#include "raylib.h"
#include "engine.h"
#include "level.h"
#include <stdint.h>
#include <stdio.h>
#include "rl_utils.h"
#include "core.h"
#include "neko_utils.h"
#include "constants.h"
#include "rcamera.h"
#include "editorUi.h"
#include <string.h>


bool _isFullScreen = false;
static bool _2D_Mode = false;
static bool _focusedMode = false;
static uint32_t _blockCount;
static uint16_t _elementCount;
static bool _isPlayerClipping = false;
static Vector3 _lastNonClippedPosition;
static uint8_t _floorHeight;
static bool drawHelpText = 1;
static int framesCounter = 0;
static int finishScreen = 0;
static int _currentWallSelection = 8;
static int cameraMode = CAMERA_FREE;
static uint8_t currentLevel = DEBUG_LEVEL;
static SFG_Level* level;
static Camera3D camera = { 0 };

enum Mode currentEditorMode = Mode_Editor;
enum EditorRenderMode currentRenderMode = RenerMode_Textured;

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

SelectedEntity selectionLocation = { 0 };

void InitWalls(bool saveOnComplete);
void InitElements(bool saveOnComplete);

void RefreshWalls(void)
{
    free(mapBlocks);
    mapBlocks = NULL;
    _blockCount = 0;
    InitWalls(true);
}

void RefreshElements(void)
{
    free(items);
    items = NULL;
    _elementCount = 0;
    InitElements(true);
}


void SetSelectionBlockLocation(void)
{   
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {   
        selectionLocation.itemIndex = -1;
        Ray ray = { 0 };
        RayCollision collision = { 0 };
        
        
        if (_focusedMode)
        {
            ray = GetMouseRay(GetMousePosition(), camera);
        }
        else
        {        
            Vector3 pos = { camera.position.x, camera.position.y, camera.position.z };
            ray.position = pos;
            ray.direction = CalculateCameraRayDirection(&camera);        
        }


        enum Entity_Type_Item foundEntityType = Entity_Type_None;
        float dist = -1;
        
        int nearestSelection = -1;

        for (int i = 0; i < _elementCount; i++)
        {
            collision = GetRayCollisionBox(ray, items[i].boundingBox);
            if (collision.hit)
            {
                if (collision.distance < dist || dist < 0)
                {
                    dist = collision.distance;
                    nearestSelection = i;
                    foundEntityType = Entity_Type_Item;
                }
            }
        }

        if (nearestSelection < 0)
        {
            selectionLocation.hasSelection = false;
        }
        else
        {
            selectionLocation.hasSelection = true;
            selectionLocation.position = items[nearestSelection].position;
            selectionLocation.mapArrayIndex = GetMapIndeFromPosition(selectionLocation.position);
            selectionLocation.itemIndex = nearestSelection;
            selectionLocation.entityType = Entity_Type_Item;    
        }

        for (int i = 0; i < _blockCount; i++)
        {
            collision = GetRayCollisionBox(ray, mapBlocks[i].boundingBox);
            if (collision.hit)
            {
                if (collision.distance < dist || dist < 0)
                {   dist = collision.distance;
                    nearestSelection = i;
                    foundEntityType = Entity_Type_Wall;
                }
            }
        }

        
        if(nearestSelection < 0)
        { 
            selectionLocation.hasSelection = false;
            selectionLocation.entityType = Entity_Type_None;
            selectionLocation.position = (Vector3){ 0.f,0.f,0.f };
            selectionLocation.mapArrayIndex = 0;
        }
        else
        {
            if (foundEntityType != Entity_Type_Wall)
            {
                return;
            }

            bool hasBlock = mapBlocks[nearestSelection].hasBlock;
            selectionLocation.position = mapBlocks[nearestSelection].position;
            selectionLocation.hasSelection = true;
            selectionLocation.mapArrayIndex = GetMapIndeFromPosition(selectionLocation.position);
            if (!hasBlock)
            {
                if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
                {
                    selectionLocation.entityType = Entity_Type_Item;
                    uint8_t col = 0;
                    uint8_t row = 0;
                    GetEntityPositionFromPosition(selectionLocation.position, &col, &row);
                    level->elements[_elementCount].coords[0] = col;
                    level->elements[_elementCount].coords[1] = row;
                    level->elements[_elementCount].type = 1;
                    RefreshElements();
                    selectionLocation.hasSelection = true;
                    selectionLocation.itemIndex = _elementCount - 1; 

                }
                else
                {                
                    selectionLocation.entityType = Entity_Type_Wall;
                    level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
                    RefreshWalls();
                }
            }
            else 
            {
                selectionLocation.entityType = Entity_Type_Wall;
                _currentWallSelection = level->mapArray[selectionLocation.mapArrayIndex];
            }
        }

    }
}

bool CheckLevelCollision(Vector3 entityPos, Vector3 entitySize)
{   
    BoundingBox entityBox = (BoundingBox){(Vector3){ entityPos.x - entitySize.x/2,
                                                     entityPos.y - entitySize.y/2,
                                                     entityPos.z - entitySize.z/2 },
                                          (Vector3){ entityPos.x + entitySize.x/2,
                                                     entityPos.y + entitySize.y/2,
                                                     entityPos.z + entitySize.z/2 }};

    for (int i=0; i < _blockCount; i++)
    {        
        if (CheckCollisionBoxes(entityBox, mapBlocks[i].boundingBox))
        {
            auto h = mapBlocks[i].position.y + (mapBlocks[i].drawHeight / 2);
            if (camera.position.y > (h  + PLAYER_STAIR_HEIGHT))
            {
                return false;
            };
         
            return true;
        } 
    }
    return false;
}

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

        case 0x13:
            return blocker;
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
        }
    }
    return itemTextures[0];
}

void InitElements(bool saveOnComplete)
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

            uint8_t stepSize = GetMapArrayHeightFromIndex(level->mapArray[mapArrayindex], level->ceilHeight);

            if (stepSize > MAX_STEP_HEIGHT)
            {
                stepSize = 0;
            }

            Texture t = GetTextureFromElementType(level->elements[i].type);
            float h = 0.25f * stepSize;
            float x = level->elements[i].coords[0] + 0.5;
            
            float y = (size / 2) + h;
            float z = level->elements[i].coords[1] + 0.5;
            Vector3 pos = (Vector3){ x - MAP_DIMENSION / 2,y ,z - MAP_DIMENSION / 2 };
            
            Vector3 elementSize = (Vector3){ 1.0f, 1.0f, 1.0f };

            BoundingBox elementbox = (BoundingBox){ (Vector3) {
                        pos.x - elementSize.x / 2,
                        pos.y - elementSize.y / 2,
                        pos.z - elementSize.z / 2
                    },
                    (Vector3) {
                        pos.x + elementSize.x / 2,
                        pos.y + elementSize.y / 2,
                        pos.z + elementSize.z / 2
                    } };

            items[i].position = pos;
            items[i].boundingBox = elementbox;
            items[i].texture = t;
            items[i].size = size;
            _elementCount++;
        }
        else
        {
            break;
        }
    }

    if (saveOnComplete && SaveLevel(level))
    {
        TraceLog(LOG_INFO, "Level saved on wall update");
    }
    else
    {
        TraceLog(LOG_ERROR, "Error saving level to file");
    }
}

void InitWalls(bool saveOnComplete)
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
        if (i > 0 && i % MAP_DIMENSION == 0)
        {
            row++;
            col = 0;
        }

        if (level->mapArray[i] > 0)
        {
            uint8_t v = level->mapArray[i];
            uint8_t height = GetMapArrayHeightFromIndex(level->mapArray[i], level->ceilHeight);
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

                float x = (1.0f * col) - MAP_DIMENSION / 2;
                float y = 1.0f;
                float z = (1.0f * row) - MAP_DIMENSION / 2;

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
                mapBlocks[_blockCount].textureIndex = textureIndex;
                Vector3 wallPos = mapBlocks[_blockCount].position;
                Vector3 wallSize = { 1.0f,mapBlocks[_blockCount].drawHeight,1.0f };

                BoundingBox levelBox = (BoundingBox){ (Vector3) {
                        wallPos.x - wallSize.x / 2,
                        wallPos.y - wallSize.y / 2,
                        wallPos.z - wallSize.z / 2
                    },
                    (Vector3) {
                        wallPos.x + wallSize.x / 2,
                        wallPos.y + wallSize.y / 2,
                        wallPos.z + wallSize.z / 2
                    } };

                
                mapBlocks[_blockCount].boundingBox = levelBox;
                mapBlocks[_blockCount].hasBlock = true;

                _blockCount++;
                k += skipCount;
            }
        }
        else
        {
            // TODO: Eww, need to remove duplicated stuff
            float x = (1.0f * col) - MAP_DIMENSION / 2;
            float y = 1.0f;
            float z = (1.0f * row) - MAP_DIMENSION / 2;
            mapBlocks[_blockCount].position = (Vector3){ x + 0.5f, BLOCK_HEIGHT / 2, z + 0.5f };
            mapBlocks[_blockCount].hasBlock = false;
            mapBlocks[_blockCount].drawHeight = 0.25f;

            Vector3 wallPos = mapBlocks[_blockCount].position;
            Vector3 wallSize = { 1.0f,mapBlocks[_blockCount].drawHeight,1.0f };

            BoundingBox levelBox = (BoundingBox){ (Vector3) {
                        wallPos.x - wallSize.x / 2,
                        wallPos.y - wallSize.y / 2,
                        wallPos.z - wallSize.z / 2
                    },
                    (Vector3) {
                        wallPos.x + wallSize.x / 2,
                        wallPos.y + wallSize.y / 2,
                        wallPos.z + wallSize.z / 2
                    } };

            mapBlocks[_blockCount].boundingBox = levelBox;

            _blockCount++;        
        }
        col++;
    }

    if (saveOnComplete && SaveLevel(level))
    {
        TraceLog(LOG_INFO, "Level saved on wall update");
    }
    else
    {
        TraceLog(LOG_ERROR, "Error saving level to file");
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
    blocker = LoadTexture("C:/Projects/NekoEngine/assets/Blocker.png");

#else
    alphaDiscard = LoadShader(NULL, "data/alphaDiscard.fs");

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
        sprintf(fullPath, "WallTextures/o_%u.png", i);
        wallTextures[i] = LoadTexture(fullPath);
    }

    for (uint8_t i = 0; i < ITEM_COUNT; i++)
    {
        char fullPath[256];
        sprintf(fullPath, "Items/o_%u.png", i);
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
    blocker = LoadTexture("assets/Blocker.png");
#endif

    framesCounter = 0;
    finishScreen = 0;

    if (level)
    {
        camera.position = (Vector3){ level->playerStart[0] - (MAP_DIMENSION / 2) , 1.0f, level->playerStart[1] - (MAP_DIMENSION / 2) };
    }

    InitWalls(false);
    InitElements(false);

    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          
    camera.fovy = DEFAULT_FOV;
    camera.projection = CAMERA_PERSPECTIVE;
    currentEditorMode = Mode_Editor;
    DisableCursor();
}

void UpdateFloorHeight()
{
    auto e = level->mapArray[selectionLocation.mapArrayIndex];
    uint8_t h = GetMapArrayHeightFromIndex(e, e == 0 ? 1 : level->ceilHeight);
    _floorHeight = h;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) && currentEditorMode == Mode_Editor)
    {        
        _focusedMode = !_focusedMode;
        _focusedMode ? EnableCursor() : DisableCursor();
    }


    if (IsKeyPressed(KEY_TAB))
    {
        static bool was_orignally_focusedmode;        
        static Vector3 orignal_cam_pos;
        static Vector3 orignal_cam_target;
        static bool was_cursor_hidden;
  

        _2D_Mode = !_2D_Mode;
        if (_2D_Mode)
        {
            was_orignally_focusedmode = _focusedMode;
            orignal_cam_pos = camera.position;
            orignal_cam_target = camera.target;
            was_cursor_hidden = IsCursorHidden();
            
            ShowCursor();
            camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
            camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
            camera.projection = CAMERA_ORTHOGRAPHIC;
            CameraPitch(&camera, -90 * DEG2RAD, true, true, false);
            CameraYaw(&camera, 180 * DEG2RAD, false);
            CameraRoll(&camera, 0);
            _focusedMode = true;
            camera.fovy = 65.f;
        }
        else
        {
            _focusedMode = was_orignally_focusedmode;
            camera.position = orignal_cam_pos;
            camera.target = orignal_cam_target;
            camera.projection = CAMERA_PERSPECTIVE;            
            camera.fovy = 65.f;
            was_cursor_hidden ? HideCursor() : ShowCursor();
        }
    }

    if (IsKeyPressed(KEY_F11))
    {
        ToggleFullscreen();
    }

    if (IsKeyPressed(KEY_G))
    {
        currentEditorMode = (currentEditorMode == Mode_Editor ? Mode_Game : Mode_Editor);
    }

    if (IsKeyPressed(KEY_H))
    {
        if(currentEditorMode == Mode_Editor)
        { 
            drawHelpText = !drawHelpText;
        }
    }

    if (IsKeyPressed(KEY_PERIOD))
    {   
        // HACK: insanely inefficient way to update the blocks, basically regenerating every block. At the moment just want something functiona.
        // TODO: Do this properly

        if (selectionLocation.hasSelection)
        {   
            if (selectionLocation.entityType == Entity_Type_Wall) 
            {
                _currentWallSelection += 7;
                if (_currentWallSelection >= 64)
                {
                    _currentWallSelection -= 63;
                    _currentWallSelection += 7;
                }
                level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
                RefreshWalls();
            }
        }
    }

    if (IsKeyPressed(KEY_COMMA))
    {
        // HACK: insanely inefficient way to update the blocks, basically regenerating every block. At the moment just want something functiona.
        // TODO: Do this properly

        if (selectionLocation.hasSelection)
        {
            if (selectionLocation.entityType == Entity_Type_Wall)
            {
                _currentWallSelection -= 7;
                if (_currentWallSelection <= 7)
                {
                    _currentWallSelection += 63;
                    _currentWallSelection -= 7;
                }
                level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
                RefreshWalls();
            }
        }
    }

    if (IsKeyPressed(KEY_DELETE))
    {
        if (selectionLocation.hasSelection)
        {            
            if (selectionLocation.entityType == Entity_Type_Wall)
            {
                level->mapArray[selectionLocation.mapArrayIndex] = 0;
                RefreshWalls();
                selectionLocation.hasSelection = false;
            }
            else if (selectionLocation.entityType == Entity_Type_Item)
            {
                uint16_t i = selectionLocation.itemIndex;                
                while (i < MAX_ELEMENTS -1 && level->elements[i].type > 0)
                {
                    uint16_t next = i + 1;
                    level->elements[i].coords[0] = level->elements[next].coords[0];
                    level->elements[i].coords[1] = level->elements[next].coords[1];
                    level->elements[i].type = level->elements[next].type;
                    i++;
                };
                RefreshElements();
                selectionLocation.hasSelection = false;
            }
        }
    }

    if (IsKeyPressed(KEY_RIGHT_BRACKET))
    {   
        if (selectionLocation.hasSelection)
        {
            if (selectionLocation.entityType == Entity_Type_Wall)
            {            
                _currentWallSelection++;
                if ((_currentWallSelection - 1) % 7 == 0)
                {
                    _currentWallSelection -= 7;
                }
                level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
                RefreshWalls();
            }
            else if (selectionLocation.entityType == Entity_Type_Item)
            {
                uint8_t t = level->elements[selectionLocation.itemIndex].type;
                uint8_t nextElement = GetNextElementType(t);
                level->elements[selectionLocation.itemIndex].type = nextElement;
                RefreshElements();
            }
        }
    }

    if (IsKeyPressed(KEY_LEFT_BRACKET))
    {
        if (selectionLocation.hasSelection)
        {
            if (selectionLocation.entityType == Entity_Type_Wall)
            {
                _currentWallSelection--;
                if ((_currentWallSelection) % 7 == 0)
                {
                    _currentWallSelection += 7;
                }
                level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
                RefreshWalls();
            }
            else if (selectionLocation.entityType == Entity_Type_Item)
            {            
                uint8_t t = level->elements[selectionLocation.itemIndex].type;
                uint8_t nextElement = GetPreviousElementType(t);
                level->elements[selectionLocation.itemIndex].type = nextElement;
                RefreshElements();
            }
        }
    }

    if (IsKeyPressed(KEY_R))
    {
        if (currentEditorMode == Mode_Editor)
        {
            currentRenderMode++;
            if (currentRenderMode >= 3)
            {
                currentRenderMode = 0;
            }
        }
        else
        {
            currentRenderMode = 0;
        }
    }

    if (currentEditorMode == Mode_Editor)
    {
        SetSelectionBlockLocation();
        cameraMode = CAMERA_FREE;
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; 
    }
    else if (currentEditorMode == Mode_Game)
    {
        cameraMode = CAMERA_FIRST_PERSON;
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };

        int arrayPos = GetMapIndeFromPosition(camera.position);
        auto e = level->mapArray[arrayPos];
        uint8_t h = GetMapArrayHeightFromIndex(e, 0);
        
        camera.position.y = PLAYER_HEIGHT + (BLOCK_HEIGHT * (h));

        if (_isPlayerClipping)
        {
           camera.position = _lastNonClippedPosition;
        }
        else
        {
           _lastNonClippedPosition = camera.position;
        }

    }
    else if (currentEditorMode == Mode_Scene)
    {
        cameraMode = CAMERA_ORBITAL;
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    }


    // Override the default key behavior for E and Q, default is rotate
    if (IsKeyDown(KEY_E))
    {
        if (currentEditorMode == Mode_Editor)
        {        
            camera.position.y += 0.1f;
        }
    }
    else if (IsKeyDown(KEY_Q))
    {
        if (currentEditorMode == Mode_Editor)
        {
            camera.position.y -= 0.1f;
        }
    }
    else
    {
        if (!_focusedMode)
        {        
            UpdateCamera(&camera, cameraMode);
        }
    
    }
    _isPlayerClipping = CheckLevelCollision(camera.position, (Vector3) { 0.5f, 0.5f, 0.5f });
    UpdateFloorHeight();
}


// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    ClearBackground(SKYBLUE);

    BeginMode3D(camera);
    DrawPlane((Vector3) { 0.0f, -0.1f, 0.0f }, (Vector2) { 256.0f, 256.0f }, GRAY);
    DrawWalls();

    if (currentEditorMode == Mode_Editor)
    {
        if (selectionLocation.hasSelection)
        {
            int mod = _floorHeight % 4 > 0;
            for (size_t i = 0; i < ((_floorHeight / 4) + mod); i++)
            {   
                Vector3 vec = { selectionLocation.position.x,(float)i + 0.5f, selectionLocation.position.z };                
                if (selectionLocation.entityType == Entity_Type_Wall)
                {                
                    DrawCube(vec, 1.005f, 1.005f, 1.005f, TRANS_RED);
                    DrawCubeWires(vec, 1.005, 1.005f, 1.005f, BLACK);
                }
                else if (selectionLocation.entityType == Entity_Type_Item)
                {
                    DrawCubeWires(vec, 1.005, 1.005f, 1.005f, RED);
                }                
            }
        }

        DrawGrid(MAP_DIMENSION, 1.0f);
    }
    
    BeginShaderMode(alphaDiscard);
    DrawElements();
    EndShaderMode(alphaDiscard);   
    EndMode3D();

    if (currentEditorMode == Mode_Game)
    {
        float x = (GetScreenWidth() / 2.0f) - ((weaponsTextures[1].width * GUN_SCALE) / 2);
        float y = GetScreenHeight() - (weaponsTextures[1].height * GUN_SCALE);
        DrawTextureEx(weaponsTextures[1], (Vector2) { x, y}, 0.0f, GUN_SCALE, WHITE);
    }
    else if (drawHelpText && currentEditorMode == Mode_Editor)
    {
        DebugInfo d = { &camera,selectionLocation.mapArrayIndex, _isPlayerClipping, _floorHeight };
        EUI_DrawDebugData(&d);
    }

    if (!_focusedMode)
    {    
        DrawCrossHair();
    }
}

void DrawCrossHair(void)
{
    DrawText("+", (GetScreenWidth() / 2 ) -  10, GetScreenHeight() / 2, 24, WHITE);
}

void DrawElements(void)
{
    for (size_t i = 0; i < _elementCount; i++)
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
            if (mapBlocks[i].hasBlock &&  mapBlocks[i].drawHeight > 0)
            {            
                if (currentRenderMode == RenerMode_Textured)
                {                   
                    UTL_DrawCubeTexture(mapBlocks[i].texture, mapBlocks[i].position, 1.0f, mapBlocks[i].drawHeight, 1.0f, WHITE);
                }
                else if (currentRenderMode == RenerMode_Colored)
                {
                    DrawCube(mapBlocks[i].position, 1.0f,mapBlocks[i].drawHeight, 1.0f, BLUE);
                    DrawCubeWires(mapBlocks[i].position, 1.0f, mapBlocks[i].drawHeight, 1.0f, WHITE);
                }
                else if (currentRenderMode == RenerMode_CollisionBlock)
                {   
                    DrawBoundingBox(mapBlocks[i].boundingBox, GREEN);
                }
            }
        }
    }
}

// Gameplay Screen Unload logic
void UnloadEngineScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    MemFree(level);
    level = NULL;
    _blockCount = 0;
}

