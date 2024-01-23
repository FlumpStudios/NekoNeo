#define RLIGHTS_IMPLEMENTATION
#include "raylib.h"
#include "engine.h"   
#include "constants.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Font font = { 0 };


static void UpdateDrawFrame(void);         


//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PROJECT_NAME);
    font = LoadFont("resources/mecha.png");    
    InitGameplayScreen();
  
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    // Main game loop
    while (!WindowShouldClose())   
    {     
        // Update and draw frame
        UpdateDrawFrame();
    }
#endif

    // Unload stuff here
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
