#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <SDL2/SDL_ttf.h>

// TODO PLEASE PLEASE MOVE TO ENGINE CALLBACK HANDLER STUFF FUNCTION FILE

struct callbackData {
    char *callbackType;
    void (*callback)(struct callbackData data);
    union {
        char *param1str;
        int param1int;
        float param1float;
    } param1;
    union {
        char *param2str;
        int param2int;
        float param2float;
    } param2;
    union {
        char *param3str;
        int param3int;
        float param3float;
    } param3;
    union {
        char *param4str;
        int param4int;
        float param4float;
    } param4;
};

// linked list holding pointers towards button render objects
typedef struct button {
    struct renderObject *pObject;
    struct button *pNext;
    struct callbackData callbackData;
} button;

// SKJDFGLKJSHFGKLFDSGKJHDFKGHDKFJHGKFDJHGKJDFHGKJHSDKHGKFSHGKFDHKGJHDKFJHG

// enum denoting all possible renderObject types
typedef enum {
    renderType_Text,
    renderType_Image,
    renderType_Button,
} renderObjectType;

// struct defining renderObject(s)
typedef struct renderObject {
    // common to every render object
    int identifier;
    int depth;
    renderObjectType type;
    SDL_Texture *pTexture;
    SDL_Rect rect;
    struct renderObject *pNext;
} renderObject;


void addRenderObject(int identifier, renderObjectType type, int depth, float x, float y, float width, float height, SDL_Texture *pTexture, bool centered);

void removeRenderObject(int identifier);

renderObject *getRenderObject(int identifier);

TTF_Font *loadFont(const char *pFontPath, int fontSize);

// Create a texture from image path, returns NULL for failure
SDL_Texture *createImageTexture(const char *pPath);

SDL_Texture *createTextTexture(const char *pText, TTF_Font *pFont, SDL_Color *pColor);

int createText(int depth, float x,float y, float width, float height, char *pText, TTF_Font *pFont, SDL_Color *pColor, bool centered);

int createImage(int depth, float x, float y, float width, float height, char *pPath, bool centered);

int createButton(int depth, float x, float y, float width, float height, char *pText, TTF_Font *pFont, SDL_Color *pColor, bool centered, char *pBackgroundPath, struct callbackData data);

// function that clears all non engine render objects (depth >= 0)
void clearAll(bool freeEngine);

void removeButton(int id);

void debugForceRefresh();

void renderAll();

void checkClicked(int x, int y);

void setViewport(int screenWidth, int screenHeight);

void initGraphics(int screenWidth,int screenHeight, int windowMode, int framecap);

void shutdownGraphics();

#endif