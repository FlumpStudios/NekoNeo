#include "drawTextUtils.h"
#include "constants.h"

void drawTextWithfont(Font font, const char* text, int posX, int posY, int fontSize, Color color)
{
    if (font.texture.id != 0)
    {
        Vector2 position = { (float)posX, (float)posY };
                
        DrawTextEx(font, text, position, (float)fontSize, FONT_SPACING, color);
    }
}