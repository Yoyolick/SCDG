#ifndef DATA_H
#define DATA_H

#include "engine/graphics.h"

#include <jansson.h>

json_t *loadJSONFile(char *path);

json_t *getSaveData(char *path);

json_t *getGameData(char *path);

json_t *getObject(json_t *parent, char *key);
json_t *getObjectNOWARN(json_t *parent, char *key);

int getInteger(json_t *parent, char *key);

bool getBool(json_t *parent, char *key);

float getFloat(json_t* parent, char* key);

char *getString(json_t *parent, char *key);
char *getStringNOWARN(json_t *parent, char *key);

json_t *getArray(json_t *parent, char *key);

json_t *getArrayIndex(json_t *parent, int index);

int getArrayInt(json_t *parent, int index);

char *getArrayString(json_t *parent, int index);

void dumpJSON(json_t *parent);

void saveJSONFile(json_t *data, char *path);

void writeInt(json_t *parent, char *keyName, int toWrite);

void writeArrayInt(json_t *parent, size_t index, int toWrite);

void writeString(json_t *parent, char *keyName, char *toWrite);

#endif