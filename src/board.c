#include "raylib.h"
#include "raymath.h"
#include "tetrominos.h"
#include "timer.h"
#include "board.h"

extern const Color colors[8] = {
    {0, 0, 0, 255},     // EMPTY
    {0, 255, 255, 255}, // CIAN
    {255, 255, 0, 255}, // YELLOW
    {128, 0, 128, 255}, // PURPLE
    {0, 255, 0, 255},   // GREEN
    {255, 165, 0, 255}, // ORANGE
    {255, 0, 0, 255},   // RED
    {255, 0, 255, 255}, // MAGENTA
};

static Texture2D texture = {0};

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

Board Board_New()
{
    Board board = {0};

    memset(board.cells, 0, sizeof(board.cells));
    Board_NextTetromino(&board);
    Timer_Reset(&board.timer);

    return board;
}

void Board_SetPause(Board *board, bool pause)
{
    board->isPaused = pause;
}

void DrawCell(int x, int y, float cellWidth, float cellHeight, Color color, Texture2D *texture)
{
    if (texture && texture->id > 0)
    {
        DrawTexturePro(
            *texture,
            (Rectangle){0, 0, 7, 7},
            (Rectangle){x, y, cellWidth, cellHeight},
            (Vector2){0, 0},
            0,
            color);

        return;
    }

    DrawRectangle(
        round(x),
        round(y),
        round(cellWidth),
        round(cellHeight),
        color);
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
                    DrawCell(
                        offset_x + gridX * cellWidth,
                        offset_y + gridY * cellHeight,
                        cellWidth,
                        cellHeight,
                        color,
                        &texture);
                }
            }
        }
    }
}

void Board_Draw(const Board *board, Rectangle area)
{
    float cellWidth = round(area.width / GRID_COLS);
    float cellHeight = round(area.height / GRID_ROWS);

    DrawRectangle(
        area.x,
        area.y,
        area.width,
        area.height,
        (Color){40, 40, 132, 255});

    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            int cell = board->cells[row * GRID_COLS + col];

            if (cell == 0)
                continue;

            DrawCell(
                area.x + col * cellWidth,
                area.y + row * cellHeight,
                cellWidth,
                cellHeight,
                colors[cell],
                &texture);
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
    if (board->isPaused)
        return;

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

    int kicks[5][2];

    switch (board->tetromino.shape.type)
    {
    case PIECE_I:
        const int iKicks[4][5][2] = {
            // L -> 0
            {{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}},
            // 0 -> R
            {{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}},
            // R -> 2
            {{0, 0}, {-1, 0}, {+2, 0}, {-1, +2}, {+2, -1}},
            // 2 -> L
            {{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}},
        };

        memcpy(kicks, iKicks[board->tetromino.shape.rotation_index], sizeof(kicks));
        break;

    case PIECE_O:
        const int oKicks[5][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

        memcpy(&kicks, &oKicks, sizeof(oKicks));
        break;

    default:
        const int dKicks[4][5][2] = {
            // L -> 0
            {{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}},
            // 0 -> R
            {{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}},
            // R -> 2
            {{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}},
            // 2 -> L
            {{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}},
        };

        memcpy(kicks, dKicks[board->tetromino.shape.rotation_index], sizeof(kicks));
        break;
    }

    const Vector2 originalPosition = board->tetromino.position;

    for (int i = 0; i < 5; i++)
    {
        Vector2 newPosition = {
            originalPosition.x + kicks[i][0],
            originalPosition.y - kicks[i][1]};

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