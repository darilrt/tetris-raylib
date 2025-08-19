#ifndef GESTURE_H
#define GESTURE_H

#include "raylib.h"
#include "debug.h"

typedef enum
{
    GAMEGESTURE_NONE,
    GAMEGESTURE_TAP,        // TouchDown and up in the same place
    GAMEGESTURE_MOVE_LEFT,  // TouchMove to the left trigger every step
    GAMEGESTURE_MOVE_RIGHT, // TouchMove to the right trigger every step
    GAMEGESTURE_MOVE_UP,    // TouchMove upwards
    GAMEGESTURE_MOVE_DOWN,  // TouchMove downwards
    GAMEGESTURE_DOWN,       // TouchDown
    GAMEGESTURE_UP,         // TouchUp
} GameGesture;

typedef struct
{
    int touchId;
    int lastCount;
    GameGesture currentGesture;
} GestureHandler;

GestureHandler GestureHandler_New()
{
    GestureHandler handler;
    handler.touchId = -1;
    handler.lastCount = 0;
    handler.currentGesture = GAMEGESTURE_NONE;
    return handler;
}

void GestureHandler_Process(GestureHandler *handler)
{
    int touchCount = GetTouchPointCount();

    if (touchCount > 0)
    {
        int id = GetTouchPointId(0);

        if (handler->touchId == -1)
        {
            if (handler->lastCount == 0)
            {
                handler->touchId = id;
                handler->currentGesture = GAMEGESTURE_DOWN;
                debug_log("Touch started: %d", id);
            }
            else
            {
                handler->currentGesture = GAMEGESTURE_NONE;
            }
        }
        else
        {
            if (handler->touchId == id)
            {
                Vector2 position = GetTouchPosition(0);
                debug_log("Touch moved: %d, pos x: %f, pos y: %f", id, position.x, position.y);
            }
            else
            {
                handler->currentGesture = GAMEGESTURE_UP;
                handler->touchId = -1;
                debug_log("Touch up: %d", id);
            }
        }
    }
    else
    {
        if (handler->touchId != -1)
        {
            handler->currentGesture = GAMEGESTURE_UP;
            handler->touchId = -1;
            debug_log("Touch ended: %d", handler->touchId);
        }
        else
        {
            handler->currentGesture = GAMEGESTURE_NONE;
        }
    }

    handler->lastCount = touchCount;
}

#endif