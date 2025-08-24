#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "theme.h"
#include "cfg.h"

Theme Theme_New()
{
    Theme theme = {0};
    return theme;
}

int Theme_Load(Theme *theme, const char *themeName)
{
    if (!themeName)
    {
        printf("Error: themeName es NULL!\n");
        return 0;
    }

    int len = strlen("resources/themes/") + strlen(themeName) + strlen("/theme.cfg") + 1;
    char *path = malloc(len);

    if (!path)
    {
        printf("Error: malloc falló!\n");
        return 0;
    }

    strcpy(path, "resources/themes/");
    strcat(path, themeName);
    strcat(path, "/theme.cfg");

    Config config = Config_New();

    if (!Config_Load(&config, path))
    {
        free(path);
        return 0;
    }

    theme->name = strdup(Config_GetOrDefault(&config, "name", "Unknown Name"));

    char *fontPath;
    if ((fontPath = Config_Get(&config, "font")) != NULL)
    {
        int len = strlen("resources/themes/") + strlen(themeName) + strlen("/") + strlen(fontPath) + 1;
        char *fullFontPath = malloc(len);

        if (fullFontPath)
        {
            strcpy(fullFontPath, "resources/themes/");
            strcat(fullFontPath, themeName);
            strcat(fullFontPath, "/");
            strcat(fullFontPath, fontPath);

            theme->font = LoadFont(fullFontPath);

            free(fullFontPath);
        }
    }
    else
    {
        theme->font = LoadFont("resources/fonts/mecha.png");
    }

    free(path);
    return 1;
}