#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;




class Texture
{
private:

    SDL_Texture* texture;
    int width;
    int height;


public:

    Texture();
    ~Texture();

    void free();

    bool loadFromFile(std::string path);

    void render(int x, int y, SDL_Rect* clip = nullptr);

    int getWidth();
    int getHeight();
};




bool init();
bool loadMedia();
void close();


SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Surface* gScreenSurface = nullptr;

SDL_Surface* loadSurface(std::string path);
SDL_Texture* loadTexture(std::string path);

SDL_Rect gSpriteClips[4];
Texture gSpriteSheetTexture;






Texture::Texture()
    : texture(nullptr), width(0), height(0)
{
}


Texture::~Texture()
{
    free();
}


bool Texture::loadFromFile(std::string path)
{
    // Get rid of the preexisting texture
    free();

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        return false;
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == nullptr)
    {
        printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        SDL_FreeSurface(loadedSurface);
        return false;
    }

    width = loadedSurface->w;
    height = loadedSurface->h;
    texture = newTexture;

    SDL_FreeSurface(loadedSurface);

    return true;
}


void Texture::free()
{
    if (texture != nullptr)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        width = 0;
        height = 0;
    }
}


void Texture::render(int x, int y, SDL_Rect* clip)
{
    SDL_Rect rect = {x, y, width, height};

    if (clip != nullptr)
    {
        rect.w = clip->w;
        rect.h = clip->h;
    }

    SDL_RenderCopy(gRenderer, texture, clip, &rect);
}


int Texture::getWidth()
{
    return width;
}


int Texture::getHeight()
{
    return height;
}








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

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        gSpriteSheetTexture.render(0, 0, &gSpriteClips[0]);
        gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[1].w, 0, &gSpriteClips[1]);
        gSpriteSheetTexture.render(0, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[2]);
        gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[3].w, SCREEN_HEIGHT - gSpriteClips[3].h, &gSpriteClips[3]);

        SDL_RenderPresent(gRenderer);
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

    gRenderer = SDL_CreateRenderer(gWindow, 01, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == nullptr)
    {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
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
    if ( ! gSpriteSheetTexture.loadFromFile("assets/dots.png"))
    {
        printf("Failed to load sprite sheet texture!\n");
        return false;
    }

    // Top left
    gSpriteClips[0].x = 0;
    gSpriteClips[0].y = 0;
    gSpriteClips[0].w = 100;
    gSpriteClips[0].h = 100;

    // Top right
    gSpriteClips[1].x = 100;
    gSpriteClips[1].y = 0;
    gSpriteClips[1].w = 100;
    gSpriteClips[1].h = 100;

    // Bottom left
    gSpriteClips[2].x = 0;
    gSpriteClips[2].y = 100;
    gSpriteClips[2].w = 100;
    gSpriteClips[2].h = 100;

    // Bottom right
    gSpriteClips[3].x = 100;
    gSpriteClips[3].y = 100;
    gSpriteClips[3].w = 100;
    gSpriteClips[3].h = 100;

    return true;
}


void close()
{
    gSpriteSheetTexture.free();

    if (gRenderer != nullptr)
    {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = nullptr;
    }

    if (gWindow != nullptr)
    {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }

    IMG_Quit();
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


SDL_Texture* loadTexture(std::string path)
{
    SDL_Texture* newTexture = nullptr;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == nullptr)
        {
            printf("Unable to create texture from %s! SDL error: %s\n", path.c_str(), SDL_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}
