#ifndef GAME_H
#define GAME_H

#include <stdlib.h>

#include "raylib.h"
#include "board.h"
#include "gesture.h"

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
} GameState;

GameState GameState_New()
{
    GameState state;
    state.screen = SCREEN_GAME;

    state.screenWidth = GetScreenWidth();
    state.screenHeight = GetScreenHeight();

    state.gestureHandler = GestureHandler_New();

    Board_Init(&state.grid);

    return state;
}

Rectangle FitBox(Rectangle original, int width, int height)
{
    Rectangle result = original;

    float aspectRatio = (float)original.width / (float)original.height;
    if (width / (float)height > aspectRatio)
    {
        result.width = height * aspectRatio;
        result.height = height;
    }
    else
    {
        result.width = width;
        result.height = width / aspectRatio;
    }

    result.x = (width - result.width) / 2;
    result.y = (height - result.height) / 2;

    return result;
}

void DrawGame(const Board *grid, int width, int height)
{
    Rectangle body = FitBox((Rectangle){0, 0, 200, 200}, width, height);

    if (body.width < 1 || body.height < 1)
    {
        return;
    }

    Rectangle leftbar = {
        body.x,
        body.y,
        body.width * 0.25f,
        body.height,
    };
    DrawRectangle(leftbar.x, leftbar.y, leftbar.width, leftbar.height, LIGHTGRAY);

    Rectangle rightbar = {
        body.x + body.width - leftbar.width,
        body.y,
        body.width * 0.25f,
        body.height,
    };
    DrawRectangle(rightbar.x, rightbar.y, rightbar.width, rightbar.height, LIGHTGRAY);

    Rectangle gridbox = {
        body.x + leftbar.width,
        body.y,
        body.width - leftbar.width * 2,
        body.height,
    };
    Board_Draw(grid, gridbox);
}

void GameState_UpdateGame(GameState *state)
{
    GestureHandler_Process(&state->gestureHandler);

    if (IsKeyPressedRepeat(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT) || IsGestureDetected(GESTURE_SWIPE_RIGHT))
        Board_MoveRight(&state->grid);
    if (IsKeyPressedRepeat(KEY_LEFT) || IsKeyPressed(KEY_LEFT) || IsGestureDetected(GESTURE_SWIPE_LEFT))
        Board_MoveLeft(&state->grid);
    if (IsKeyPressed(KEY_UP) || IsGestureDetected(GESTURE_TAP))
        Board_RotateRight(&state->grid);
    if (IsKeyPressed(KEY_E))
        Board_RotateLeft(&state->grid);
    if (IsKeyPressed(KEY_SPACE))
        Board_Fall(&state->grid);
    if (IsKeyPressed(KEY_DOWN))
        Board_MoveDown(&state->grid);

    Board_Update(&state->grid);
    DrawGame(&state->grid, state->screenWidth, state->screenHeight);
}

void GameState_Update(GameState *state)
{
    if (IsWindowResized())
    {
        state->screenWidth = GetScreenWidth();
        state->screenHeight = GetScreenHeight();
    }

    switch (state->screen)
    {
    case SCREEN_TITLE:
        break;
    case SCREEN_GAME:
        GameState_UpdateGame(state);
        break;
    }
}

#endif