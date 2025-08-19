#ifndef TIMER_H
#define TIMER_H

#include "raylib.h"

typedef struct
{
    float start;
} Timer;

float Timer_GetElapsedTime(const Timer *timer)
{
    return GetTime() - timer->start;
}

void Timer_Reset(Timer *timer)
{
    timer->start = GetTime();
}

#endif