/*
    TODO: ENGINE
    - engine resources (icon, fonts) need to be seperated in a sensical way from game code
      and resources. Engine should feel more seperate and able to be converted to other projects
    - (maybe) seperate some prinf output feedbacks into debug only
    - run every file in gpt4 and ask for issues
    - update all header files with the comment descriptions of the functions
*/

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "audio.h"
#include "graphics.h"
#include "engine.h"
#include "logging.h"

// buffer to hold filepath strings
// will be modified by getPath()
char path_buffer[1024];

// get the base path
char *base_path = NULL;

// initialize engine internal variable globals to NULL
SDL_Color *pEngineFontColor = NULL;
TTF_Font *pEngineFont = NULL;

// helper function to get the screen size
// TODO: consider moving graphics.c
struct ScreenSize getScreenSize(){
    // initialize video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        logMessage(error, "SDL could not initialize!\n");
        exit(1);
    }

    // use video to initialize display mode
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        logMessage(error, "SDL_GetCurrentDisplayMode failed!\n");
        exit(1);
    }

    int screenWidth = displayMode.w;
    int screenHeight = displayMode.h;
    
    char buffer[100];
    sprintf(buffer, "Inferred screen size: %dx%d\n", screenWidth, screenHeight);
    logMessage(debug, buffer);

    // return a ScreenSize struct with the screen width and height
    struct ScreenSize screenSize = {screenWidth, screenHeight};
    return screenSize;
}

// helper function to get the path to a resource
// will modify buffer automatically
char *getPath(char *path){
    // lazily set the base path used for opening resources cross platform on first call
    if(base_path == NULL){
        base_path = SDL_GetBasePath();
        if (base_path == NULL) {
            logMessage(error, "Error getting base path!\n");
        }
    }

    snprintf(path_buffer, sizeof(path_buffer), "%s../../resources/%s", base_path,path);
    return path_buffer;
}

// engine entry point, takes in the screenWidth, screenHeight and a bool flag for
// starting in debug mode
void initEngine(int screenWidth, int screenHeight, bool debug, int volume, int windowMode, int framecap, bool skipintro){
    // initialize graphics systems, creating window renderer, etc
    initGraphics(screenWidth,screenHeight,windowMode,framecap);

    // load a font for use in engine (value of global in engine.h modified)
    pEngineFont = loadFont(getPath("fonts/Nunito-Bold.ttf"), 500);

    // allocate memory for and create a pointer to our engineFontColor struct for use in graphics.c
    // TODO: check this later because i'm so tired and perplexed with this workaround to letting the fn go out of scope
    SDL_Color engineFontColor = {255, 255, 0, 255};
    pEngineFontColor = &engineFontColor;
    pEngineFontColor = malloc(sizeof(SDL_Color));
    pEngineFontColor->r = 255;
    pEngineFontColor->g = 255;
    pEngineFontColor->b = 0;
    pEngineFontColor->a = 255;

    // load a SDL_Color(s) for use in engine debug displays and startup
    SDL_Color colorWhite = {255, 255, 255, 255};

    // if we are in debug mode
    // BUG/INFO: FPS COUNTER MUST ABSOLUTELY BE THE HIGHEST DEPTH OR THE RENDER ORDER FUDGES THE NUMBERS 
    // (we need to count from the first item which is the counter to get accurate numbers (i think))
    if(debug){
        // initialize logging
        log_init();

        // display in console
        logMessage(debug, "Debug mode enabled.\n");

        // add fps counter manually to render stack with a custom id
        addRenderObject(-1, renderType_Text, 999, .0f, .0f, .15f, .1f, createTextTexture("fps: 0",pEngineFont,pEngineFontColor),false);

        // add object counter (only updates when changed)
        addRenderObject(-2, renderType_Text, 998, .0f, .1f, .15f, .1f, createTextTexture("renderObjects: 0",pEngineFont,pEngineFontColor),false);

        // add audio chunk counter (only updates when changed)
        addRenderObject(-3, renderType_Text, 997, .0f, .2f, .15f, .1f, createTextTexture("audio chunks: 0",pEngineFont,pEngineFontColor),false);
        
        // add audio chunk counter (only updates when changed)
        addRenderObject(-4, renderType_Text, 997, .0f, .3f, .15f, .1f, createTextTexture("log lines: 0",pEngineFont,pEngineFontColor),false);
    }

    // startup audio systems
    initAudio();

    // before we play our loud ass startup sound, lets check what volume the game wants
    // the engine to be at initially
    setVolume(-1, volume);

    /*
        Part of the engine startup which isnt configurable by the game is displaying
        a splash screen with the engine title and logo for 2550ms and playing a
        startup noise
    */
    if(skipintro){
        logMessage(info,"Skipping Intro.");
    }
    else{
        playSound(getPath("sfx/startup.mp3"),0,0); // play startup sound

        // create startup logo and title and save their id# into memory to destroy them after startup
        const int engineLogo = createImage(0,.5f,.5f,.35f,.4f,getPath("images/enginelogo.png"),true);

        const int engineTitle = createText(0,.5f,.3f,.3f,.1f,"yoyo engine",pEngineFont,&colorWhite,true);

        // render everything in engine queue
        renderAll(); 

        // pause on engine splash for 2550ms (TODO: consider alternatives)
        SDL_Delay(2550); 
        
        // remove startup objects
        clearAll(false);
    }

    // render everything in engine queue after splash asset removal
    renderAll();

    // debug output
    logMessage(info, "Engine Fully Initialized.\n");
} // control is now resumed by the game

// function that shuts down all engine subsystems and components ()
void shutdownEngine(){
    logMessage(info, "Shutting down engine...\n");

    /*
        TECHNICALLY this might not have been called unless getPath() 
        was called at least once, however: i dont feel like fixing 
        this as the engine will always call this internally for the 
        splash screen (unless the argv for skipping is passed) in 
        which case it doesnt matter because its called in the game 
        and nobody will ever use this engine :P
    */

    // free the engine font color
    free(pEngineFontColor);
    pEngineFontColor = NULL;

    // remove and free ALL render objects
    clearAll(true);

    // shutdown graphics
    shutdownGraphics();
    logMessage(info, "Shut down graphics.\n");

    // shutdown audio
    shutdownAudio();
    logMessage(info, "Shut down audio.\n");

    // shutdown logging
    // note: must happen before SDL because it relies on SDL path to open file
    log_shutdown();
    SDL_free(base_path); // free base path after (used by logging)

    // quit SDL (should destroy anything else i forget)
    SDL_Quit();
}