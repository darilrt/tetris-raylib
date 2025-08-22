#ifndef DEBUG_H
#define DEBUG_H

#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <string.h>

#ifndef MAX_LOGS
#define MAX_LOGS 15
#endif

#ifndef LOG_SIZE
#define LOG_SIZE 256
#endif

#ifndef LOG_LIFE
#define LOG_LIFE 5.0f
#endif

#ifndef DEBUG_FONT_SCALE
#define DEBUG_FONT_SCALE 1.0f
#endif

#ifndef DEBUG_COLOR
#define DEBUG_COLOR RED
#endif

typedef struct
{
    char message[LOG_SIZE];
    float timestamp;
} Log;

typedef struct
{
    Font font;
    Vector2 screenSize;

    Log logs[MAX_LOGS];
    int logCount;
} Debugger;

Debugger Debugger_New()
{
    Debugger debugger;
    debugger.font = LoadFontEx("resources/test_pack/font.ttf", 32, 0, 250);
    debugger.logCount = 0;
    debugger.screenSize = (Vector2){
        (float)GetScreenWidth(),
        (float)GetScreenHeight(),
    };
    return debugger;
}

void Debugger_RemoveLog(Debugger *debugger, int index)
{
    if (index < 0 || index >= debugger->logCount)
        return;

    for (int i = index; i < debugger->logCount - 1; i++)
    {
        debugger->logs[i] = debugger->logs[i + 1];
    }
    debugger->logCount--;
}

void Debugger_Update(Debugger *debugger)
{
    if (IsWindowResized())
    {
        debugger->screenSize = (Vector2){
            (float)GetScreenWidth(),
            (float)GetScreenHeight(),
        };
    }

    float fontSize = (debugger->screenSize.x / 50.0f) * DEBUG_FONT_SCALE;

    const float margin = 5;
    float y = margin;

    for (int i = 0; i < debugger->logCount; i++)
    {
        Log *log = &debugger->logs[i];

        float timeDiff = GetTime() - log->timestamp;

        if (timeDiff > LOG_LIFE)
        {
            Debugger_RemoveLog(debugger, i);
            i--;
        }

        Color color = Fade(DEBUG_COLOR, Clamp(LOG_LIFE - timeDiff, 0.0f, 1.0f));

        DrawTextEx(debugger->font, log->message, (Vector2){margin, y}, fontSize, 1, color);
        y += debugger->font.baseSize * fontSize / debugger->font.baseSize + margin;
    }
}

void Debugger_Log(Debugger *debugger, const char *message, size_t length)
{
    if (debugger->logCount < MAX_LOGS)
    {
        strncpy(
            debugger->logs[debugger->logCount].message,
            message,
            sizeof(char) * LOG_SIZE);

        debugger->logs[debugger->logCount].message[LOG_SIZE - 1] = '\0'; // Ensure null-termination
        debugger->logs[debugger->logCount].timestamp = GetTime();
        debugger->logCount++;
    }
    else
    {
        Debugger_RemoveLog(debugger, 0);
        Debugger_Log(debugger, message, length);
    }
}

void Debugger_LogF(Debugger *debugger, const char *format, va_list args)
{
    char buffer[LOG_SIZE];
    int n = vsnprintf(buffer, sizeof(buffer), format, args);

    if (n < 0)
    {
        return;
    }

    size_t len = (n < sizeof(buffer)) ? n : sizeof(buffer) - 1;
    Debugger_Log(debugger, buffer, len);
}

void debug_log(const char *fmt, ...);

#endif