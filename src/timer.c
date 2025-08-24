#include "raylib.h"

#include "timer.h"

float Timer_GetElapsedTime(const Timer *timer)
{
    return GetTime() - timer->start;
}

void Timer_Reset(Timer *timer)
{
    timer->start = GetTime();
}
