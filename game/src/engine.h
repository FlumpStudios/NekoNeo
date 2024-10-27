#ifndef SCREENS_H
#define SCREENS_H

extern Font font;


#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void DrawCrossHair();
void DrawElements();
void DrawWalls();
void UnloadEngineScreen(void);

#ifdef __cplusplus
}
#endif

char levelPack[51];

#endif // SCREENS_H