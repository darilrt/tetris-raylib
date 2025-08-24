#include "game.h"
#include "debug.h"

GameState GameState_New()
{
    GameState state;
    state.screen = SCREEN_GAME;

    state.screenWidth = GetScreenWidth();
    state.screenHeight = GetScreenHeight();

    state.gestureHandler = GestureHandler_New();

    state.grid = Board_New();

    state.theme = Theme_New();

    if (!Theme_Load(&state.theme, "test_pack"))
    {
        TraceLog(LOG_ERROR, "Error loading theme: test_pack");
        return state;
    }

    debug_log("Theme loaded: %s", state.theme.name ? state.theme.name : "No name");

    get_debugger()->font = state.theme.font;

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
    static GestureEvent event;
    static bool blockGesture = false;
    GestureHandler_Process(&state->gestureHandler);

    while (GestureHandler_PullEvent(&state->gestureHandler, &event))
    {
        if (!blockGesture)
        {
            switch (event.gesture)
            {
            case GAMEGESTURE_TAP:
                Board_RotateRight(&state->grid);
                break;

            case GAMEGESTURE_MOVE_LEFT:
                Board_MoveLeft(&state->grid);
                break;

            case GAMEGESTURE_MOVE_RIGHT:
                Board_MoveRight(&state->grid);
                break;

            case GAMEGESTURE_MOVE_UP:
                debug_log("Stored");
                blockGesture = true;
                break;

            case GAMEGESTURE_MOVE_DOWN:
                Board_Fall(&state->grid);
                blockGesture = true;
                break;

            default:
                break;
            }
        }
        else
        {
            switch (event.gesture)
            {
            case GAMEGESTURE_UP:
                blockGesture = false;
                break;

            default:
                break;
            }
        }
    }

    if (IsKeyPressedRepeat(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT))
        Board_MoveRight(&state->grid);
    if (IsKeyPressedRepeat(KEY_LEFT) || IsKeyPressed(KEY_LEFT))
        Board_MoveLeft(&state->grid);
    if (IsKeyPressed(KEY_UP))
        Board_RotateRight(&state->grid);
    if (IsKeyPressed(KEY_E))
        Board_RotateLeft(&state->grid);
    if (IsKeyPressed(KEY_SPACE))
        Board_Fall(&state->grid);
    if (IsKeyPressed(KEY_DOWN))
        Board_MoveDown(&state->grid);

    if (IsKeyPressed(KEY_P))
    {
        Board_SetPause(&state->grid, !state->grid.isPaused);
        debug_log("Pause %s", state->grid.isPaused ? "ON" : "OFF");
    }

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
