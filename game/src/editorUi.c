#define TEXT_COLOUR BLACK
#define RECTANGLE_COLOR SKYBLUE
#define RECTANGLE_EDGE_COLOUR BLUE
#define Y_START 15
#define SPACING 15
#define RECTANGLE_ALPHA 0.25f

#include "editorUi.h"
#include <stdint.h>

static void DrawControls()
{
    uint8_t y = Y_START;

    DrawText("- Render mode keys: R", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText("- Toggle gameplay: G", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Cycle wall height: comma(,) and period (.)", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Cycle level door height: comma(,) and period(.) when highlighting door column", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Cycle level height: Shift + comma(,)/period(.)", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Cycle wall texture / cycle items : \"[\" and \"]\" or mouse wheel", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Set selected wall as door : T", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Set player start position : P", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Rotate player start position : F5/F6 : P", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Toggle fullsreen : F11", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Add lock to door : 1,2,3. Zero to remove", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText("- Focus mode (lock cam and reveal cursor): Middle mouse", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Overhead Perspective (Map view): M", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- open Console: Tab", 15, y, 10, TEXT_COLOUR);
    y += SPACING;


 
    DrawText("- Show/Hide help and debug info: H", 15, y, 10, TEXT_COLOUR);
    DrawRectangle(5, Y_START - 10, 430, y + 10, Fade(RECTANGLE_COLOR, RECTANGLE_ALPHA));
    DrawRectangleLines(Y_START - 10, 5, 430, y + 10, RECTANGLE_EDGE_COLOUR);
}


static void DrawCameraInfo(DebugInfo* debugInfo)
{
    int xpos = GetScreenWidth() - 200;

    uint8_t y = Y_START;

    DrawText(TextFormat("- Level pack: %s", debugInfo->levelPack), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->position.x, debugInfo->camera->position.y, debugInfo->camera->position.z), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->target.x, debugInfo->camera->target.y, debugInfo->camera->target.z), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;

    /*DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->up.x, debugInfo->camera->up.y, debugInfo->camera->up.z), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;*/

    DrawText(TextFormat("- Map Array Index: %i", debugInfo->arrayCell), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- Floor Height: %i", debugInfo->floorHeight), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- Step Size: %i", debugInfo->stepSize), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- Current Fps: %i", debugInfo->fps), xpos, y, 10, TEXT_COLOUR);

    y += SPACING;

    DrawText(TextFormat("- Remaining Elements: %i", debugInfo->remainingElements), xpos, y, 10, TEXT_COLOUR);
    
   
    y += SPACING + 5;
    
    DrawLine(xpos - 10, y, xpos + 185, y, RECTANGLE_EDGE_COLOUR);

    y += 5;

    DrawText("- Currently selected wall", xpos, y + 10, 10, TEXT_COLOUR);
    DrawTexture(debugInfo->selectedWall, xpos + 140, y, WHITE);
    
    y += SPACING + 21;

    DrawText("- Currently selected item", xpos, y + 10, 10, TEXT_COLOUR);

    DrawTexture(debugInfo->selectedItem, xpos + 140, y, WHITE);
    
    y += SPACING + 10;

    DrawRectangle(xpos -10, Y_START - 10, 195, y + 10, Fade(RECTANGLE_COLOR, RECTANGLE_ALPHA));
    DrawRectangleLines(xpos - 10, Y_START - 10, 195, y + 10, RECTANGLE_EDGE_COLOUR);
}


void EUI_DrawDebugData(DebugInfo* debugInfo, UiMode mode)
{
    if (mode == Off) { return;}

    if (mode > Off)
    {
        DrawCameraInfo(debugInfo);
        if (mode > DebugOnly)
        {        
            DrawControls();
        }
    }
}