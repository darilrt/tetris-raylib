#ifndef GESTURE_H
#define GESTURE_H

#include "raylib.h"

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

GestureHandler GestureHandler_New();

void GestureHandler_PushEvent(GestureHandler *handler, GameGesture gesture, Vector2 position);

void GestureHandler_ProcessMovement(GestureHandler *handler);

void GestureHandler_CheckTapGesture(GestureHandler *handler);

void GestureHandler_Process(GestureHandler *handler);

bool GestureHandler_PullEvent(GestureHandler *handler, GestureEvent *event);

#endif