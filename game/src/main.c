#define RLIGHTS_IMPLEMENTATION
#include "raylib.h"
#include "engine.h"   
#include "constants.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#ifdef DEBUG
#define FONT_LOCATION "C:\\Projects\\NekoNeo\\game\\src\\fonts\\mecha.png"
#else
#define FONT_LOCATION "fonts\\mecha.png"
#endif // DEBUG

Font font = { 0 };

static void UpdateDrawFrame(void);         

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    for (size_t i = 0; i < argc; i++)
    {
        if(i == 1)
        { 
            strcpy(levelPack, argv[i]);
            levelPack[50] = NULL;
        }
    }
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PROJECT_NAME);
    font = LoadFont(FONT_LOCATION);
    InitGameplayScreen();
  
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    // Main game loop
    while (!WindowShouldClose() && readyToExit == 0)
    {     
        // Update and draw frame
        UpdateDrawFrame();
    }
#endif

    // Unload stuff here

    // TODO: Finish unloading properly
    UnloadEngineScreen();
    UnloadFont(font);    
    CloseWindow();
    return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    UpdateGameplayScreen();
    DrawGameplayScreen();        
    EndDrawing();    
}
