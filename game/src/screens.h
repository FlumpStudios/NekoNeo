#ifndef SCREENS_H
#define SCREENS_H


//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
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
void UnloadGameplayScreen(void);
int FinishGameplayScreen(void);


#ifdef __cplusplus
}
#endif

#endif // SCREENS_H