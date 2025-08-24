#include "raylib.h"

#include "debug.h"

#include "game.h"
#include "cfg.h"

Debugger debugger;

Debugger *get_debugger()
{
    return &debugger;
}

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
    debugger.fontScale = 1.5f;

    GameState *state = malloc(sizeof(GameState));
    *state = GameState_New();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        GameState_Update(state);

        Debugger_Update(&debugger);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
