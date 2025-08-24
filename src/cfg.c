#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cfg.h"

Config Config_New()
{
    Config config = {0};
    config.field_count = 0;
    config.fields = NULL;
    return config;
}

int ParseLine(const char *line, char *key, char *value)
{
    // Example line: `key=value` or `key = value`
    if (sscanf(line, "%[^=]=%[^\n]", key, value) == 2)
    {
        return 1; // Successfully parsed
    }
    return 0; // Failed to parse
}

char *Trim(const char *str)
{
    while (isspace(*str))
        str++;

    char *end = (char *)str + strlen(str) - 1;

    while (end > str && isspace(*end))
        end--;

    *(end + 1) = '\0';

    return (char *)str;
}

int Config_Load(Config *config, const char *filename)
{
    assert(config != NULL);

    FILE *file = fopen(filename, "r");

    if (!file)
    {
        return 0;
    }

    char c;
    char buffer[1024];
    char key[512];
    char value[512];
    int i = 0;

    while (1)
    {
        c = fgetc(file);

        if (c == '\n' || c == '\r' || c == EOF)
        {
            if (i > 0)
            {
                buffer[i] = '\0';

                if (ParseLine(buffer, key, value))
                {
                    config->fields = realloc(config->fields, sizeof(Field) * (config->field_count + 1));
                    config->fields[config->field_count].key = strdup(Trim(key));
                    config->fields[config->field_count].value = strdup(Trim(value));
                    config->field_count++;
                }
            }
            i = 0;

            if (c == EOF)
            {
                break;
            }
        }
        else
        {
            buffer[i++] = c;

            if (i >= sizeof(buffer) - 1)
            {
                return 0;
            }
        }
    }

    fclose(file);
    return 1;
}

void Config_Free(Config *config)
{
    assert(config != NULL);

    for (int i = 0; i < config->field_count; i++)
    {
        free((void *)config->fields[i].key);
        free((void *)config->fields[i].value);
    }

    free(config->fields);
    config->fields = NULL;
    config->field_count = 0;
}

void Config_Print(const Config *config)
{
    assert(config != NULL);

    for (int i = 0; i < config->field_count; i++)
    {
        printf("%s: %s\n", config->fields[i].key, config->fields[i].value);
    }
}

const char *Config_Get(const Config *config, const char *key)
{
    assert(config != NULL);
    assert(key != NULL);

    for (int i = 0; i < config->field_count; i++)
    {
        if (strcmp(config->fields[i].key, key) == 0)
        {
            return config->fields[i].value;
        }
    }

    return NULL; // Key not found
}

const char *Config_GetOrDefault(const Config *config, const char *key, const char *default_value)
{
    const char *value = Config_Get(config, key);
    return value ? value : default_value;
}