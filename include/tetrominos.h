#ifndef TETROMINOS_H
#define TETROMINOS_H

#include <string.h>
#include <assert.h>

typedef enum
{
    PIECE_I = 0,
    PIECE_O = 1,
    PIECE_T = 2,
    PIECE_S = 3,
    PIECE_Z = 4,
    PIECE_J = 5,
    PIECE_L = 6,
} PieceType;

typedef struct
{
    int cells[4][4];
    PieceType type;
    int rotation_index;
} Tetromino;

inline void Tetromino_CopyTo(const Tetromino *src, Tetromino *dest)
{
    memcpy(dest, src, sizeof(Tetromino));
}

static int Tetromino_At(const Tetromino *tetromino, int row, int col)
{
    assert(row >= 0 && row < 4 && col >= 0 && col < 4);
    return tetromino->cells[row][col];
}

Tetromino Tetromino_New(PieceType type, int rotation);

void Tetromino_Rotate(Tetromino *tetromino, int times);

#endif