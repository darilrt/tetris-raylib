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
    float fontScale;
    Vector2 screenSize;

    Log logs[MAX_LOGS];
    int logCount;
} Debugger;

Debugger Debugger_New();

void Debugger_RemoveLog(Debugger *debugger, int index);

void Debugger_Update(Debugger *debugger);

void Debugger_Log(Debugger *debugger, const char *message, size_t length);

void Debugger_LogF(Debugger *debugger, const char *format, va_list args);

// This functions needs to be implemented by the user
void debug_log(const char *fmt, ...);
Debugger *get_debugger();

#endif