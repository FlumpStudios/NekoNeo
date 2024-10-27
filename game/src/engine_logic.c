#include <assert.h>
#include "raylib.h"
#include "engine.h"
#include "level.h"
#include <stdint.h>
#include <stdio.h>
#include "rl_utils.h"
#include <stdint.h>
#include <stdlib.h>
#include "core.h"
#include "neko_utils.h"
#include "constants.h"
#include "rcamera.h"
#include "editorUi.h"
#include <string.h>
#include "raymath.h"

static Mode PreviousMode = Mode_Editor;
static bool _levelReady = false;
static bool _2D_Mode = false;
static bool _focusedMode = false;
static uint32_t _blockCount;
static uint16_t _elementCount;
static uint8_t _floorHeight;
static UiMode drawHelpText = 1;
static int finishScreen = 0;
static int _currentWallSelection = 8;
static int _currentItemSelection = 1;
static int cameraMode = CAMERA_FREE;
static SFG_Level* level;
static Camera3D camera = { 0 };

static LevelHistory _levelHistory;
static size_t _historySize = INITIAL_HISTORY_BUFFER_SIZE;

Mode currentEditorMode = Mode_Editor;
EditorRenderMode currentRenderMode = RenerMode_Textured;

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
Texture2D lock;
Model playerMarker;
Shader alphaDiscard;
Element* items;
MapBlock* mapBlocks;
SelectedEntity selectionLocation = { 0 };

typedef struct {
    char Text[MAX_INPUT_CHARS];
    char ResponseMessage[MAX_INPUT_CHARS];
} ConsoleHistory;

//CONSOLE STUFF
char name[MAX_INPUT_CHARS + 1] = "\0";
int letterCount = 0;
ConsoleHistory _consoleHistory[CONSOLE_HISTORY_SIZE];


// Forward declarations
void InitWalls(bool saveOnComplete);
void InitElements(bool saveOnComplete);
void DrawPlayerStartPosition(void);
void RefreshMap(bool updateHistory);
void UnloadEngineScreen(void);

void GetFullPath(char* buffer)
{
    if (strcmp(levelPack, EMPTY))
    {
        
    }
}

