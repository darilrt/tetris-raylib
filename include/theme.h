#ifndef THEME_H
#define THEME_H

#include "raylib.h"

typedef struct
{
    const char *name;
    Font font;

    struct
    {
        // Board
        Color boardColor;
        Texture2D boardTexture;

        // Block
        Color blockColors[7];
        Texture2D blockTexture;
    } board;
} Theme;

Theme Theme_New();

int Theme_Load(Theme *theme, const char *themeName);

#endif