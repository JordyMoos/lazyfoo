#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


bool init();


bool loadMedia();


void close();


SDL_Surface* loadSurface(std::string path);


SDL_Window* gWindow = nullptr;
SDL_Surface* gScreenSurface = nullptr;
SDL_Surface* gCurrentSurface = nullptr;


int main(int argc, char* args[])
{
    if ( ! init())
    {
        printf("Failed to initialize!\n");
        close();
        return 1;
    }

    if ( ! loadMedia())
    {
        printf("Failed to load media!\n");
        close();
        return 1;
    }

    bool quit = false;
    SDL_Event e;

    while ( ! quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }

        // Apply the image and swap the back buffer
        SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
        SDL_UpdateWindowSurface(gWindow);
    }

    close();
    return 0;
}


bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow("SDL Tutorial", 0, 0,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr)
    {

        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if ( ! (IMG_Init(imgFlags) & imgFlags))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    gScreenSurface = SDL_GetWindowSurface(gWindow);

    return true;
}


bool loadMedia()
{
    gCurrentSurface = loadSurface("assets/loaded.png");
    if (gCurrentSurface == nullptr)
    {
        return false;
    }

    return true;
}


void close()
{
    if (gCurrentSurface != nullptr)
    {
        SDL_FreeSurface(gCurrentSurface);
        gCurrentSurface = nullptr;
    }

    if (gWindow != nullptr)
    {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }

    SDL_Quit();
}


SDL_Surface* loadSurface(std::string path)
{
    SDL_Surface* optimizedSurface = nullptr;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        printf("unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        // Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == nullptr)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}

