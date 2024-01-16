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

    DrawText("- Cycle level height: Shift + comma(,)/period (.)", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Cycle wall texture / cycle items : [ and ]", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Set selected wall as door  : P", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Toggle fullsreen : F11", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Toggle level outside : F2", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText("- Focus mode (lock cam and reveal cursor): Middle mouse", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Overhead Perspective: Tab", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
 
    DrawText("- Show/Hide help and debug info: H", 15, y, 10, TEXT_COLOUR);
    DrawRectangle(5, Y_START - 10, 330, y + 10, Fade(RECTANGLE_COLOR, RECTANGLE_ALPHA));
    DrawRectangleLines(Y_START - 10, 5, 330, y + 10, RECTANGLE_EDGE_COLOUR);
}


static void DrawCameraInfo(DebugInfo* debugInfo)
{
    int xpos = GetScreenWidth() - 200;

    uint8_t y = Y_START;

    DrawText("Camera status:", xpos, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->position.x, debugInfo->camera->position.y, debugInfo->camera->position.z), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->target.x, debugInfo->camera->target.y, debugInfo->camera->target.z), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->up.x, debugInfo->camera->up.y, debugInfo->camera->up.z), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText(TextFormat("- Is Camera Clipping wall: %u", debugInfo->wallCollision), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- Map Array Index: %i", debugInfo->arrayCell), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- FLoor height: %i", debugInfo->floorHeight), xpos, y, 10, TEXT_COLOUR);
    y += SPACING;

    const char* outsideString = debugInfo->isOutside ? "True" : "False";
    DrawText(TextFormat("- Is level outsude: %s", outsideString), xpos, y, 10, TEXT_COLOUR);
    

    DrawRectangle(xpos -10, Y_START - 10, 195, y + 10, Fade(RECTANGLE_COLOR, RECTANGLE_ALPHA));
    DrawRectangleLines(xpos - 10, Y_START - 10, 195, y + 10, RECTANGLE_EDGE_COLOUR);
}


void EUI_DrawDebugData(DebugInfo* debugInfo)
{
    DrawControls();
    DrawCameraInfo(debugInfo);
}