void ConsoleQuery(const char* query, char* responseBuffer, size_t size)
{
    const char* inputString = TextToUpper(query);
    if (strncmp(inputString, "LOAD", 4) == 0)
    {
        const char* spacePos = strstr(inputString, " ");

        if (spacePos != NULL) {
            char* path = spacePos + 1;
            if (strcmp(levelPack, EMPTY) != 0)
            {
                char tempPath[MAX_LEVEL_PACK_NAME + MAX_SAVE_FILE_NAME];
                strcpy(tempPath, path);
                sprintf(path, "%s/%s", levelPack, tempPath);
            }
        
            if (!SFG_loadLevelFromFile(level, path))
            {
                sprintf(responseBuffer, "Error loading level '%s'", path);
            }
            else
            {       
                sprintf(responseBuffer, "level '%s' loaded successfully", path);
                RefreshMap(true);
            }
        }        
    }
    else if (strncmp(inputString, "SAVE", 4) == 0)
    {   
        char* spacePos = strstr(inputString, " ");
        
        if (spacePos == 0)
        {
            strcpy(responseBuffer, "Error: Invalid file name");
        }
        else if (strnlen(spacePos, MAX_INPUT_CHARS) > MAX_SAVE_FILE_NAME)
        {
            sprintf(responseBuffer, "Error: Filename exceeded max character limit %i", MAX_SAVE_FILE_NAME);
        }
        else
        {
            if (spacePos != NULL) {
                char* path = spacePos + 1;
                if (strcmp(levelPack, EMPTY) != 0)
                {
                    char tempPath[MAX_LEVEL_PACK_NAME + MAX_SAVE_FILE_NAME];
                    strcpy(tempPath, path);
                    sprintf(path, "%s/%s", levelPack, tempPath);
                }
            
                if (SaveLevel(level, path))
                {
                    sprintf(responseBuffer, "level '%s' saved successfully", path);
                }
                else
                {
                    sprintf(responseBuffer, "Error saving level '%s'", path);
                }
            }
        }
    }
    else if (strncmp(inputString, "QUIT", 4) == 0)
    {
        // TODO: Properly release everything        
        UnloadEngineScreen();
    }
    // TODO: do a decent job of adding help
    /*else if (strncmp(inputString, "HELP", 4) == 0)
    {
        strcpy(responseBuffer, "\"Close\" - Close console; \"load\" + level name - load level; \"save\" + level name - save level; \"test\" - Test level; setLevelPack + level pack \n \"nuke\" - Clear level; \"quit\" - quit editor; \"setStepSize\" + int param - set the global step size");
    }*/
    else if (strncmp(inputString, "NUKE", 4) == 0)
    {
        memset(level, 0, sizeof(SFG_Level));
        SFG_loadLevelFromFile(level, BASE_LEVEL_NAME);
        initLevel(level);
        RefreshMap(true);
        strcpy(responseBuffer, "Map Nuked!!! Exit console and Ctrl + Z if you're feeling regret");
    }
    else if (strncmp(inputString, "CLOSE", 5) == 0)
    {
        currentEditorMode = PreviousMode;
        strcpy(responseBuffer, "Console closed");
    }
    else if (strncmp(inputString, "CLS", 3) == 0)
    {
        memset(_consoleHistory, 0, sizeof(ConsoleHistory) * CONSOLE_HISTORY_SIZE);
        memset(name, 0, MAX_INPUT_CHARS);
    }
    else if (strncmp(inputString, "CLEARLEVELPACK", 14) == 0)
    {
        memset(levelPack, NULL, MAX_LEVEL_PACK_NAME);
        strcpy(responseBuffer, "Level pack cleared");
    }
    else if (strncmp(inputString, "SETLEVELPACK", 12) == 0)
    {
        char buffer[MAX_INPUT_CHARS];
        memset(buffer, NULL, MAX_INPUT_CHARS);
        strcpy(buffer, inputString + 13);
        if (strnlen(buffer, MAX_LEVEL_PACK_NAME) >= MAX_LEVEL_PACK_NAME)
        {
            strcpy(responseBuffer, "Pack name too long");
        }
        else if (strnlen(buffer, MAX_LEVEL_PACK_NAME) < 1)
        {
            strcpy(responseBuffer, "Please enter a pack name");
        }
        else
        {
            strcpy(levelPack, inputString + 13);
            sprintf(responseBuffer, "Level pack updated to %s", levelPack);
        }
    }
    else if (strncmp(inputString, "LEVELPACK", 9) == 0)
    {
        if (strcmp(levelPack, EMPTY) != 0)
        {
            sprintf(responseBuffer, "%s", levelPack);
        }
        else
        {
            strcpy(responseBuffer, "No level pack has been set");
        }
    }
    else if (strncmp(inputString, "STEPSIZE", 8) == 0)
    {
        if (level)
        {
            sprintf(responseBuffer,"%i", level->stepSize);
        }
        else
        {
            strcpy(responseBuffer, "Something went wrong: could not find level data");           
        }
    }
    else if (strncmp(inputString, "SETSTEPSIZE", 11) == 0)
    {        
        if (strnlen(inputString, 100) < 13)
        {            
            strcpy(responseBuffer, "Please enter a value for step size");
        }
        else
        {
            char* endptr = NULL;

            auto size = strtol(&inputString[12], &endptr, 13);

            if (*endptr != '\0') {
                strcpy(responseBuffer, "Could not read step size value");
            }
            else if (size > MAX_STEP_SIZE)
            {
                char tooBigbuffer[40];
                const char* maxSizeResponse = sprintf(tooBigbuffer, "Step size must be equel to or below %i", MAX_STEP_SIZE);
                strcpy(responseBuffer, tooBigbuffer);
            }
            else if (size < 1)
            {
                char tooSmallbuffer[40];
                const char* maxSizeResponse = sprintf(tooSmallbuffer, "Step size cannont be below 1");
                strcpy(responseBuffer, tooSmallbuffer);
            }
            else {
                level->stepSize = size;
                RefreshMap(true);
                strcpy(responseBuffer, "Step size updated");
            }
            
        }        
    }
    else if (strncmp(inputString, "TEST", 4) == 0)
    {    

#ifdef  DEBUG
        strcpy(responseBuffer, "Map testing current disabled in debug");
        return;
#endif

        const char* exeLocation = "Ruyn.exe -w -d";        
        system(exeLocation);
    }
    else
    {
        if (strnlen(inputString, MAX_INPUT_CHARS) > 0)
        {
            sprintf(responseBuffer, "'%s' is not recognsied as a command", query);
        }
        else
        {
            sprintf(responseBuffer, "Please enter a command");
        }
    }
}

void HandleConsoleInput(void)
{       
    int key = GetCharPressed();

    while (key > 0)
    {
        // NOTE: Only allow keys in range [32..125]
        if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
        {
            name[letterCount] = (char)key;
            name[letterCount + 1] = '\0';
            letterCount++;
        }

        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        letterCount--;
        if (letterCount < 0) letterCount = 0;
        name[letterCount] = '\0';
    }

    else if (IsKeyPressed(KEY_ENTER))
    {
        for (size_t i = CONSOLE_HISTORY_SIZE - 1; i > 0; i--)
        {
            if (i > 0)
            {
                _consoleHistory[i] = _consoleHistory[i - 1];
            }
        }

        ConsoleQuery(name, &_consoleHistory[0].ResponseMessage, MAX_INPUT_CHARS);
        strcpy(&_consoleHistory[0].Text, &name);

        memset(&name, 0, sizeof(char) * MAX_INPUT_CHARS);
        letterCount = 0;
        name[letterCount] = '\0';
    };
}

