#define MAX_TOUCH_POINTS 1
#include "raylib.h"

#ifdef __ANDROID__
#define DEBUG_FONT_SCALE 3.0f
#endif
#include "debug.h"

#include "game.h"

#include <string.h>

Debugger debugger;

void debug_log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Debugger_LogF(&debugger, fmt, args);
    va_end(args);
}

int main(void)
{
#ifdef __ANDROID__
    SetGesturesEnabled(GESTURE_SWIPE_RIGHT | GESTURE_SWIPE_LEFT | GESTURE_SWIPE_UP | GESTURE_SWIPE_DOWN);
    SetConfigFlags(FLAG_WINDOW_TOPMOST | FLAG_WINDOW_UNDECORATED);
    InitWindow(GetScreenWidth(), GetScreenHeight(), "raylib game template");
#else
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 1000, "raylib game template");
#endif

    SetTargetFPS(60);

    debugger = Debugger_New();

#ifdef __ANDROID__
    debug_log("Im in Android");
#else
    debug_log("Im not in Android");
#endif

    GameState *state = malloc(sizeof(GameState));
    *state = GameState_New();

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_T))
            debug_log("T key is down");

        BeginDrawing();
        ClearBackground(RAYWHITE);

        GameState_Update(state);

        Debugger_Update(&debugger);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
