#ifndef BOARD_H
#define BOARD_H

#include <string.h>

#include "raylib.h"
#include "raymath.h"
#include "tetrominos.h"
#include "timer.h"

#define GRID_ROWS 20
#define GRID_COLS 10

const Color colors[8] = {
    {0, 0, 0, 255},     // EMPTY
    {0, 255, 255, 255}, // CIAN
    {255, 255, 0, 255}, // YELLOW
    {128, 0, 128, 255}, // PURPLE
    {0, 255, 0, 255},   // GREEN
    {255, 165, 0, 255}, // ORANGE
    {255, 0, 0, 255},   // RED
    {255, 0, 255, 255}, // MAGENTA
};

typedef enum
{
    CLOCKWISE = 1,
    COUNTERCLOCKWISE = 3
} RotationDirection;

#define MIN_FALL_TIME 0.1f
#define MAX_FALL_TIME 0.9f

typedef struct
{
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

bool CheckCollision(const Board *board, int x, int y)
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (Tetromino_At(&board->tetromino.shape, row, col))
            {
                int gridX = x + col;
                int gridY = y + row;

                if (gridX < 0 || gridX >= GRID_COLS || gridY >= GRID_ROWS)
                {
                    return true;
                }

                if (gridY > 0 && board->cells[gridY * GRID_COLS + gridX] > 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void Board_ClearLines(Board *board)
{
    for (int row = GRID_ROWS - 1; row >= 0; row--)
    {
        bool fullLine = true;

        for (int col = 0; col < GRID_COLS; col++)
        {
            if (board->cells[row * GRID_COLS + col] == 0)
            {
                fullLine = false;
                break;
            }
        }

        if (fullLine)
        {
            // Shift all lines above down
            for (int r = row; r > 0; r--)
            {
                memcpy(&board->cells[r * GRID_COLS], &board->cells[(r - 1) * GRID_COLS], sizeof(int) * GRID_COLS);
            }

            // Clear the top line
            memset(&board->cells[0], 0, sizeof(int) * GRID_COLS);
            row++; // Check this row again after shifting
        }
    }
}

void Board_UpdateGhostPosition(Board *board)
{
    Vector2 ghostPos = board->tetromino.position;

    while (!CheckCollision(board, ghostPos.x, ghostPos.y + 1))
    {
        ghostPos.y++;
    }

    board->tetromino.ghostPosition = ghostPos;
}

void Board_NextTetromino(Board *board)
{
    board->tetromino.shape = Tetromino_New(rand() % 7, rand() % 4);
    board->tetromino.position = (Vector2){GRID_COLS / 2 - 2, -1};
    board->tetromino.color = rand() % 7 + 1;

    Board_UpdateGhostPosition(board);
}

void Board_ApplyTetromino(Board *board)
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (Tetromino_At(&board->tetromino.shape, row, col))
            {
                int gridX = board->tetromino.position.x + col;
                int gridY = board->tetromino.position.y + row;

                if (gridY >= 0 && gridY < GRID_ROWS && gridX >= 0 && gridX < GRID_COLS)
                {
                    board->cells[gridY * GRID_COLS + gridX] = board->tetromino.color;
                }
            }
        }
    }

    Board_ClearLines(board);
    Board_NextTetromino(board);
}

void Board_MoveDown(Board *board)
{
    int newPos = board->tetromino.position.y + 1;

    if (!CheckCollision(board, board->tetromino.position.x, newPos))
    {
        board->tetromino.position.y = newPos;
        Board_UpdateGhostPosition(board);
        Timer_Reset(&board->timer);
        return;
    }

    Board_ApplyTetromino(board);
    Timer_Reset(&board->timer);
}

void Board_Init(Board *board)
{
    memset(board->cells, 0, sizeof(board->cells));

    Board_NextTetromino(board);

    Timer_Reset(&board->timer);
}

void DrawTetromino(
    const Tetromino *tetromino,
    Vector2 position,
    Color color,
    float cellWidth,
    float cellHeight,
    int offset_x,
    int offset_y)
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (Tetromino_At(tetromino, row, col))
            {
                int gridX = position.x + col;
                int gridY = position.y + row;

                if (gridX >= 0 && gridX < GRID_COLS && gridY >= 0 && gridY < GRID_ROWS)
                {
                    DrawRectangle(
                        offset_x + gridX * cellWidth - 1,
                        offset_y + gridY * cellHeight - 1,
                        cellWidth + 1,
                        cellHeight + 1,
                        color);
                }
            }
        }
    }
}

void Board_Draw(const Board *board, Rectangle area)
{
    float cellWidth = area.width / GRID_COLS;
    float cellHeight = area.height / GRID_ROWS;

    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            int cell = board->cells[row * GRID_COLS + col];

            DrawRectangle(
                area.x + col * cellWidth - 1,
                area.y + row * cellHeight - 1,
                cellWidth + 1,
                cellHeight + 1,
                colors[cell]);
        }
    }

    const Color ghostColor = {255, 255, 255, 100}; // Semi-transparent white for ghost
    DrawTetromino(
        &board->tetromino.shape,
        board->tetromino.ghostPosition,
        ghostColor,
        cellWidth,
        cellHeight,
        area.x,
        area.y);

    DrawTetromino(
        &board->tetromino.shape,
        board->tetromino.position,
        colors[board->tetromino.color],
        cellWidth,
        cellHeight,
        area.x,
        area.y);
}

void Board_Update(Board *board)
{
    if (Timer_GetElapsedTime(&board->timer) > MAX_FALL_TIME)
    {
        Board_MoveDown(board);
    }
}

void Board_Fall(Board *board)
{
    while (!CheckCollision(board, board->tetromino.position.x, board->tetromino.position.y + 1))
    {
        board->tetromino.position.y++;
    }

    Board_ApplyTetromino(board);
}

void Board_MoveRight(Board *board)
{
    int newPos = board->tetromino.position.x + 1;

    if (!CheckCollision(board, newPos, board->tetromino.position.y))
    {
        board->tetromino.position.x = newPos;
        Board_UpdateGhostPosition(board);
    }
}

void Board_MoveLeft(Board *board)
{
    int newPos = board->tetromino.position.x - 1;

    if (!CheckCollision(board, newPos, board->tetromino.position.y))
    {
        board->tetromino.position.x = newPos;
        Board_UpdateGhostPosition(board);
    }
}

void Board_TryRotateWithKick(Board *board, RotationDirection direction)
{
    Tetromino_Rotate(&board->tetromino.shape, direction);

    if (!CheckCollision(board, board->tetromino.position.x, board->tetromino.position.y))
    {
        Board_UpdateGhostPosition(board);
        return;
    }

    const int kicks[5][2] = {
        {0, 0},
        {-1, 0},
        {-1, 1},
        {0, -2},
        {-1, -2},
    };
    const Vector2 originalPosition = board->tetromino.position;

    for (int i = 0; i < 5; i++)
    {
        Vector2 newPosition = {
            originalPosition.x + kicks[i][0],
            originalPosition.y + kicks[i][1]};

        if (!CheckCollision(board, newPosition.x, newPosition.y))
        {
            board->tetromino.position = newPosition;
            Board_UpdateGhostPosition(board);
            return;
        }
    }

    Tetromino_Rotate(&board->tetromino.shape, direction == CLOCKWISE ? COUNTERCLOCKWISE : CLOCKWISE);
}

void Board_RotateRight(Board *board)
{
    Board_TryRotateWithKick(board, CLOCKWISE);
}

void Board_RotateLeft(Board *board)
{
    Board_TryRotateWithKick(board, COUNTERCLOCKWISE);
}

#endif