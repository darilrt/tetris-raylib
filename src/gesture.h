#ifndef GESTURE_H
#define GESTURE_H

#include "raylib.h"
#include "debug.h"

#define GESTURE_MIN_DISTANCE 25
#define GESTURE_EVENT_BUFFER_SIZE 64
#define GESTURE_STEP_DISTANCE 70

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
    GameGesture gesture;
    Vector2 position;
} GestureEvent;

typedef struct
{
    // Configuration
    float tapThreshold;
    float stepThreshold;

    int touchId;
    int lastCount;

    GestureEvent eventBuffer[GESTURE_EVENT_BUFFER_SIZE];
    int eventCount;

    Vector2 startPosition; // Starting position of the gesture
    Vector2 lastPosition;  // Last position of the gesture

    bool isMoving; // If the gesture is currently moving cannot be tapped
} GestureHandler;

GestureHandler GestureHandler_New()
{
    GestureHandler handler;
    handler.touchId = -1;
    handler.lastCount = 0;
    handler.isMoving = false;
    handler.tapThreshold = GESTURE_MIN_DISTANCE;
    handler.stepThreshold = GESTURE_STEP_DISTANCE;
    return handler;
}

void GestureHandler_PushEvent(GestureHandler *handler, GameGesture gesture, Vector2 position)
{
    if (handler->eventCount < GESTURE_EVENT_BUFFER_SIZE)
    {
        handler->eventBuffer[handler->eventCount++] = (GestureEvent){gesture, position};
    }
}

void GestureHandler_ProcessMovement(GestureHandler *handler)
{
    Vector2 lastPosition = GetTouchPosition(0);

    if (Vector2Distance(handler->lastPosition, lastPosition) > handler->stepThreshold)
    {
        handler->isMoving = true;

        GameGesture direction = GAMEGESTURE_NONE;

        bool isMovingHorizontal = fabs(lastPosition.x - handler->lastPosition.x) > fabs(lastPosition.y - handler->lastPosition.y);

        if (isMovingHorizontal)
        {
            if (lastPosition.x > handler->lastPosition.x)
            {
                direction = GAMEGESTURE_MOVE_RIGHT;
            }
            else if (lastPosition.x < handler->lastPosition.x)
            {
                direction = GAMEGESTURE_MOVE_LEFT;
            }
        }
        else
        {
            if (lastPosition.y > handler->lastPosition.y)
            {
                direction = GAMEGESTURE_MOVE_DOWN;
            }
            else if (lastPosition.y < handler->lastPosition.y)
            {
                direction = GAMEGESTURE_MOVE_UP;
            }
        }

        if (direction != GAMEGESTURE_NONE)
        {
            GestureHandler_PushEvent(
                handler,
                direction,
                lastPosition);

            handler->lastPosition = lastPosition;
        }
    }
}

void GestureHandler_CheckTapGesture(GestureHandler *handler)
{
    if (handler->touchId == -1 || handler->isMoving)
    {
        return;
    }

    Vector2 lastPosition = GetTouchPosition(0);

    if (Vector2Distance(handler->startPosition, lastPosition) < handler->tapThreshold)
    {
        Vector2 middlePoint = Vector2Add(handler->startPosition, lastPosition);
        middlePoint = Vector2Scale(middlePoint, 0.5f);

        GestureHandler_PushEvent(handler, GAMEGESTURE_TAP, middlePoint);
    }
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
                handler->isMoving = false;
                handler->startPosition = GetTouchPosition(0);
                handler->lastPosition = handler->startPosition;

                GestureHandler_PushEvent(handler, GAMEGESTURE_DOWN, handler->lastPosition);
            }
            else
            {
                // Do nothing
            }
        }
        else
        {
            if (handler->touchId == id)
            {
                GestureHandler_ProcessMovement(handler);
            }
            else
            {
                handler->isMoving = false;
                Vector2 lastPosition = GetTouchPosition(0);
                GestureHandler_PushEvent(handler, GAMEGESTURE_UP, lastPosition);

                GestureHandler_CheckTapGesture(handler);

                handler->touchId = -1;
                handler->lastPosition = lastPosition;
            }
        }
    }
    else
    {
        if (handler->touchId != -1)
        {
            handler->isMoving = false;
            Vector2 lastPosition = GetTouchPosition(0);
            GestureHandler_PushEvent(handler, GAMEGESTURE_UP, lastPosition);

            GestureHandler_CheckTapGesture(handler);

            handler->touchId = -1;
            handler->lastPosition = lastPosition;
        }
        else
        {
            // Do nothing
        }
    }

    handler->lastCount = touchCount;
}

bool GestureHandler_PullEvent(GestureHandler *handler, GestureEvent *event)
{
    if (handler->eventCount > 0)
    {
        *event = handler->eventBuffer[0];

        // Shift events down
        for (int i = 1; i < handler->eventCount; i++)
        {
            handler->eventBuffer[i - 1] = handler->eventBuffer[i];
        }
        handler->eventCount--;
        return true;
    }
    return false;
}

#endif