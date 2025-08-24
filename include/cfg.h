#ifndef CONFIG_H
#define CONFIG_H

typedef struct
{
    const char *key;
    const char *value;
} Field;

// cfg file format for serialization and deserialization
typedef struct
{
    Field *fields;
    int field_count;
} Config;

Config Config_New();

int Config_Load(Config *config, const char *filename);

void Config_Free(Config *config);

void Config_Print(const Config *config);

const char *Config_Get(const Config *config, const char *key);

const char *Config_GetOrDefault(const Config *config, const char *key, const char *default_value);

#endif // CONFIG_H