void RenderConsole(void)
{    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int inputY = screenHeight - (CONSOLE_FONT_SIZE * 2);
    int responseY = screenHeight - (CONSOLE_FONT_SIZE);

    Rectangle textBox = {0,  screenHeight / 1.5f, screenWidth, screenHeight / 3 };
    DrawRectangleRec(textBox, TRANS_RED);
    DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, WHITE);    
    DrawText(name, (int)textBox.x + 5, inputY, CONSOLE_FONT_SIZE, WHITE);

    for (size_t i = 0; i < CONSOLE_HISTORY_SIZE; i++)
    {
        auto y = (inputY - (CONSOLE_FONT_SIZE * 2) - (i * CONSOLE_FONT_SIZE * 2 + 5));

        DrawText(_consoleHistory[i].Text, (int)textBox.x + 5, y, CONSOLE_FONT_SIZE, LIGHTGRAY);
        char buffer[MAX_INPUT_CHARS + 2];
        sprintf(buffer, "\n%s", _consoleHistory[i].ResponseMessage);
        DrawText(buffer, (int)textBox.x + 5, y, CONSOLE_FONT_SIZE, YELLOW);
    }
        
    if (letterCount < MAX_INPUT_CHARS)
    {
        static int framesCounter = 0;
        framesCounter++;
        if (framesCounter > 40)
        {
            framesCounter = 0;
        }
        if (((framesCounter / 20) % 2) == 0) DrawText("_", (int)textBox.x + 8 + MeasureText(name, CONSOLE_FONT_SIZE), inputY, CONSOLE_FONT_SIZE, WHITE);
    }
}

void UpdateHistory(void)
{
    if (_levelHistory.currentIndex >= _historySize - 2)
    {
        size_t originalSize = sizeof(SFG_Level) * _historySize;

        SFG_Level* buffer = MemAlloc(originalSize);
        assert(buffer);
        memcpy(buffer, _levelHistory.history, originalSize);
        MemFree(_levelHistory.history);

        _historySize *= 2;
        size_t newSize = sizeof(SFG_Level) * _historySize;

        _levelHistory.history = MemAlloc(newSize);
        assert(_levelHistory.history);

        memcpy(_levelHistory.history, buffer, newSize);
        MemFree(buffer);
    }

    _levelHistory.currentIndex++;
    _levelHistory.history[_levelHistory.currentIndex] = *level;
    memset(&_levelHistory.history[_levelHistory.currentIndex + 1], 0, sizeof(SFG_Level));
}

void RefreshMap(bool updateHistory)
{
    free(mapBlocks);
    mapBlocks = NULL;
    _blockCount = 0;
    InitWalls(true);

    free(items);
    items = NULL;
    _elementCount = 0;
    InitElements(true);
    
    if (updateHistory)
    {    
        UpdateHistory();
    }    
}

void CheckPlayerBlockCollision(void)
{
    for (int i = 0; i < _blockCount; i++)
    {
        if (!mapBlocks[i].hasBlock) { continue; }
        float h = mapBlocks[i].position.y + (mapBlocks[i].drawHeight / 2);
        if (camera.position.y > (h + PLAYER_STAIR_HEIGHT))
        {
            continue;
        };

        if (camera.position.x - (PLAYER_SCALE / 2) < mapBlocks[i].position.x + (BLOCK_WIDTH / 2) &&
            camera.position.x + (PLAYER_SCALE / 2) > mapBlocks[i].position.x - (BLOCK_WIDTH / 2) &&
            camera.position.z - (PLAYER_SCALE / 2) < mapBlocks[i].position.z + (BLOCK_WIDTH / 2) &&
            camera.position.z + (PLAYER_SCALE / 2) > mapBlocks[i].position.z - (BLOCK_WIDTH / 2))
        {
            float dx = fabs(camera.position.x - mapBlocks[i].position.x);
            float dy = fabs(camera.position.z - mapBlocks[i].position.z);

            float overlapX = (PLAYER_SCALE + BLOCK_WIDTH) / 2.0f - dx;
            float overlapY = (PLAYER_SCALE + BLOCK_WIDTH) / 2.0f - dy;

            if (overlapX < overlapY)
            {
                if (camera.position.x < mapBlocks[i].position.x)
                    camera.position.x -= overlapX;
                else
                    camera.position.x += overlapX;
            }

            else
            {
                if (camera.position.z < mapBlocks[i].position.z)
                    camera.position.z -= overlapY;
                else
                    camera.position.z += overlapY;
            }
        }
    }
}

void SetSelectionBlockLocation(void)
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

        // CHECK FOR ITEMS
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

        // Check for walls
        for (int i = 0; i < _blockCount; i++)
        {
            collision = GetRayCollisionBox(ray, mapBlocks[i].boundingBox);
            if (collision.hit)
            {
                if (collision.distance < dist || dist < 0)
                {   dist = collision.distance;
                    nearestSelection = i;
                    bool hasBlock = mapBlocks[i].hasBlock;
                    foundEntityType = hasBlock ? Entity_Type_Wall : Entity_Type_Free;
                }
            }
        }

        if (foundEntityType == Entity_Type_None)
        {
            // DO NOTHING AT THE MOMENT
        }
        else if (foundEntityType == Entity_Type_Wall || foundEntityType == Entity_Type_Free)
        {
            selectionLocation.entityType = foundEntityType;
            selectionLocation.itemIndex = -1;
            selectionLocation.position = mapBlocks[nearestSelection].position;
            selectionLocation.mapArrayIndex = GetMapIndeFromPosition(selectionLocation.position);
            if (foundEntityType == Entity_Type_Wall)
            {
                _currentWallSelection = level->mapArray[selectionLocation.mapArrayIndex];
            }
        }
        else if (foundEntityType == Entity_Type_Item)
        {
            selectionLocation.entityType = Entity_Type_Item;
            selectionLocation.itemIndex = nearestSelection;
            selectionLocation.position = items[nearestSelection].position;
            selectionLocation.mapArrayIndex = GetMapIndeFromPosition(selectionLocation.position);            
            _currentItemSelection = level->elements[nearestSelection].type;            
        }
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && foundEntityType != Entity_Type_Item)
        {
            uint8_t col = 0;
            uint8_t row = 0;

            GetEntityPositionFromPosition(selectionLocation.position, &col, &row);
            level->elements[_elementCount].coords[0] = col;
            level->elements[_elementCount].coords[1] = row;
            level->elements[_elementCount].type = _currentItemSelection;
            RefreshMap(true);
            selectionLocation.itemIndex = _elementCount - 1;
        }

        if (foundEntityType != Entity_Type_Wall && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
            RefreshMap(true);
        }
        

        if (_currentWallSelection > DOOR_MASK)
        {
            _currentWallSelection -= DOOR_MASK;
        }
    
}

