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

    DrawText("Camera controls:", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText("- Look around: arrow keys or mouse", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText("- Render mode keys: R", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText("- Toggle gameplay: G", 15, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText("- SHow/Hide help and debug info: H", 15, y, 10, TEXT_COLOUR);

    DrawRectangle(5, Y_START - 10, 330, y + 10, Fade(RECTANGLE_COLOR, RECTANGLE_ALPHA));
    DrawRectangleLines(Y_START - 10, 5, 330, y + 10, RECTANGLE_EDGE_COLOUR);
}

static void DrawCameraInfo(DebugInfo* debugInfo)
{
    uint8_t y = Y_START;

    DrawText("Camera status:", 610, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->position.x, debugInfo->camera->position.y, debugInfo->camera->position.z), 610, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->target.x, debugInfo->camera->target.y, debugInfo->camera->target.z), 610, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", debugInfo->camera->up.x, debugInfo->camera->up.y, debugInfo->camera->up.z), 610, y, 10, TEXT_COLOUR);
    y += SPACING;
    
    DrawText(TextFormat("- Clipping wall: %u", debugInfo->wallCollision), 610, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- Map Array Index: %i", debugInfo->arrayCell), 610, y, 10, TEXT_COLOUR);
    y += SPACING;

    DrawText(TextFormat("- FLoor height: %i", debugInfo->floorHeight), 610, y, 10, TEXT_COLOUR);

    DrawRectangle(600, Y_START - 10, 195, y + 10, Fade(RECTANGLE_COLOR, RECTANGLE_ALPHA));
    DrawRectangleLines(600, Y_START - 10, 195, y + 10, RECTANGLE_EDGE_COLOUR);
}


void EUI_DrawDebugData(DebugInfo* debugInfo)
{
    DrawControls();
    DrawCameraInfo(debugInfo);
}