#ifndef TIMER_H
#define TIMER_H

typedef struct
{
    float start;
} Timer;

float Timer_GetElapsedTime(const Timer *timer);

void Timer_Reset(Timer *timer);

#endif