void RemoveElement(uint16_t i)
{
    while (i < MAX_ELEMENTS - 1 && level->elements[i].type > 0)
    {
        uint16_t next = i + 1;
        level->elements[i].coords[0] = level->elements[next].coords[0];
        level->elements[i].coords[1] = level->elements[next].coords[1];
        level->elements[i].type = level->elements[next].type;
        i++;
    };
}

int DoesPositionHaveDoor(Vector3 location)
{
    uint8_t x = 0;
    uint8_t y = 0;

    GetEntityPositionFromPosition(location, &x, &y);
    
    for (size_t i = 0; i < _elementCount; i++)
    {
        if (level->elements[i].coords[0] == x && level->elements[i].coords[1] == y)
        {
            return i;
        }
    }
    return -1;
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
            case SFG_LEVEL_ELEMENT_LOCK0:
            case SFG_LEVEL_ELEMENT_LOCK1:
            case SFG_LEVEL_ELEMENT_LOCK2:
                return lock;
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

            uint8_t stepSize = GetMapArrayHeightFromIndex(level->mapArray[mapArrayindex], level->floorHeight, level->stepSize);

            Texture t = GetTextureFromElementType(level->elements[i].type);
            float h = 0.25f * stepSize;
            float x = level->elements[i].coords[0] + 0.5;
            
            float y = (size / 2) + h;
            float z = level->elements[i].coords[1] + 0.5;
            Vector3 pos = (Vector3){ x - MAP_DIMENSION / 2,y ,z - MAP_DIMENSION / 2 };
            
            Vector3 elementSize = (Vector3){ 0.95f, 0.95f, 0.95f };

            BoundingBox elementbox = {0};

            if (level->elements[i].type < SFG_LEVEL_ELEMENT_LOCK0 || level->elements[i].type > SFG_LEVEL_ELEMENT_LOCK2)
            {
                elementbox = (BoundingBox){ (Vector3) {
                            pos.x - elementSize.x / 2,
                            pos.y - elementSize.y / 2,
                            pos.z - elementSize.z / 2
                        },
                        (Vector3) {
                            pos.x + elementSize.x / 2,
                            pos.y + elementSize.y / 2,
                            pos.z + elementSize.z / 2
                        } 
                };                
            }


            items[i].type = level->elements[i].type;
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

    if (saveOnComplete && SaveLevel(level, DEBUG_LEVEL))
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
            uint8_t height = GetMapArrayHeightFromIndex(level->mapArray[i], level->floorHeight, level->stepSize);            
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

                mapBlocks[_blockCount].isDoor = isDoor;
                if (isDoor)
                {
                    if (k == level->doorLevitation * 4)
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

    if (saveOnComplete && SaveLevel(level, DEBUG_LEVEL))
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
    alphaDiscard = LoadShader(NULL, "C:/Projects/NekoNeo/game/src/shaders/alphaDiscard.fs");
    
    level = MemAlloc(sizeof(SFG_Level));

    if (level == NULL) 
{
        TraceLog(LOG_ERROR, "Memory allocation failed on level!");
    }
    else {
        
        memset(level,0,sizeof(SFG_Level));

        if (!SFG_loadLevelFromFile(level, DEBUG_LEVEL))
        {
            TraceLog(LOG_ERROR, "Error Loading debug level from file");

            if (!SFG_loadLevelFromFile(level, BASE_LEVEL_NAME))
            {
                TraceLog(LOG_ERROR, "Error Loading base level from file");
            }

            initLevel(level);
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
    plasmaBotEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_9.png");
    enderEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_11.png");
    turretEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_14.png");
    exploderEnemy = LoadTexture("C:/Projects/NekoEngine/Enemies/o_17.png");
    blocker = LoadTexture("C:/Projects/NekoEngine/assets/Blocker.png");
    lock = LoadTexture("C:/Projects/NekoEngine/assets/lock.png");
    playerMarker = LoadModel("C:/Projects/NekoEngine/assets/PlayerStartPosition.obj");

#else
    alphaDiscard = LoadShader(NULL, "data/alphaDiscard.fs");

    level = MemAlloc(sizeof(SFG_Level));

    if (level == NULL) {
        TraceLog(LOG_ERROR, "Memory allocation failed on level!");
    }
    else {
        memset(level, 0, sizeof(SFG_Level));

        if (!SFG_loadLevelFromFile(level, DEBUG_LEVEL))
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
    plasmaBotEnemy = LoadTexture("Enemies/o_9.png");
    enderEnemy = LoadTexture("Enemies/o_11.png");
    turretEnemy = LoadTexture("Enemies/o_14.png");
    exploderEnemy = LoadTexture("Enemies/o_17.png");
    blocker = LoadTexture("assets/Blocker.png");
    playerMarker = LoadModel("assets/PlayerStartPosition.obj");
    lock = LoadTexture("assets/lock.png");
#endif
    finishScreen = 0;

    _levelHistory.history = MemAlloc(sizeof(SFG_Level) * _historySize);
    assert(_levelHistory.history);
    memset(_levelHistory.history, 0, sizeof(SFG_Level)* _historySize);
    
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
    
    // Initial level history
    if (level)
    {       
        memcpy(&_levelHistory.history[0], level, sizeof(SFG_Level));
    }

    _levelReady = true;
}

void UpdateFloorHeight(void)
{
    auto e = level->mapArray[selectionLocation.mapArrayIndex];
    uint8_t h = GetMapArrayHeightFromIndex(e, e == 0 ? 1 : level->floorHeight, level->stepSize);
    _floorHeight = h;
}

void ScrollUpEntities(void)
{
    if (selectionLocation.entityType == Entity_Type_Wall)
    {
        bool isDoor = level->mapArray[selectionLocation.mapArrayIndex] > DOOR_MASK;

        _currentWallSelection--;
        if ((_currentWallSelection) % 7 == 0)
        {
            _currentWallSelection += 7;
        }

        if (isDoor)
        {
            level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection | DOOR_MASK;
        }
        else
        {
            level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
        }

        RefreshMap(true);
    }
    else if (selectionLocation.entityType == Entity_Type_Item)
    {
        uint8_t t = _currentItemSelection;
        uint8_t previousElement = GetPreviousElementType(t);
        level->elements[selectionLocation.itemIndex].type = previousElement;
        _currentItemSelection = previousElement;
        RefreshMap(true);
    }
}

void ScrollDownEntities(void)
{
    if (selectionLocation.entityType == Entity_Type_Wall)
    {
        bool isDoor = level->mapArray[selectionLocation.mapArrayIndex] > DOOR_MASK;

        _currentWallSelection++;
        if ((_currentWallSelection - 1) % 7 == 0)
        {
            _currentWallSelection -= 7;
        }

        if (isDoor)
        {
            level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection | DOOR_MASK;
        }
        else
        {
            level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
        }


        RefreshMap(true);
    }
    else if (selectionLocation.entityType == Entity_Type_Item)
    {
        uint8_t t = _currentItemSelection;
        uint8_t nextElement = GetNextElementType(t);
        level->elements[selectionLocation.itemIndex].type = nextElement;
        _currentItemSelection = nextElement;
        RefreshMap(true);
    }
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (currentEditorMode == Mode_Console)
    {
        HandleConsoleInput();
    }
    if (!_levelReady)
    {
        return;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) && currentEditorMode == Mode_Editor)
    {        
        _focusedMode = !_focusedMode;
        _focusedMode ? EnableCursor() : DisableCursor();
    }



    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (currentEditorMode == Mode_Console)
        {
            currentEditorMode = PreviousMode;
        }
    }

    if (IsKeyPressed(KEY_TAB))
    {                    
        if (currentEditorMode != Mode_Console)
        {
            PreviousMode = currentEditorMode;
            currentEditorMode = Mode_Console;
        }
    }

    if (currentEditorMode == Mode_Console) { return; }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Z))
    {
        if (_levelHistory.currentIndex > 0)
        {
            _levelHistory.currentIndex--;
            memcpy(level, &_levelHistory.history[_levelHistory.currentIndex], sizeof(SFG_Level));
            RefreshMap(false);

        }
    }
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Y))
    {
        SFG_Level a = _levelHistory.history[_levelHistory.currentIndex + 1];
        if (a.floorHeight > 0 && a.ceilHeight > 0)
        {
            _levelHistory.currentIndex++;
            memcpy(level, &_levelHistory.history[_levelHistory.currentIndex], sizeof(SFG_Level));
            RefreshMap(false);            
        }
    }


    static bool isPlayerRotatingRight = false;
    if (IsKeyDown(KEY_F6))
    {
        level->playerStart[2]++;
        if (level->playerStart[2] >= 254)
        {
            level->playerStart[2] = 0;
        }
        isPlayerRotatingRight = true;
    }
    else
    {
        // Save on key up
        if (isPlayerRotatingRight == true)
        {
            RefreshMap(true);
            isPlayerRotatingRight = false;
        }
    }

    static bool isPlayerRotatingLeft = false;
    if (IsKeyDown(KEY_F5))
    {
        level->playerStart[2]--;
        if (level->playerStart[2] <= 0)
        {
            level->playerStart[2] = 255;
        }
        isPlayerRotatingLeft = true;
    }
    else
    {
        // Save on key up
        if (isPlayerRotatingLeft == true)
        {
            RefreshMap(true);
            isPlayerRotatingLeft = false;
        }
    }

    if (IsKeyPressed(KEY_ZERO))
    {
        if (selectionLocation.entityType == Entity_Type_Wall && level->mapArray[selectionLocation.mapArrayIndex] > DOOR_MASK)
        {
            int index = DoesPositionHaveDoor(selectionLocation.position);
        
            if (index >= 0)
            {
                RemoveElement(index);
                RefreshMap(true);
            }        
        }
    }

    if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_THREE))
    {
        if (selectionLocation.entityType == Entity_Type_Wall)
        {        
            if (level->mapArray[selectionLocation.mapArrayIndex] > DOOR_MASK)
            {
                uint8_t x = 0;
                uint8_t y = 0;

                GetEntityPositionFromPosition(selectionLocation.position, &x, &y);
                
                
                int index = DoesPositionHaveDoor(selectionLocation.position);                
                bool incCounter = false;

                if (index < 0)
                {                
                    index = _elementCount;
                    incCounter = true;
                }

                level->elements[_elementCount].coords[0] = x;
                level->elements[_elementCount].coords[1] = y;
                
                if (IsKeyPressed(KEY_ONE))
                {
                    level->elements[index].type = SFG_LEVEL_ELEMENT_LOCK0;
                }
                else if (IsKeyPressed(KEY_TWO))
                {
                    level->elements[index].type = SFG_LEVEL_ELEMENT_LOCK1;
                }
                else if (IsKeyPressed(KEY_THREE))
                {
                    level->elements[index].type = SFG_LEVEL_ELEMENT_LOCK2;
                }
                if (incCounter)
                {                
                    _elementCount++;
                }

                RefreshMap(true);
            }
        }
    }
    
    if (IsKeyPressed(KEY_T))
    {
        if (selectionLocation.entityType == Entity_Type_Wall)
        {
            if (level->mapArray[selectionLocation.mapArrayIndex] < DOOR_MASK)
            {                
                int index = level->mapArray[selectionLocation.mapArrayIndex];                
                level->mapArray[selectionLocation.mapArrayIndex] = (index % 7) | DOOR_MASK;
            }
            else
            {
                level->mapArray[selectionLocation.mapArrayIndex] = (level->mapArray[selectionLocation.mapArrayIndex] + 7) & (~DOOR_MASK);
                int k = DoesPositionHaveDoor(selectionLocation.position);
                if(k >= 0)
                {
                    RemoveElement(k);
                }
            }
            RefreshMap(true);
        }
    }

    if (IsKeyPressed(KEY_P))
    {
        if (selectionLocation.entityType != Entity_Type_Item)
        {
            uint8_t col = 0;
            uint8_t row = 0;

            GetEntityPositionFromPosition(selectionLocation.position, &col, &row);

            level->playerStart[0] = col;
            level->playerStart[1] = row;

            if (SaveLevel(level, DEBUG_LEVEL))
            {
                TraceLog(LOG_INFO, "Level saved on player position update");
            }
            else
            {
                TraceLog(LOG_ERROR, "Error saving level to file");
            }
        }
        RefreshMap(true);
    }


    if (IsKeyPressed(KEY_M))
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
            PreviousMode = currentEditorMode;
            currentEditorMode = Mode_Editor;
        }
        else
        {
            _focusedMode = was_orignally_focusedmode;
            camera.position = orignal_cam_pos;
            camera.target = orignal_cam_target;
            camera.projection = CAMERA_PERSPECTIVE;            
            camera.fovy = 65.f;
            was_cursor_hidden ? HideCursor() : ShowCursor();
            currentEditorMode = PreviousMode;
        }
    }

    if (IsKeyPressed(KEY_F2))
    {
        level->ceilHeight = (level->ceilHeight == OUTSIDE_CEIL_VALUE ? level->ceilHeight = level->floorHeight : OUTSIDE_CEIL_VALUE);
        RefreshMap(true);
    }

    if (IsKeyPressed(KEY_F11))
    {
        if(IsWindowFullscreen)            
        { 
            SetWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        }
        else
        {
            int display = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
        }
        ToggleFullscreen();
    }

    if (IsKeyPressed(KEY_G))
    {
        currentEditorMode = (currentEditorMode == Mode_Editor ? Mode_Game : Mode_Editor);
    }

    if (IsKeyPressed(KEY_H))
    {         
        drawHelpText++;
        if (drawHelpText > DebugAndHelp)
        {
            drawHelpText = 0;
        }
    }

    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_PERIOD))
    {
        level->floorHeight++;
        if (level->floorHeight > MAX_WALL_HEIGHT)
        {
            level->floorHeight = MIN_WALL_HEIGHT;
        }

        if (level->ceilHeight < OUTSIDE_CEIL_VALUE)
        {
            level->ceilHeight = level->floorHeight;
        }

        RefreshMap(true);
    }
    else if (IsKeyPressed(KEY_PERIOD))
    {   
        if (level->mapArray[selectionLocation.mapArrayIndex] > DOOR_MASK)
        {
            level->doorLevitation += 1;

            if (level->doorLevitation >= level->floorHeight / 4)
            {
                level->doorLevitation = 0;
            }

            RefreshMap(true);
            
            return;
        }

        // TODO: Do this properly.
        if (selectionLocation.entityType == Entity_Type_Wall) 
        {
            _currentWallSelection += 7;
            if (_currentWallSelection >= TILE_DICTIONARY_SIZE)
            {
                _currentWallSelection -= (TILE_DICTIONARY_SIZE);
                _currentWallSelection += 8;
            }
            level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
            RefreshMap(true);
        }
        
    }

    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_COMMA))
    {
        level->floorHeight--;
        if (level->floorHeight < MIN_WALL_HEIGHT)
        {
            level->floorHeight = MAX_WALL_HEIGHT;
        }

        if (level->ceilHeight < OUTSIDE_CEIL_VALUE)
        {
            level->ceilHeight = level->floorHeight;
        }

        RefreshMap(true);
    }
    else if (IsKeyPressed(KEY_COMMA))
    {     

        if (level->mapArray[selectionLocation.mapArrayIndex] > DOOR_MASK)
        {
            if (level->doorLevitation == 0)
            {
                level->doorLevitation = level->floorHeight / 4;
            }
            level->doorLevitation -= 1;

            RefreshMap(true);

            return;
        }
        
        if (selectionLocation.entityType == Entity_Type_Wall)
        {
            _currentWallSelection -= 7;
            if (_currentWallSelection <= 7)
            {
                _currentWallSelection += (TILE_DICTIONARY_SIZE);
                _currentWallSelection -= 8;
            }
            level->mapArray[selectionLocation.mapArrayIndex] = _currentWallSelection;
            RefreshMap(true);
        }
        
    }

    if (IsKeyPressed(KEY_DELETE))
    {       
        if (selectionLocation.entityType == Entity_Type_Wall)
        {
            if(level->mapArray[selectionLocation.mapArrayIndex] > DOOR_MASK)
            { 

                int doorIndex = DoesPositionHaveDoor(selectionLocation.position);
                if (doorIndex >= 0)
                {                    
                    RemoveElement(doorIndex);
                }
            }

            level->mapArray[selectionLocation.mapArrayIndex] = 0;
            RefreshMap(true);
        }
        else if (selectionLocation.entityType == Entity_Type_Item)
        {
            RemoveElement(selectionLocation.itemIndex);
            RefreshMap(true);
        }
        selectionLocation.entityType = Entity_Type_None;
    }

    if (IsKeyPressed(KEY_RIGHT_BRACKET))
    {   
        ScrollDownEntities();
    }

    if (IsKeyPressed(KEY_LEFT_BRACKET))
    {   
        ScrollUpEntities();
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
        uint8_t h = GetMapArrayHeightFromIndex(e, 0, level->stepSize);
        
        camera.position.y = PLAYER_HEIGHT + (BLOCK_HEIGHT * (h));
    }
    else if (currentEditorMode == Mode_Scene)
    {
        cameraMode = CAMERA_ORBITAL;
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    }


    UpdateFloorHeight();

    int wheelMove = GetMouseWheelMove();

    // Override the default key behavior
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
    else if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        
    }
    else if (wheelMove > 0.5f)
    {
        ScrollDownEntities();
    }
    else if (wheelMove < -0.5f)
    {
        ScrollUpEntities();
    }
    else
    {
        if (!_focusedMode)
        {
            UpdateCamera(&camera, cameraMode);
        }
        
        if(currentEditorMode == Mode_Game)
        {
            CheckPlayerBlockCollision();
        }
    }    
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    ClearBackground(SKYBLUE);
    if (!_levelReady)
    {
        return;
    }

    BeginMode3D(camera);
    DrawPlane((Vector3) { 0.0f, -0.1f, 0.0f }, (Vector2) { 256.0f, 256.0f }, GRAY);
    DrawWalls();

    if (currentEditorMode == Mode_Editor)
    {
        int mod = _floorHeight % 4 > 0;
        
        if (selectionLocation.entityType == Entity_Type_Wall)
        {
            size_t range = ((_floorHeight / 4) + mod);
            for (size_t i = 0; i < range; i++)
            {
                float h = SELECTION_BOX_SIZE;
                float y = (float)i;
                
                if (i == range - 1)
                {                
                    float floor_mod = 0.25f * (_floorHeight % 4);

                    if (floor_mod == 0) { floor_mod = 1; }

                    h = SELECTION_BOX_SIZE * floor_mod;
                    y += h / 2;
                }
                else                    
                {
                    y += 0.5f;
                }
                                
                Vector3 vec = { selectionLocation.position.x,y, selectionLocation.position.z };
                if (selectionLocation.entityType == Entity_Type_Wall)
                {
                    DrawCube(vec, SELECTION_BOX_SIZE, h, SELECTION_BOX_SIZE, TRANS_BLUE);
                    DrawCubeWires(vec, SELECTION_BOX_SIZE, h, SELECTION_BOX_SIZE, BLACK);                    
                }                
            }
        }
        else if (selectionLocation.entityType == Entity_Type_Item)
        {
            Vector3 vec = { selectionLocation.position.x,selectionLocation.position.y, selectionLocation.position.z };
            DrawCubeWires(vec, SELECTION_BOX_SIZE, SELECTION_BOX_SIZE, SELECTION_BOX_SIZE, BLACK);
        }
        else if (selectionLocation.entityType == Entity_Type_Free)
        {
            Vector3 vec = { selectionLocation.position.x, 0.5f , selectionLocation.position.z };
            DrawCubeWires(vec, SELECTION_BOX_SIZE, SELECTION_BOX_SIZE, SELECTION_BOX_SIZE, WHITE);
        }

        DrawGrid(MAP_DIMENSION, 1.0f);
        DrawPlayerStartPosition();
    }

    BeginShaderMode(alphaDiscard);
    DrawElements();    
    EndShaderMode();   
    EndMode3D(); 

    if (currentEditorMode == Mode_Console)
    {
        RenderConsole();
    }   


    if (currentEditorMode == Mode_Game)
    {
        float x = (GetScreenWidth() / 2.0f) - ((weaponsTextures[1].width * GUN_SCALE) / 2);
        float y = GetScreenHeight() - (weaponsTextures[1].height * GUN_SCALE);
        DrawTextureEx(weaponsTextures[1], (Vector2) { x, y}, 0.0f, GUN_SCALE, WHITE);
    }

    if (level)
    {    
        DebugInfo d = { &camera,selectionLocation.mapArrayIndex, _floorHeight, level->ceilHeight == OUTSIDE_CEIL_VALUE, GetFPS(), level->stepSize, strcmp(levelPack, EMPTY) == 0 ? "None set": levelPack};
        EUI_DrawDebugData(&d, drawHelpText);
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

void DrawPlayerStartPosition(void)
{
    float size = 1.f;

    int mapArrayindex = GetMapArrayIndex(level->playerStart[0], level->playerStart[1]);

    uint8_t stepSize = GetMapArrayHeightFromIndex(level->mapArray[mapArrayindex], level->floorHeight, level->stepSize);

    float h = 0.25f * stepSize;    
    float x = level->playerStart[0] + 0.5;
    float y = (size / 2) + h;
    float z = level->playerStart[1] + 0.5;
    Vector3 pos = (Vector3){ x - MAP_DIMENSION / 2, y ,z - MAP_DIMENSION / 2 };
    
    Vector3 source = { 0.f,-1.f,0.f };
    Vector3 scale = { 0.01f, 0.01f,0.01f };
    float rotationSource = DEGRESS_TO_BYTE_CONVERSION * (float)(level->playerStart[2]);
    if (currentRenderMode == RenerMode_Textured)
    {
        if (IsModelReady(playerMarker))
        {        
            DrawModelEx(playerMarker, pos, source, rotationSource, scale, WHITE);
            // DrawModel(playerMarker, pos, 0.005f, WHITE);
        }
    }
    else if (currentRenderMode == RenerMode_Colored)
    {
        DrawCube(pos, 1.0f, 1, 1.0f, YELLOW);
        DrawCubeWires(pos, 1.0f, 1.0f, 1.0f, WHITE);
    }
    else if (currentRenderMode == RenerMode_CollisionBlock)
    {
        DrawCubeWires(pos, 1.0f, 1.0f, 1.0f, YELLOW);
    }
}

void DrawElements(void)
{
    for (size_t i = 0; i < _elementCount; i++)
    {
        if (currentRenderMode == RenerMode_Textured)
        {
            if (items[i].type == SFG_LEVEL_ELEMENT_CARD0 || items[i].type == SFG_LEVEL_ELEMENT_LOCK0)
            {
                DrawBillboard(camera, items[i].texture, items[i].position, items[i].size, RED);
            }
            else if (items[i].type == SFG_LEVEL_ELEMENT_CARD1 || items[i].type == SFG_LEVEL_ELEMENT_LOCK1)
            {
                DrawBillboard(camera, items[i].texture, items[i].position, items[i].size, GREEN);
            }
            else if (items[i].type == SFG_LEVEL_ELEMENT_CARD2 || items[i].type == SFG_LEVEL_ELEMENT_LOCK2)
            {
                DrawBillboard(camera, items[i].texture, items[i].position, items[i].size, BLUE);
            }
            else
            {            
                DrawBillboard(camera, items[i].texture, items[i].position, items[i].size, WHITE);
            }
        }
        else if (currentRenderMode == RenerMode_Colored)
        {
            DrawCube(items[i].position, 1.0f, 1, 1.0f, RED);
            DrawCubeWires(items[i].position, 1.0f, 1.0f, 1.0f, WHITE);
        }
        else if (currentRenderMode == RenerMode_CollisionBlock)
        {
            DrawBoundingBox(items[i].boundingBox, RED);
        }
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
                    if (mapBlocks[i].isDoor)
                    {
                        DrawCube(mapBlocks[i].position, 1.0f, mapBlocks[i].drawHeight, 1.0f, GREEN);
                    }
                    else
                    {
                        DrawCube(mapBlocks[i].position, 1.0f,mapBlocks[i].drawHeight, 1.0f, BLUE);
                    }
                    DrawCubeWires(mapBlocks[i].position, 1.0f, mapBlocks[i].drawHeight, 1.0f, WHITE);
                }
                else if (currentRenderMode == RenerMode_CollisionBlock)
                {   if(mapBlocks[i].textureIndex)
                    DrawBoundingBox(mapBlocks[i].boundingBox, BLUE);
                }
            }
        }
    }
}

// Gameplay Screen Unload logic
void UnloadEngineScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    MemFree(mapBlocks);
    MemFree(items);
    MemFree(level);
    level = NULL;
    _blockCount = 0;    
}

