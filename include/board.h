#ifndef BOARD_H
#define BOARD_H

#include <string.h>

#include "raylib.h"
#include "raymath.h"
#include "tetrominos.h"
#include "timer.h"

#define GRID_ROWS 20
#define GRID_COLS 10

extern const Color colors[8];

typedef enum
{
    CLOCKWISE = 1,
    COUNTERCLOCKWISE = 3
} RotationDirection;

#define MIN_FALL_TIME 0.1f
#define MAX_FALL_TIME 0.9f

typedef struct
{
    bool isPaused;
    int cells[GRID_ROWS * GRID_COLS];

    struct
    {
        Tetromino shape;
        Vector2 position;
        Vector2 ghostPosition;
        int color;
    } tetromino;

    Timer timer;
} Board;

bool CheckCollision(const Board *board, int x, int y);

void Board_ClearLines(Board *board);

void Board_UpdateGhostPosition(Board *board);

void Board_NextTetromino(Board *board);

void Board_ApplyTetromino(Board *board);

void Board_MoveDown(Board *board);

Board Board_New();

void Board_SetPause(Board *board, bool pause);

void DrawCell(int x, int y, float cellWidth, float cellHeight, Color color, Texture2D *texture);

void DrawTetromino(
    const Tetromino *tetromino,
    Vector2 position,
    Color color,
    float cellWidth,
    float cellHeight,
    int offset_x,
    int offset_y);

void Board_Draw(const Board *board, Rectangle area);

void Board_Update(Board *board);

void Board_Fall(Board *board);

void Board_MoveRight(Board *board);

void Board_MoveLeft(Board *board);

void Board_TryRotateWithKick(Board *board, RotationDirection direction);

void Board_RotateRight(Board *board);

void Board_RotateLeft(Board *board);

#endif