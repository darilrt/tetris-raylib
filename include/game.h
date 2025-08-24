#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include <stdlib.h>

#include "board.h"
#include "gesture.h"
#include "theme.h"

typedef enum
{
    SCREEN_TITLE,
    SCREEN_GAME,
} Screen;

typedef struct
{
    Screen screen;
    int screenWidth;
    int screenHeight;

    Board grid;
    GestureHandler gestureHandler;

    Theme theme;
} GameState;

GameState GameState_New();

Rectangle FitBox(Rectangle original, int width, int height);

void DrawGame(const Board *grid, int width, int height);

void GameState_UpdateGame(GameState *state);

void GameState_Update(GameState *state);

#endif